###################################
# atv.mk	亚洲电视网络
#####################################

DOMAIN_NAMES		:= \
			dns_vod_play_server=\(tcp\)\(v4\)play.atv-net.com:80 \
			dns_live2_play_server=\(tcp\)\(v4\)play.atv-net.com:80 \
			dns_dac_server=\(tcp\)\(v4\)dac.150hi.com:80 \
			dns_log_server=\(tcp\)\(v4\)log.150hi.com:80 \
			dns_auth_server=\(tcp\)\(v4\)cert.150hi.com:8008 \

include $(ROOT_STRATEGY_DIRECTORY)/packet/domain_name.mk
