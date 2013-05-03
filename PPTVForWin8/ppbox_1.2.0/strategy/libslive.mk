##############################
#    live 作为静态库内置策略
#
#############################

PROJECT_COMPILE_FLAGS := $(PROJECT_COMPILE_FLAGS) -DPPBOX_STATIC_BIND_LIVE_LIB

ifeq ($(LOCAL_NAME),/packet/ppbox)
	PACKET_DEPENDS		:= $(filter-out /live/live,$(PACKET_DEPENDS))
endif

ifeq ($(LOCAL_NAME),/ppbox/live_worker)
    PROJECT_DEPENDS := $(PROJECT_DEPENDS) /live/live
endif

ifeq ($(LOCAL_NAME),/live/live)
    LOCAL_CONFIG := static $(LOCAL_CONFIG)
endif

