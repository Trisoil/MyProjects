###################################
#  ipad策略
#####################################

include $(ROOT_STRATEGY_DIRECTORY)/onelib.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_live.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_cert.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_vod.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_rtspd.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_debugproxy.mk 

ifeq ($(LOCAL_NAME),/ppbox/ppbox)
	LOCAL_CONFIG		:= static2 $(LOCAL_CONFIG) 
endif

