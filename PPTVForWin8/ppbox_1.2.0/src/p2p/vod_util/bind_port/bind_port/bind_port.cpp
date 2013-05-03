#include <WinSock2.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

int Bind(const string& protocol,unsigned short port)
{
	SOCKET* connection = new SOCKET;
	SOCKADDR_IN Addr;
	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(port);
	//Addr.sin_addr.s_addr = htonl(INADDR_ANY);inet_addr
	Addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (strncmp(protocol.c_str(), "tcp", 3) == 0)
	{
		*connection = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	} 
	else if (strncmp(protocol.c_str(), "udp", 3) == 0)
	{
		*connection = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);		
	}

	cout<<bind(*connection, (SOCKADDR*)&Addr, sizeof(Addr));
	if (strncmp(protocol.c_str(), "tcp", 3) == 0)
	{
		listen(*connection,5);
	}
	return 1;

}

int main(int argc, char* argv[])
{
	WSADATA wsaData;
	SOCKET connection;
	SOCKADDR_IN Addr;


	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//assert(argc >= 3);
	if (argc < 2)
	{
		cout<<"usage:(bind_port.exe tcp port) or (bind_port.exe filename)"<<endl;
		return 1;
	}
	int port;
	const char *p = argv[1];
	//const char *path = "D:\\MyCode\\bind_port\\Debug\\bind.txt";

	if (strncmp(p, "tcp", 3) == 0)
	{
		Bind(argv[1],atoi(argv[2]));
	} 
	else if (strncmp(p, "udp", 3) == 0)
	{
		Bind(argv[1], atoi(argv[2]));
	}
	else
	{
		ifstream file(p);
		//file.open(argv[1]);
		//cout<<file.is_open()<<endl;
		vector<string> bind_line;
		string tmp_line;
		string protocol;
		unsigned short port;
		while(getline(file, tmp_line))
		{
			stringstream ss(tmp_line);
			ss>>protocol;
			ss>>port;
			Bind(protocol, port);
		}

	}
	cin>>port;

	return 1;
}