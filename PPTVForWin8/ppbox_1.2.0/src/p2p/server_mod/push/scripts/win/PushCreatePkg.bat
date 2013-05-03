mkdir PushServerV2
mkdir PushServerV2\Instance1
mkdir PushServerV2\Instance2
mkdir PushServerV2\Instance3
mkdir PushServerV2\Instance4
copy PushRetrieveData.py PushServerV2\.
copy run.bat PushServerV2\.
copy PushInit.py PushServerV2\.
copy push_server1.conf PushServerV2\Instance1\push_server.conf
copy push_server2.conf PushServerV2\Instance2\push_server.conf
copy push_server3.conf PushServerV2\Instance3\push_server.conf
copy push_server4.conf PushServerV2\Instance4\push_server.conf
copy push_logging.conf PushServerV2\Instance1\.
copy push_logging.conf PushServerV2\Instance2\.
copy push_logging.conf PushServerV2\Instance3\.
copy push_logging.conf PushServerV2\Instance4\.
copy pushhot.clist.config PushServerV2\Instance1\.
copy pushhot.clist.config PushServerV2\Instance2\.
copy pushhot.clist.config PushServerV2\Instance3\.
copy pushhot.clist.config PushServerV2\Instance4\.
copy newhot_content_list.xml PushServerV2\.
copy newupload_content_list.xml PushServerV2\.
copy open_task_list.xml PushServerV2\Instance1\.
copy open_task_list.xml PushServerV2\Instance2\.
copy open_task_list.xml PushServerV2\Instance3\.
copy open_task_list.xml PushServerV2\Instance4\.
copy pushtasksync2.bat PushServerV2\.
copy pushtasksync2.job PushServerV2\.
copy ..\..\..\build\msvc-90\server_mod\push\Release\push_server.exe PushServerV2\.
