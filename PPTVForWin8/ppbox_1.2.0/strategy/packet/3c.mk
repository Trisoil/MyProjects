###################################
#  3c.mk	山寨版
#####################################

DOMAIN_NAMES		:= \
			dns_live_jump_server=\(tcp\)\(v4\)livejump.150hi.com:80 \
			dns_live2_jump_server=\(tcp\)\(v4\)live.dt.150hi.com:80 \
			dns_vod_jump_server=\(tcp\)\(v4\)k.150hi.com:80 \
			dns_demux_vod_jump_server=\(tcp\)\(v4\)k.150hi.com:80 \
			dns_download_vod_jump_server=\(tcp\)\(v4\)k.150hi.com:80 \
			dns_vod_play_server=\(tcp\)\(v4\)epg.api.150hi.com:80 \
			dns_download_vod_drag_server=\(tcp\)\(v4\)drag.150hi.com:80 \
			dns_vod_drag_server=\(tcp\)\(v4\)drag.150hi.com:80 \
			dns_dac_server=\(tcp\)\(v4\)dac.150hi.com:80 \
			dns_log_server=\(tcp\)\(v4\)log.150hi.com:80 \
			dns_auth_server=\(tcp\)\(v4\)cert.150hi.com:8008 \

DYNAMIC_STRINGS		:= \
			string_vod_type=ppstb \

include $(ROOT_STRATEGY_DIRECTORY)/packet/domain_name.mk
include $(ROOT_STRATEGY_DIRECTORY)/packet/dynamic_string.mk
