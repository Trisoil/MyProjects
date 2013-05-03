//------------------------------------------------------------------------------------------
//     Copyright (c)2005-2010 PPLive Corporation.  All rights reserved.
//------------------------------------------------------------------------------------------

#include "Common.h"
#include "App.h"
#include <exception>

void unhandled_exception_handler() 
{ 
   std::cout << "Got an unhandled exception!" << std::endl; 
} 

int main()
{
    std::set_terminate(unhandled_exception_handler);

    live_media_test::App application;
    int app_result = application.Run();
    return app_result;
}
