#pragma once

namespace storage
{
	class MemResource
		:public Resource
	{
	public:
		typedef boost::shared_ptr<MemResource> p;
	public:
		static p CreateResource(size_t file_length_,boost::shared_ptr<Instance> inst_p);
		virtual void Close();

		//œ˚œ¢
	protected:	
		MemResource(size_t file_length,boost::shared_ptr<Instance> inst_p);
		//¥Ê»°
		virtual void ReadSubPiece(const SubPieceInfo& subpiece_info, const boost::asio::ip::udp::endpoint& end_point, IUploadListener::p listener);
		virtual void ReadSubPieceForPlay(const SubPieceInfo& subpiece_info, IPlayerListener::p listener) ;
		//virtual void WriteSubPieces() ;
		virtual void WriteSubPiece(const SubPieceInfo& subpiece_info, const Buffer& buffer) ;

		virtual void RemoveBlock(u_int index) ;
	private:
		virtual MD5  HashBlock(u_int block_index);
		map<SubPieceInfo,Buffer> resource_stor_;
	};
}