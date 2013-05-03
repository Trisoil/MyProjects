/******************************************************************************
*
* Copyright (c) 2012 PPLive Inc.  All rights reserved
* 
* PushPack.h
* 
* Description: 和push进行交互的命令
*             
* 
* --------------------
* 2011-02-06, youngky create
* --------------------
******************************************************************************/
#ifndef __PPLIVE_PUSH_PACK_H__
#define __PPLIVE_PUSH_PACK_H__
#include <vector>
#include <boost/shared_ptr.hpp>
#include "Protocol.h"
#include <util/archive/LittleEndianBinaryIArchive.h>
#include <util/archive/LittleEndianBinaryOArchive.h>
#include "protocol/CheckSum.h"
#include <util/buffers/SubBuffers.h>
#include <string>
#include <sstream>

using namespace protocol;
using namespace std;
namespace ns_pplive
{
	namespace ns_push
	{
		class PushPack
		{
        public:
            int PackQueryPushTask(int pversion, boost::uint32_t transaction_id, const Guid& peer_guid, 
                boost::asio::ip::udp::endpoint endpoint_, boost::uint32_t avg_upload_speed_kbs, 
                boost::uint32_t used_disk_size, boost::uint32_t upload_bandwidth_kbs,
                boost::uint32_t total_disk_size, boost::uint32_t nat_type,
                boost::uint32_t online_percent,
                UdpBuffer & send_buffer, std::vector<protocol::PlayHistoryItem>& play_history_vec);

            int UnPackQueryPushTask(int pversion, UdpBuffer& recv_buffer, boost::uint32_t transaction_id, 
                boost::uint8_t& error_code, std::vector<protocol::PushTaskItem>& push_task_vec);

		public:
			PushPack(void);
			~PushPack(void);

        private:
            template <typename PackType> 
            void PackSerialize(UdpBuffer & send_buffer,boost::uint8_t action,const PackType& packet)
            {
                util::archive::LittleEndianBinaryOArchive<boost::uint8_t> oa(send_buffer);
                oa <<int(0)<< action << packet;

                boost::uint32_t & chk_sum = const_cast<boost::uint32_t &>(
                    *boost::asio::buffer_cast<boost::uint32_t const *>(*send_buffer.data().begin()));

                if (packet.peer_version_ < protocol::PEER_VERSION_V5)
                {
                    chk_sum = check_sum_old(util::buffers::sub_buffers(send_buffer.data(), 4));
                }
                else
                {
                    chk_sum = check_sum_new(util::buffers::sub_buffers(send_buffer.data(), 4));
                }        

                //chk_sum = check_sum_new(util::buffers::sub_buffers(send_buffer.data(), 4));
            }

            template <typename PackType> 
            int UnpackUnSerialize(UdpBuffer & recv_buffer,boost::uint32_t transaction_id,PackType& packet,boost::uint8_t& error_code)
            {
                std::basic_istream<boost::uint8_t> is(&recv_buffer);
                boost::uint32_t chk_sum = 0;
                is.read((boost::uint8_t *)&chk_sum, sizeof(boost::uint32_t));	

                //这里需要chk_sum
                boost::uint8_t action = is.get();
                if (action != PackType::Action)
                {
                    err_msg_.str("");
                    err_msg_<<"action != PackType::Action,action is:"<<int(action)<<"PackType::Action is:"<<int(PackType::Action);
                    return -1;
                }

                ((protocol::Packet &)packet).PacketAction = PackType::Action;
                util::archive::LittleEndianBinaryIArchive<boost::uint8_t> ia(recv_buffer);
                ia >> packet;

                if (transaction_id != packet.transaction_id_)
                {
                    err_msg_.str("");
                    err_msg_<<"transaction_id != packet.transaction_id_,transaction_id is:"<<transaction_id<<"packet.transaction_id_ is:"<<packet.transaction_id_;
                    return -1;
                }
                error_code = packet.error_code_;
                return 0;
            }

        private:
            std::ostringstream			err_msg_;

        public:
            std::string GetErrMsg();
		};
	}
}

#endif /*__PPLIVE_PUSH_PACK_H__*/


