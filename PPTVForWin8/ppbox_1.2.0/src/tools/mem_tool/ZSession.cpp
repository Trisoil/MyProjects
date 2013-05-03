#include "ZSession.h"
#include <stdio.h>
#include <stdlib.h>




#include <string.h>

const char* g_cStr = "ShaMem	PhyMem	SClean	SDirty	PClean	PDirty\n";




ZSession::ZSession()
{
    //m_bFirst = false;
}

ZSession::~ZSession()
{

}

void ZSession::insert(std::string& strName,MEM_DATA& _data)
{
    ZIter iter = m_VecData.begin();
    for(; iter != m_VecData.end(); ++iter)
    {
        if ( strName == (*iter).strKey )
        {
            break;
        }
    }

    

    if (iter == m_VecData.end())
    {

        m_VecData.push_back(_data);
    }
    else
    {
  

        (*iter) = _data;

    }



}



void ZSession::AddTotalFile()
{
    if (m_VecData.size() < 1)
    {
        return;
    }

    int iSize = 0;
    char szBuf[1024] = {0};
    char szNum[128] = {0};

    ZIter iter = m_VecData.begin();

    std::string strName;

    for(; iter != m_VecData.end(); ++iter)
    {
        /*if (3 == (*iter).iType)
        {
            continue;
        }
        */

        iSize = sprintf(szNum,"%u	",(*iter).iSize);
        AddFile(m_fShared,szNum,iSize);


        CLEAN_BUF(szNum,sizeof(szNum));
        iSize = sprintf(szNum,"%u	",(*iter).iRss);
        AddFile(m_fPhymem,szNum,iSize);


        CLEAN_BUF(szNum,sizeof(szNum));
        iSize = sprintf(szNum,"%u	",(*iter).iShareClean);
        AddFile(m_fSClean,szNum,iSize);


        CLEAN_BUF(szNum,sizeof(szNum));
        iSize = sprintf(szNum,"%u	",(*iter).iShareDirty);
        AddFile(m_fSDirty,szNum,iSize);

  
        CLEAN_BUF(szNum,sizeof(szNum));
        iSize = sprintf(szNum,"%u	",(*iter).iShareDirty);
        AddFile(m_fPClean,szNum,iSize);

        CLEAN_BUF(szNum,sizeof(szNum));
        iSize = sprintf(szNum,"%u	",(*iter).iPriveDirty);
        AddFile(m_fPDirty,szNum,iSize);
  
    }
    AddFile(m_fShared,"\n",1);
    AddFile(m_fPhymem,"\n",1);
    AddFile(m_fSClean,"\n",1);
    AddFile(m_fSDirty,"\n",1);
    AddFile(m_fPClean,"\n",1);
    AddFile(m_fPDirty,"\n",1);

    


  /*  int iSize = 0;
    char szBuf[1024] = {0};
    char szNum[128] = {0};

    MEM_DATA _data;

    unsigned int i1 = 0;
    unsigned int i2 = 0;
    unsigned int i3 = 0; 
    unsigned int i4 = 0; 
    unsigned int i5 = 0; 
    unsigned int i6 = 0;		

    ZIter iter = m_mapData.begin();*/
/*

    if (!IsWriteTile())
    {
           
        for (; iter != m_mapData.end(); ++iter)
        {
  
            iSize = sprintf(szNum,"%s	",iter->first.c_str());
            AddFile(m_fShared,szNum,iSize);
            AddFile(m_fPhymem,szNum,iSize);
            AddFile(m_fSClean,szNum,iSize);
            AddFile(m_fSDirty,szNum,iSize);
            AddFile(m_fPClean,szNum,iSize);
            AddFile(m_fPDirty,szNum,iSize);
        }

        AddFile(m_fShared,"\n",1);
        AddFile(m_fPhymem,"\n",1);
        AddFile(m_fSClean,"\n",1);
        AddFile(m_fSDirty,"\n",1);
        AddFile(m_fPClean,"\n",1);
        AddFile(m_fPDirty,"\n",1);

    }

    
    iter = m_mapData.begin();
    for (; iter != m_mapData.end(); ++iter)
    {
        _data = iter->second;

        i1 += _data.iSize;
        iSize = sprintf(szNum,"%u	",_data.iSize);
        AddFile(m_fShared,szNum,iSize);

        i2 += _data.iRss;  
        CLEAN_BUF(szNum,sizeof(szNum));
        iSize = sprintf(szNum,"%u	",_data.iRss);
        AddFile(m_fPhymem,szNum,iSize);

        i3 += _data.iShareClean; 
        CLEAN_BUF(szNum,sizeof(szNum));
        iSize = sprintf(szNum,"%u	",_data.iShareClean);
        AddFile(m_fSClean,szNum,iSize);

        i4 += _data.iShareDirty;  
        CLEAN_BUF(szNum,sizeof(szNum));
        iSize = sprintf(szNum,"%u	",_data.iShareDirty);
        AddFile(m_fSDirty,szNum,iSize);

        i5 += _data.iShareDirty;
        CLEAN_BUF(szNum,sizeof(szNum));
        iSize = sprintf(szNum,"%u	",_data.iShareDirty);
        AddFile(m_fPClean,szNum,iSize);

        i6 += _data.iPriveDirty;
        CLEAN_BUF(szNum,sizeof(szNum));
        iSize = sprintf(szNum,"%u	",_data.iPriveDirty);
        AddFile(m_fPDirty,szNum,iSize);
    }

    iSize = sprintf(szBuf,"%u	%u	%u	%u	%u	%u\n",i1,i2,i3,i4,i5,i6);
    AddFile(m_fTotal,szBuf,iSize);


    AddFile(m_fShared,"\n",1);
    AddFile(m_fPhymem,"\n",1);
    AddFile(m_fSClean,"\n",1);
    AddFile(m_fSDirty,"\n",1);
    AddFile(m_fPClean,"\n",1);
    AddFile(m_fPDirty,"\n",1);*/

}
void ZSession::AddSharedFile()
{
 
}
void ZSession::AddPhymemFile()
{

}

