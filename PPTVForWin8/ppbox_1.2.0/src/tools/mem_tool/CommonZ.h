#pragma once

#define LOGS(p) printf("%s [%s][%d]\n",p,__FILE__,__LINE__);

#define RW_MODULE "r"

#define CLEAN_BUF(p,i) memset(p,0,i);

#define BUF_SIZE 256

#define HEAD_KEY "kB"
#define TYPE_KEY "-xp"

#define NAME_KEY '/'

#define PROCESS_NAME "Owner"




#define KB_SIZE     "Size"
#define KB_RSS      "Rss"
#define KB_SCLEAN "Shared_Clean"
#define KB_SDIRTY "Shared_Dirty"
#define KB_PCLEAN "Private_Clean"
#define KB_PDIRTY "Private_Dirty"


// 文件名 [5/3/2011 zenzhang]

//代码段
#define CODE_TOTAL "Code_Total.txt"
#define CODE_SHARED "Code_Virtul.txt"
#define CODE_PHYMEM "Code_Phymem.txt"

#define CODE_SCLEAN "Code_SharedClean.txt"
#define CODE_SDIRTY "Code_SharedDirty.txt"

#define CODE_PCLEAN "Code_PrivateClean.txt"
#define CODE_PDIRTY "Code_PrivateDirty.txt"

//数据段
#define DATA_TOTAL "Data_Total.txt"
#define DATA_SHARED "Data_Virtual.txt"
#define DATA_PHYMEM "Data_Phymem.txt"

#define DATA_SCLEAN "Data_SharedClean.txt"
#define DATA_SDIRTY "Data_SharedDirty.txt"

#define DATA_PCLEAN "Data_PrivateClean.txt"
#define DATA_PDIRTY "Data_PrivateDirty.txt"

