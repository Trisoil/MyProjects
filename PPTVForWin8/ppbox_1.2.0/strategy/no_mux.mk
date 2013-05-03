###################################
#  no_mux 策略
#####################################

PROJECT_COMPILE_FLAGS := $(PROJECT_COMPILE_FLAGS) -DPPBOX_DISABLE_MUX

PROJECT_DEPENDS		:= $(filter-out /ppbox/mux, $(PROJECT_DEPENDS))
