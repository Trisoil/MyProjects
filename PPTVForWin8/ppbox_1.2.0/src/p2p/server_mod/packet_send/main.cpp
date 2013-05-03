#include <iostream>

#include "packet_sender.h"

void generate_file_list( std::vector<std::string> & file_list , int argc, const char * argv[]) 
{
    // 
    for( int i = 1; i < argc ; i++ ) {
        // 
        std::string filename = argv[i];

        // 
        if( filename.find( "*" ) != filename.npos ) {
            // ÓĞÍ¨Åä·û
        } else {
            file_list.push_back( filename );
        }
    }
}

void print_usage()
{
    std::cout<< "usage:"
        << "\tpacket_send filename.xml" << std::endl
        << "\tpacket_send filename1.xml filename2.xml" << std::endl
        << "\tpacket_send packet/*.xml" << std::endl
        ;
}

int main (int argc, const char * argv[]) 
{
    if( argc < 2 ) {
        print_usage();
        return -1;
    }

    std::vector<std::string> file_list;

    generate_file_list( file_list , argc , argv );

    packet_sender sender( file_list);
    sender.run();

    return 0;
}

