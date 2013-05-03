/******************************************************************************
*
* Copyright (c) 2011 PPLive Inc.  All rights reserved
* 
* ListRequestInfo.h
* 
* Description: List����������Ϣ          
* 
* --------------------
* 2011-12-12, kelvinchen create
* --------------------
******************************************************************************/

#include "Common.h"
#include "protocol/include/struct/Base.h"


using namespace std;
extern log4cplus::Logger g_logger;

namespace udptrackerag
{
    struct ListRequestInfo
    {
        ListRequestInfo();

        void UpdateMagicNum();        

        static boost::uint64_t GetMagicNum(const RID& resource_id,unsigned transaction_id);        

        //random_shuf��ʾ�Ƿ����ѡȡ�����Ϊtrue���ͻ��ڷ��صĸ���tracker��list�������ѡȡ��������tracker���ؽ����ǰlocation_percent*request_peer_count�����������ComplementPeerInfos����
        int GetPeerInfos(vector<protocol::CandidatePeerInfo>& peer_info,int location_percent,bool random_shuf) const;

        //��ȡ��tracker��ѯ����peer��������δȥ��
        unsigned GetResponsePeerCount() const;      

        //��ѯ���ж��ٸ�responseû���յ�
        unsigned GetLeftResponse();

        //��ı䴫��Ĳ������Լ���copy
        void AddPeerInfo(const boost::asio::ip::udp::endpoint& end_point,vector<protocol::CandidatePeerInfo>& peer_infos);

        //���Ͳ�ѯ��tracker��ʱ��,������tick(����)
        boost::uint64_t send_request_time;
        RID  resource_id;
        //list���ظ���end_point�����������tracker_proxy����peer
        boost::asio::ip::udp::endpoint end_point;
        unsigned short request_peer_count;
        boost::uint8_t action;

        //��¼��tracker��ѯ���Ľ��
        map<boost::asio::ip::udp::endpoint,vector<protocol::CandidatePeerInfo> > tracker_response;
        //���к�
        unsigned transaction_id; 

        //��������tracker�ȴ��ذ�����tracker_response��key��������query_tracker_num�ĸ�������˵��tracker���ظ��ˡ�
        unsigned query_tracker_num;

        //ħ����Ψһ��ʶһ����ѯ�Ķ���,ֻ��ͨ������setmagicnum������
        boost::uint64_t magic_num;  

        //client�İ汾��Ҫ��¼�������ظ���ʱ����ͻ��ˡ�
        uint16_t peer_version;

    private:
        //����peer,vector���Ѿ��е����ݣ��Ǹ����ȼ������ݣ�������Ҫ���������������ݣ�Ŀǰ��ȡ���Ǵӷ��ؽ���ĺ�˲�����䡣
        //�������������ǰ����peer_info���Ѿ��Ǹ����ȼ��������ˡ�
        void ComplementPeerInfos(vector<protocol::CandidatePeerInfo>& peer_info) const;
    };
}