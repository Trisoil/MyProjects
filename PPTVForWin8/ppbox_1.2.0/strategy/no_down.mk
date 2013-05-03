###################################
#  no_down 策略
#####################################

PROJECT_COMPILE_FLAGS := $(PROJECT_COMPILE_FLAGS) -DPPBOX_DISABLE_DOWNLOAD

PROJECT_DEPENDS		:= $(filter-out /ppbox/download, $(PROJECT_DEPENDS))


ifeq ($(LOCAL_NAME),/ppbox/peer_worker)
    PROJECT_COMPILE_FLAGS := $(PROJECT_COMPILE_FLAGS) -DPEER_BUFFER_SIZE=25
endif
