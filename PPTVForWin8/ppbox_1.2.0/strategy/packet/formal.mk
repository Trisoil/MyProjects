###################################
#  formal.mk	正式版
#####################################

DOMAIN_NAMES		:= \
			dns_live_jump_server=\(tcp\)\(v4\)j.api.pplive.com:80 \
			dns_live2_jump_server=\(tcp\)\(v4\)live.dt.synacast.com:80 \
			dns_vod_jump_server=\(tcp\)\(v4\)dt.api.pplive.com:80 \
			dns_demux_vod_jump_server=\(tcp\)\(v4\)dt.api.pplive.com:80 \
			dns_download_vod_jump_server=\(tcp\)\(v4\)dt.api.pplive.com:80 \
			dns_vod_play_server=\(tcp\)\(v4\)epg.api.pptv.com:80 \
			dns_download_vod_drag_server=\(tcp\)\(v4\)drag.api.pplive.com:80 \
			dns_vod_drag_server=\(tcp\)\(v4\)drag.api.pplive.com:80 \
			dns_dac_server=\(tcp\)\(v4\)dc.api.pplive.com:80 \
			dns_log_server=\(tcp\)\(v4\)lg.api.pplive.com:80 \
			dns_auth_server=\(tcp\)\(v4\)c.api.pplive.com:8008 \

include $(ROOT_STRATEGY_DIRECTORY)/packet/domain_name.mk
