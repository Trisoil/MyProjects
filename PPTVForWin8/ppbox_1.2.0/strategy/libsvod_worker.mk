################################
#  vod_worker作为静态库内置策略
#
#
################################


PROJECT_COMPILE_FLAGS := $(PROJECT_COMPILE_FLAGS) -DPPBOX_CONTAIN_VOD_WORKER

ifeq ($(LOCAL_NAME),/packet/ppbox)
	PACKET_DEPENDS		:= $(filter-out /ppbox/vod_worker,$(PACKET_DEPENDS))
endif

ifeq ($(LOCAL_NAME),/ppbox/vod_worker)
    PROJECT_TYPE := lib
    LOCAL_CONFIG := static $(LOCAL_CONFIG)
endif

ifeq ($(LOCAL_NAME),/ppbox/vod)
    PROJECT_DEPENDS := /ppbox/vod_worker $(PROJECT_DEPENDS)
endif
