###################################
#  no_live 策略
#####################################

PROJECT_COMPILE_FLAGS	:= $(PROJECT_COMPILE_FLAGS) -DPPBOX_DISABLE_LIVE

DIRECTORY_SUBS		:= $(filter-out live live_worker,$(DIRECTORY_SUBS))

PROJECT_DEPENDS		:= $(filter-out /ppbox/live /ppbox/live_worker,$(PROJECT_DEPENDS))

ifeq ($(LOCAL_NAME),/packet/ppbox)
	PACKET_DEPENDS		:= $(filter-out /live/live /ppbox/live_worker,$(PACKET_DEPENDS))
endif
