###################################
#  no_peer 策略
#####################################

PROJECT_COMPILE_FLAGS := $(PROJECT_COMPILE_FLAGS) -DPPBOX_DISABLE_PEER

DIRECTORY_SUBS          := $(filter-out peer peer_worker,$(DIRECTORY_SUBS))

PROJECT_DEPENDS		:= $(filter-out /ppbox/peer /ppbox/peer_worker,$(PROJECT_DEPENDS))

ifeq ($(LOCAL_NAME),/packet/ppbox)
	PACKET_DEPENDS		:= $(filter-out /peer/peer /ppbox/peer_worker,$(PACKET_DEPENDS))
endif
