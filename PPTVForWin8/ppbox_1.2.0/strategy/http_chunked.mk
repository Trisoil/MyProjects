###################################
#  use http chunked 策略
#####################################

ifeq ($(LOCAL_NAME),/ppbox/httpd)
    PROJECT_COMPILE_FLAGS := $(PROJECT_COMPILE_FLAGS) -DPPBOX_HTTP_CHUNKED
endif

