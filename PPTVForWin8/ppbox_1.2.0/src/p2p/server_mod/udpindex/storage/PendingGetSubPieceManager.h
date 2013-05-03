#pragma once

namespace storage
{
	struct PendingGetSubPiece
	{
		SubPieceInfo subpiece_info_;
		boost::asio::ip::udp::endpoint end_point_;
		IUploadListener::p listener_;
		RequestSubPiecePacket::p packet_;
		PendingGetSubPiece(const SubPieceInfo& subpiece_info, const boost::asio::ip::udp::endpoint& end_point, RequestSubPiecePacket::p packet, IUploadListener::p listener)
			:subpiece_info_(subpiece_info),end_point_(end_point),listener_(listener),packet_(packet){;};
		PendingGetSubPiece()
			:subpiece_info_(),end_point_(),listener_(){;};

	};

	class PendingGetSubPieceManager
		:public boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<PendingGetSubPieceManager> p;
		static PendingGetSubPieceManager::p  Create() { return PendingGetSubPieceManager::p(new PendingGetSubPieceManager()); };
	public:

		void PushReq(const PendingGetSubPiece &req) 
		{
			std::map<size_t,std::list<PendingGetSubPiece>>::iterator block_pend_req_list = manager_.find(req.subpiece_info_.block_index_);
			if(block_pend_req_list==manager_.end())
			{
				std::list<PendingGetSubPiece> list_t;
				list_t.push_back(req);
				manager_.insert(make_pair(req.subpiece_info_.block_index_,list_t));
				return;
			}
			block_pend_req_list->second.push_back(req);
			return;
		};

		bool PopReq(PendingGetSubPiece &req,const size_t block_index)
		{
			std::map<size_t,std::list<PendingGetSubPiece>>::iterator it = manager_.find(block_index);
			if(it!=manager_.end())
			{
				assert(!it->second.empty());
				req = it->second.front();
				it->second.pop_front();
				if(it->second.empty())
				{
					manager_.erase(it);
				}
				return true;
			}
			return false;
		}

	protected:
		PendingGetSubPieceManager() {};
	private:
		std::map<size_t,std::list<PendingGetSubPiece>> manager_;
	};
}
