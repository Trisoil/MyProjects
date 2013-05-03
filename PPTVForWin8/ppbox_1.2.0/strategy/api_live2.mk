###################################
#  api live2 only策略
#####################################

include $(ROOT_STRATEGY_DIRECTORY)/no_live.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_mux.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_vod.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_down.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_demux_vod.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_httpd.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_rtspd.mk
