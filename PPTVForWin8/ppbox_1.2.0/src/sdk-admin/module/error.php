<?php

define(ERROR_SUCCEED                        ,   0); // OK
define(ERROR_SUCCEED_WITH_INFO              ,   1); // OK，但有警告信息
define(ERROR_FAILED_SOME                    ,   2); // 部分错误
define(ERROR_SKIPPED                        ,   3); // 跳过
define(ERROR_NOT_SUPPORTED                  ,   4); // 不支持的动作
define(ERROR_INVALID_ARGV                   ,   5); // 无效参数值
define(ERROR_NO_SUCH_ITEM                   ,   6); // 不存在指定项
define(ERROR_NO_PERMISSION                  ,   7); // 不允许的操作
define(ERROR_ITEM_ALREADY_EXIST             ,   8); // 指定项已经存在
define(ERROR_CANCELED                       ,   9); // 操作被取消
define(ERROR_PROCESSING                     ,  10); // 操作正在进行
define(ERROR_MULTIPLE_ITEMS                 ,  11); // 存在多项
define(ERROR_NO_MOMERY                      ,  12); // 内存不足
define(ERROR_IO_BUSY                        ,  13); // 输入输出忙
define(ERROR_TIME_OUT                       ,  14); // 操作超时
define(ERROR_TRY_AGAIN                      ,  15); // 未准备好
define(ERROR_TOO_BUSY                       ,  16); // 操作太过频繁
define(ERROR_DATA_NOT_COMPLETE              ,  17); // 数据不完整
define(ERROR_ERROR                          ,  99); // 未知错误
                                            
define(ERROR_TRANSFER                       , 100); // 通讯错误
define(ERROR_CONNECT                        , 101); // 无法连接
define(ERROR_CONNECTION_ABORT               , 102); // 连接中断
define(ERROR_SERVER_TIMEOUT                 , 103); // 服务器响应超时
define(ERROR_TRANSFER_LAST                  , 110); // 通讯错误
                                            
define(ERROR_PACKET_FORMAT                  , 110); // 报文错误
define(ERROR_UNKNOWN_REQUEST_TYPE           , 111); // 请求类型未知
define(ERROR_MISSING_PARAMETER              , 112); // 缺少必要参数
                                            
define(ERROR_FILE_SYSTEM                    , 120); // 文件系统错误
define(ERROR_FILE_NOT_EXIST                 , 121); // 文件不存在
define(ERROR_FILE_ALREADY_EXIST             , 122); // 文件已存在
define(ERROR_NO_DISK_SPACE                  , 123); // 磁盘空间不足

    ERROR_SUCCEED_WITH_INFO         => array("OK but some thing wrong"      , "操作成功完成，但有警告信息"), 
    ERROR_FAILED_SOME               => array("some failed"                  , "部分错误"), 
    ERROR_SKIPPED                   => array("skiped"                       , "跳过"), 
    ERROR_NOT_SUPPORTED             => array("Unsupported operation"        , "不支持的动作"), 
    ERROR_INVALID_ARGV              => array("Invalid parameter"            , "无效参数值"), 
    ERROR_NO_SUCH_ITEM              => array("No such item"                 , "不存在指定项"), 
    ERROR_NO_PERMISSION             => array("No permission"                , "不允许的操作"), 
    ERROR_ITEM_ALREADY_EXIST        => array("Item already exist"           , "指定项已经存在"), 
    ERROR_CANCELED                  => array("Action is canceled"           , "操作被取消"), 
    ERROR_PROCESSING                => array("Action is processing"         , "操作正在进行"), 
    ERROR_MULTIPLE_ITEMS            => array("Multiple items exist"         , "存在多项"), 
    ERROR_NO_MOMERY                 => array("No momery"                    , "内存不足"),
    ERROR_IO_BUSY                   => array("I/O busy"                     , "输入输出忙"),
    ERROR_TIME_OUT                  => array("Operation time out"           , "操作超时"),
    ERROR_TRY_AGAIN                 => array("Not ready, try again"         , "未准备好"),
    ERROR_TOO_BUSY                  => array("Operation too busy"           , "操作太过频繁"),
    ERROR_DATA_NOT_COMPLETE         => array("Data not complete"            , "数据不完整"),
    ERROR_ERROR                     => array("error"                        , "未知错误"), 

    ERROR_TRANSFER                  => array("Transfer error"               , "通讯错误"), 
    ERROR_CONNECT                   => array("Can not connect"              , "无法连接"), 
    ERROR_CONNECTION_ABORT          => array("Connection aborted"           , "连接中断"), 
    ERROR_SERVER_TIMEOUT            => array("Server timeout"               , "服务器响应超时"), 

    ERROR_PACKET_FORMAT             => array("Packet format error"          , "报文错误"), 
    ERROR_UNKNOWN_REQUEST_TYPE      => array("Unknown request type"         , "请求类型未知"), 
    ERROR_MISSING_PARAMETER         => array("Missing parameter"            , "缺少必要参数"), 

    ERROR_FILE_SYSTEM               => array("File system error"            , "文件系统错误"), 
    ERROR_FILE_NOT_EXIST            => array("File not exist"               , "文件不存在"), 
    ERROR_FILE_ALREADY_EXIST        => array("File already exist"           , "文件已存在"), 
    ERROR_NO_DISK_SPACE             => array("Disk full"                    , "磁盘空间不足"),
);

/*
echo $error_message[1][1];
//*/

?>                                                                                               
           
