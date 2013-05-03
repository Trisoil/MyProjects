###################################
#  onelib策略
#####################################

include $(ROOT_STRATEGY_DIRECTORY)/libsvod.mk
include $(ROOT_STRATEGY_DIRECTORY)/libsvod_worker.mk
include $(ROOT_STRATEGY_DIRECTORY)/libspeer.mk
include $(ROOT_STRATEGY_DIRECTORY)/libspeer_worker.mk
include $(ROOT_STRATEGY_DIRECTORY)/libslive.mk
include $(ROOT_STRATEGY_DIRECTORY)/libslive_worker.mk
include $(ROOT_STRATEGY_DIRECTORY)/libsalive.mk
PROJECT_COMPILE_FLAGS := $(PROJECT_COMPILE_FLAGS) -DPPBOX_SINGLE_PROCESS
