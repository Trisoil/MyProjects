###################################
#  android p2p策略
#####################################

include $(ROOT_STRATEGY_DIRECTORY)/libspeer.mk
include $(ROOT_STRATEGY_DIRECTORY)/libspeer_worker.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_live.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_vod.mk
PROJECT_COMPILE_FLAGS := $(PROJECT_COMPILE_FLAGS) -DPPBOX_SINGLE_PROCESS
include $(ROOT_STRATEGY_DIRECTORY)/no_down.mk
include $(ROOT_STRATEGY_DIRECTORY)/no_cert.mk
