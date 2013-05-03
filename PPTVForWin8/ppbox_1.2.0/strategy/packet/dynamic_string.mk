###################################
#   domain_name.mk	支持打包时动态修改域名
#####################################

PACKET_POST_ACTION		:= $(PACKET_POST_ACTION) dynamic_string

dynamic_string			= file_tag $(1) dynamic_string $(DYNAMIC_STRINGS) > /dev/null
