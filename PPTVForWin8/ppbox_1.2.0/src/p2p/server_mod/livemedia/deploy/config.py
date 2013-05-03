#!/usr/bin/env python
# -*- coding: utf-8 -*-
#


submit_interval_in_ms = 60000                    # 汇报给服务器的时间,单位:毫秒
check_config_interval_in_ms = 60000              # 更新服务器配置的时间,单位:毫秒

program_name = "livemedia-linux-x64-gcc44-mt"     # 可执行程序的名字
server_ip  = "127.0.0.1"

submit_host = "live-cms.g1d.net"                  # 这是配置 live-cms 的地址,以后上线直接配置成域名
submit_port = 80                                # 这是 live-cms 的端口
submit_post_path = "/ReportStatus.jsp"          # 这是接口路径,一般不需要修改
submit_config_path = "/GetConfiguation.jsp"     # 这是接口路径,一般不需要修改

submit_host2 = "udpmonitor.g1d.net"                  
submit_port2 = 80                                
submit_post_path2 = "/ReportServerStatus.php"          

post_file_name = "status.xml"                   # 一般不需要修改

python_bin = "/usr/bin/python"                  # python 程序的路径,如果不是这个目录,请修改
killall_bin = "/usr/bin/killall"                # killall 程序的路径,如果不是这个目录,请修改

