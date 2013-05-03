#pragma once

#include "util/Util.h"
#include <boost/asio/io_service.hpp>
#include <boost/shared_array.hpp>
#include <boost/filesystem.hpp>
#include "Piece.h"
// namespace boost{
// 	namespace asio{
// 		class io_service;
// 	}
// }
#include <framework/timer/Timer.h>
#include <framework/timer/AsioTimerManager.h>
// 
// #include <boost/asio/streambuf.hpp>

#define CHECK_POINT_TAG_SCALE 60  // 60 sec.
#define CHECK_POINT_OFFSET	  101

#define ID_CHECK_SCALE		  3600 // 1 hour.

const static std::string piece_extension = ".piece";
const static std::string flv_extension = ".flv";

struct lite_buffer
{
	boost::shared_array<unsigned char> buf_;
	std::size_t size_;
	lite_buffer() : size_(0) {}
	lite_buffer(std::size_t size) : size_(size), buf_(new unsigned char[size])
	{}
 
	template <typename Archive/*, typename lite_buffer*/>
	void serialize(Archive & ar)
	{
		ar & framework::container::make_array(buf_.get(), size_);
	}
};

typedef boost::shared_ptr<lite_buffer> liter_buffer_ptr;
typedef boost::shared_ptr<std::vector<liter_buffer_ptr> > piece_ptr;

namespace boost
{
	template<typename T>
	struct is_pointer<boost::shared_ptr<T> > : boost::mpl::true_ {};
}

class CPieceWriter
{
public:
	CPieceWriter(const std::string& path, const boost::uint32_t& piece_scale, const boost::uint32_t& piece_life_span);
	~CPieceWriter();
	virtual bool Start();
	virtual void Run() {}
	virtual void AsyncWritePiece(const piece_ptr& piece, 
		const boost::uint32_t& seek_point_pos, const boost::uint32_t& media_ts = 0);
	virtual void ReaderError();


	void SetCheckpointScale(const boost::uint32_t& scale) { checkpoint_scale_ = scale; }

	struct piece_file_info
	{
		boost::uint32_t id_;
		std::string path_;
		std::size_t size_;
		piece_file_info (boost::uint32_t id
			, const std::string& path, const std::size_t size)
			: id_(id), path_(path), size_(size) {}
	};

private:

	virtual void HandleReaderError();
	virtual void WriteOneSpecialPiece(int special_type = 0) {}

	virtual void ReportReaderStatus(bool status);

protected:

	virtual void HandleWritePiece(const piece_ptr& piece, 
		const boost::uint32_t& seek_point_pos, const boost::uint32_t& media_ts);
	virtual void OnClockAlarm();
 
	std::size_t DecreasePiece(const std::size_t& remove_size = 1);
	boost::uint32_t GetUTC();

	bool NeedCheckPointTag() { return !(current_piece_id_ % checkpoint_scale_); }
	bool NeedCheckID() { return !(current_piece_id_ % ID_CHECK_SCALE); }

	boost::filesystem::path file_path_;
	boost::shared_ptr<boost::asio::io_service::work> work_;
	boost::asio::io_service ios_;

	piece_ptr header_piece_;
	boost::uint32_t current_piece_id_;

	enum {
		STAT_FIRST_PIECE,
		STAT_SECOND_PIECE,
		STAT_NORMAL
	};
	int stat_;

	std::deque<piece_file_info> piece_file_queue_;

	framework::timer::AsioTimerManager clock_manager_;
	framework::timer::PeriodicTimer clock_;

	bool reader_ok_;
	boost::uint32_t piece_life_span_;

	int writing_piece_num_;

	liter_buffer_ptr offair_tag_;
	liter_buffer_ptr checkpoint_tag_;
	liter_buffer_ptr onmetadata_tag_;
	liter_buffer_ptr videoheader_tag_;
	liter_buffer_ptr audioheader_tag_;

	boost::uint32_t piece_scale_;
	boost::uint32_t checkpoint_scale_;

	std::string guid_str_;

	boost::uint32_t cur_media_ts_;

	int report_alarm_;
};

class CPPLPieceWriter : public CPieceWriter
{
public:
	CPPLPieceWriter(const std::string& path, const boost::uint32_t& piece_scale
		, const boost::uint32_t& piece_life_span, const boost::uint32_t& head_piece_life_span);

	void Run();
private:
	void HandleWritePiece(const piece_ptr& piece, 
		const boost::uint32_t& seek_point_pos, const boost::uint32_t& media_ts);
//	void WriteHeadPiece(const piece_ptr& piece, const boost::uint32_t& seek_point_pos);
	void WriteOneSpecialPiece(int special_type = 0);
	void OnClockAlarm();

	PIECE_HEADER piece_header_;

	boost::uint32_t header_piece_id_;

	std::deque<CPieceWriter::piece_file_info> head_piece_file_queue_;

	boost::uint32_t head_piece_life_span_;

	boost::uint32_t last_seek_point_piece_id_;
};

class CFLVPieceWriter : public CPieceWriter
{
public:
	CFLVPieceWriter(const std::string& path, const boost::uint32_t& piece_scale, const boost::uint32_t& piece_life_span)
		: CPieceWriter(path, piece_scale, piece_life_span) {}
	~CFLVPieceWriter(void);

	void Run();

private:
	void HandleWritePiece(const piece_ptr& piece, 
		const boost::uint32_t& seek_point_pos, const boost::uint32_t& media_ts);
	void WriteOneSpecialPiece(int special_type = 0);

	boost::uint32_t header_piece_len_;
};

class CPPLBlockWriter : public CPieceWriter
{
public:
	CPPLBlockWriter(const std::string& path, const boost::uint32_t& piece_scale, const boost::uint32_t& piece_life_span);
	~CPPLBlockWriter(void);

	void Init(boost::uint32_t& media_ts);
	void Run();

private:
	void HandleWritePiece(const piece_ptr& piece, 
		const boost::uint32_t& seek_point_pos, const boost::uint32_t& media_ts);
	void WriteOneSpecialPiece(int special_type = 0);

	boost::uint32_t		header_piece_len_;
	FLV_BLOCK_HEADER	block_header_;

	// Subpiece Checksum
	boost::uint8_t	piece_buf_[FLV_BLOCK_SIZE];
	boost::uint32_t piece_len_;

};