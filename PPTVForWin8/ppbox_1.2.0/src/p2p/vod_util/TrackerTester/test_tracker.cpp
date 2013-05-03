// test_tracker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Tester.h"

int main(int argc, char* argv[])
{
    if( argc != 3 ) {
        printf("argument error\n");
        return -1;
    }

    int port;
    sscanf( argv[2] , "%d" , &port );
    Tester test( argv[1] , port );
    test.start();

    if( test.test_result_ == 1 )
	    return 0;
    else
        return -1;
}

