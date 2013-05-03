##############################
#    vod 作为静态库内置策略
#
#############################

PROJECT_COMPILE_FLAGS := $(PROJECT_COMPILE_FLAGS) -DPPBOX_STATIC_BIND_VOD_LIB

ifeq ($(LOCAL_NAME),/packet/ppbox)
	PACKET_DEPENDS		:= $(filter-out /vod,$(PACKET_DEPENDS))
endif

ifeq ($(LOCAL_NAME),/ppbox/vod_worker)
    PROJECT_DEPENDS := $(PROJECT_DEPENDS) /vod
endif

ifeq ($(LOCAL_NAME),/vod)
    LOCAL_CONFIG := static $(LOCAL_CONFIG)
endif

