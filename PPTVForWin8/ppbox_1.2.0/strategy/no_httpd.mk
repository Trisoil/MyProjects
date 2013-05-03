###################################
#  no_httpd策略
#####################################

PROJECT_COMPILE_FLAGS := $(PROJECT_COMPILE_FLAGS) -DPPBOX_DISABLE_HTTPD

ifeq ($(LOCAL_NAME),/ppbox/ppbox)
	PROJECT_DEPENDS 	:= $(filter-out /ppbox/httpd,$(PROJECT_DEPENDS))
endif

