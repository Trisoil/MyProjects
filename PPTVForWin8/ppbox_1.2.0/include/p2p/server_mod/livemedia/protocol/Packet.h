// Packet.h

#ifndef _PROTOCOL_PACKET_H_
#define _PROTOCOL_PACKET_H_

#include "protocol/Structs.h"
#include "protocol/Base.h"

namespace protocol
{

    const boost::uint32_t PEER_VERSION = 0x00000103;

    struct Packet
	{
		static boost::uint32_t NewTransactionID()
		{
			static boost::uint32_t glocal_transaction_id = 0;
			glocal_transaction_id ++;
			return glocal_transaction_id;
		}
	
        template <typename Archive>
        void serialize(Archive & ar)
        {
            ar & transaction_id_;
        }

		virtual boost::uint32_t length() const
		{
//			return sizeof(transaction_id_);
			// packet header
			return 9;
		}

		boost::uint8_t GetAction() const
		{
			return PacketAction;
		}

        boost::asio::ip::udp::endpoint end_point_;
		boost::uint8_t PacketAction;
        boost::uint32_t transaction_id_;
	};

	template <boost::uint8_t action>
    struct PacketT
    {
		static const boost::uint8_t Action = action;
    };

}

#endif // _PROTOCOL_PACKET_H_
