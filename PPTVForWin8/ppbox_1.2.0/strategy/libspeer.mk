##############################
#    peer 作为静态库内置策略
#
#############################

PROJECT_COMPILE_FLAGS := $(PROJECT_COMPILE_FLAGS) -DPPBOX_STATIC_BIND_PEER_LIB

ifeq ($(LOCAL_NAME),/packet/ppbox)
	PACKET_DEPENDS		:= $(filter-out /peer/peer,$(PACKET_DEPENDS))
endif

ifeq ($(LOCAL_NAME),/ppbox/peer_worker)
    PROJECT_DEPENDS := $(PROJECT_DEPENDS) /peer/peer
endif

ifeq ($(LOCAL_NAME),/peer/peer)
    LOCAL_CONFIG := static $(LOCAL_CONFIG)
endif

