###################################
#   domain_name.mk	支持打包时动态修改域名
#####################################

PACKET_POST_ACTION		:= $(PACKET_POST_ACTION) domain_name

domain_name			= file_tag $(1) domain_name $(DOMAIN_NAMES) > /dev/null
