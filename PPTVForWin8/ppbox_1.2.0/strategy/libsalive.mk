# libsalive

PROJECT_COMPILE_FLAGS	:= $(PROJECT_COMPILE_FLAGS) -DPPBOX_CONTAIN_PPBOX_ALIVE

ifeq ($(LOCAL_NAME),/ppbox/alive)
	PROJECT_TYPE		:= lib
	LOCAL_CONFIG		:= static $(LOCAL_CONFIG)
	PROJECT_DEPENDS		:= $(filter-out /ppbox/certify,$(PROJECT_DEPENDS))
endif

ifeq ($(LOCAL_NAME),/ppbox/certify)
	PROJECT_DEPENDS 	:= /ppbox/alive $(PROJECT_DEPENDS)
endif

ifeq ($(LOCAL_NAME),/packet/ppbox)
	PACKET_DEPENDS		:= $(filter-out /ppbox/alive, $(PACKET_DEPENDS))
endif
