//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#ifndef UDPTRACKER_MINIDUMP_H
#define UDPTRACKER_MINIDUMP_H

#include <windows.h>
#include <tchar.h>
#include <dbghelp.h>
#include <stdio.h>
#include <crtdbg.h>


///////////////////////////////////////////////////////////////////////////////
// Directives 
//

#pragma comment ( lib, "dbghelp.lib" )


///////////////////////////////////////////////////////////////////////////////
// Function declarations 
//

LONG WINAPI CreateMiniDump( EXCEPTION_POINTERS* pep ); 

BOOL CALLBACK MyMiniDumpCallback(
                                 PVOID                            pParam, 
                                 const PMINIDUMP_CALLBACK_INPUT   pInput, 
                                 PMINIDUMP_CALLBACK_OUTPUT        pOutput 
                                 ); 

///////////////////////////////////////////////////////////////////////////////
// Minidump creation function 
//

LONG WINAPI CreateMiniDump( EXCEPTION_POINTERS* pep ) 
{
    // Open the file 

    time_t t = time(0);
    char tmp[64];
    strftime(tmp, 64, "%X", localtime(&t));

    for (int i=0; i<strlen(tmp); i++)
    {
        if (tmp[i] == ':')
            tmp[i] = '_';
    }

    wchar_t tt[64];

    // ×ª»»ANSI×Ö·û´®ÎªUNICODE×Ö·û´® 
    MultiByteToWideChar(CP_ACP, 0, tmp, -1, tt, 64);

    string ti(tmp);

    string dump_name = "MiniDump.dmp";
    HANDLE hFile = CreateFile( (ti+dump_name).c_str(), GENERIC_READ | GENERIC_WRITE, 
        0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );  

    if( ( hFile != NULL ) && ( hFile != INVALID_HANDLE_VALUE ) ) 
    {
        // Create the minidump 

        MINIDUMP_EXCEPTION_INFORMATION mdei; 

        mdei.ThreadId           = GetCurrentThreadId(); 
        mdei.ExceptionPointers  = pep; 
        mdei.ClientPointers     = FALSE; 

        MINIDUMP_CALLBACK_INFORMATION mci; 

        mci.CallbackRoutine     = (MINIDUMP_CALLBACK_ROUTINE)MyMiniDumpCallback; 
        mci.CallbackParam       = 0; 

        MINIDUMP_TYPE mdt       = (MINIDUMP_TYPE)(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory); 

        BOOL rv = MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), 
            hFile, mdt, (pep != 0) ? &mdei : 0, 0, &mci ); 

        if( !rv ) 
            printf("MiniDumpWriteDump failed. Error: %u \n", GetLastError());
        else 
            printf("Minidump created.\n");

        // Close the file 

        CloseHandle( hFile ); 

    }
    else 
    {
        printf("CreateFile failed. Error: %u \n", GetLastError());
    }

    TerminateProcess(GetCurrentProcess(), 0);

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// Custom minidump callback 
//

BOOL CALLBACK MyMiniDumpCallback(
                                 PVOID                            pParam, 
                                 const PMINIDUMP_CALLBACK_INPUT   pInput, 
                                 PMINIDUMP_CALLBACK_OUTPUT        pOutput 
                                 ) 
{
    BOOL bRet = FALSE; 


    // Check parameters 

    if( pInput == 0 ) 
        return FALSE; 

    if( pOutput == 0 ) 
        return FALSE; 


    // Process the callbacks 

    switch( pInput->CallbackType ) 
    {
    case IncludeModuleCallback: 
        {
            // Include the module into the dump 
            bRet = TRUE; 
        }
        break; 

    case IncludeThreadCallback: 
        {
            // Include the thread into the dump 
            bRet = TRUE; 
        }
        break; 

    case ModuleCallback: 
        {
            // Does the module have ModuleReferencedByMemory flag set ? 

            if( !(pOutput->ModuleWriteFlags & ModuleReferencedByMemory) ) 
            {
                // No, it does not - exclude it 

                wprintf( L"Excluding module: %s \n", pInput->Module.FullPath ); 

                pOutput->ModuleWriteFlags &= (~ModuleWriteModule); 
            }

            bRet = TRUE; 
        }
        break; 

    case ThreadCallback: 
        {
            // Include all thread information into the minidump 
            bRet = TRUE;  
        }
        break; 

    case ThreadExCallback: 
        {
            // Include this information 
            bRet = TRUE;  
        }
        break; 

    case MemoryCallback: 
        {
            // We do not include any information here -> return FALSE 
            bRet = FALSE; 
        }
        break; 
/*
    case CancelCallback: 
        break; 
*/
    }

    return bRet; 

}


#endif//UDPTRACKER_MINIDUMP_H
