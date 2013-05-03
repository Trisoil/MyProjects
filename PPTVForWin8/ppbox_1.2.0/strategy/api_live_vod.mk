###################################
#  api 带点直播 多进程策略
#####################################

include $(ROOT_STRATEGY_DIRECTORY)/no_mux.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_demux_live2.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_down.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_peer.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_httpd.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_rtspd.mk
