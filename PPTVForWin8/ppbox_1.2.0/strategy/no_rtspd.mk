###################################
#  no_rtspd策略
#####################################

PROJECT_COMPILE_FLAGS := $(PROJECT_COMPILE_FLAGS) -DPPBOX_DISABLE_RTSPD

ifeq ($(LOCAL_NAME),/ppbox/ppbox)
	PROJECT_DEPENDS 	:= $(filter-out /ppbox/rtspd,$(PROJECT_DEPENDS))
endif

