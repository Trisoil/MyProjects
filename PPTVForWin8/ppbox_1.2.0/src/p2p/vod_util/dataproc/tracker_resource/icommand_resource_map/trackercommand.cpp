// test_tracker.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Tester.h"
#include<iostream>
#include<string>
#include<fstream>

int main(int argc, char* argv[])
{
    if( argc != 2 ) {
        printf("argument error, please include the tracker input files\n");
        //return -1;
    }

	//std::ifstream finput(argv[1]);
	std::ifstream finput("tracker_list.txt");
	if(finput.fail())
	{
		std::cout <<"The file could not be opened.\n";
		return -1;
	}
	
	std::vector <string> ips;
	std::vector <int> ports;
	std::string str;
	int i = 0, pos, port;
	std::string ipstr;
	while(getline(finput,str))
	{
		//now process the string.
		boost::algorithm::trim(str);
		if (str.empty()) break;
		if (str[0] == '#' ) continue;
		pos = str.find(':');
		ipstr = str.substr(0,pos);
		port = atoi(str.substr(pos+1).c_str());
		printf("reading tracker %s  %d\n", ipstr.c_str(), port);
		ips.push_back(ipstr);
		ports.push_back(port);
		i++;
	}


    Tester test( &ips, &ports );
    test.start();

	fgetc(stdin);

    return 0;
}

