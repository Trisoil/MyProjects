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

    HANDLE hFile = CreateFile( _T("MiniDump.dmp"), GENERIC_READ | GENERIC_WRITE, 
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
            _tprintf( _T("MiniDumpWriteDump failed. Error: %u \n"), GetLastError() ); 
        else 
            _tprintf( _T("Minidump created.\n") ); 

        // Close the file 

        CloseHandle( hFile ); 

    }
    else 
    {
        _tprintf( _T("CreateFile failed. Error: %u \n"), GetLastError() ); 
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
