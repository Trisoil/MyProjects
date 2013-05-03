###################################
#  no_dac策略
#####################################

PROJECT_COMPILE_FLAGS := $(PROJECT_COMPILE_FLAGS) -DPPBOX_DISABLE_DAC

PROJECT_DEPENDS 	:= $(filter-out /ppbox/dac,$(PROJECT_DEPENDS))