void ZSession::AddScleanFile()
{

}
void ZSession::AddSdiryFile()
{

}
void ZSession::AddPcleanFile()
{

}

void ZSession::AddpDirtyFile()
{

}

void ZSession::AddTitle()
{
    

    
    std::string strName;
    std::string strTitle;
    ZIter iter = m_VecData.begin();

    for (; iter != m_VecData.end(); ++iter)
    {
        strName = (*iter).strKey + "(";    
        strName += (*iter).strName;
        strName += ")";
        strTitle += strName;
        strTitle += "	";
    }
    strTitle += "\n";


    m_fShared.AddHead(strTitle.c_str(),strTitle.length());
    m_fPhymem.AddHead(strTitle.c_str(),strTitle.length());
    m_fSClean.AddHead(strTitle.c_str(),strTitle.length());
    m_fSDirty.AddHead(strTitle.c_str(),strTitle.length());
    m_fPClean.AddHead(strTitle.c_str(),strTitle.length());
    m_fPDirty.AddHead(strTitle.c_str(),strTitle.length());
 

}   

void ZSession::AddFile(ZFile& file,const char* pStr, const int iSize)
{
    file.write(pStr,iSize);
}

void ZSession::Clean()
{
    ZPrint();
  

    ZIter iter = m_VecData.begin();
    for (; iter != m_VecData.end(); ++iter)
    {
        (*iter).iType = 3;
        (*iter).iSize = 0;
        (*iter).iRss = 0;
        (*iter).iPriveClean = 0;
        (*iter).iPriveDirty = 0;
        (*iter).iShareClean = 0;
        (*iter).iShareDirty = 0;
		//(*iter).strName = "";

    }

}

void ZSession::ZPrint()
{
    if(1 > m_VecData.size())
    {
        return;
    }

    AddTotalFile();
  /*  AddSharedFile();
    AddPhymemFile();

    AddScleanFile();
    AddSdiryFile();
    AddPcleanFile();
    AddpDirtyFile();*/
}
