###################################
#  no_vod 策略
#####################################

PROJECT_COMPILE_FLAGS := $(PROJECT_COMPILE_FLAGS) -DPPBOX_DISABLE_VOD

DIRECTORY_SUBS          := $(filter-out vod vod_worker,$(DIRECTORY_SUBS))

PROJECT_DEPENDS		:= $(filter-out /ppbox/vod /ppbox/vod_worker,$(PROJECT_DEPENDS))

ifeq ($(LOCAL_NAME),/packet/ppbox)
	PACKET_DEPENDS		:= $(filter-out /vod /ppbox/vod_worker,$(PACKET_DEPENDS))
endif
