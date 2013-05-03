###################################
#  no_live 策略
#####################################

PROJECT_COMPILE_FLAGS := $(PROJECT_COMPILE_FLAGS) -DPPBOX_DISABLE_CERTIFY

PROJECT_DEPENDS		:= $(filter-out /ppbox/certify, $(PROJECT_DEPENDS))

ifeq ($(LOCAL_NAME),/packet/ppbox)
	PACKET_DEPENDS		:= $(filter-out /ppbox/alive, $(PACKET_DEPENDS))
endif
