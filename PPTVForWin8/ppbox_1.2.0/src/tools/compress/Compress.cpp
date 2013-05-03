#include "tools/compress/Common.h"
#include "tools/compress/LempelZiv.h"

#include <framework/memory/MemoryPage.h>
#include <boost/cstdint.hpp>
#include <iostream>

void usage(void)
{
    std::cout << "usage: e(compress)|d(decompress) input_file out_file";
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        usage();
        return 0;
    }

    std::string option      = argv[1];
    std::string input_file  = argv[2];
    std::string output_file = argv[3];
    ppbox::compress::LempelZiv lz_compress(input_file, output_file);

    boost::system::error_code ec;
    if (option == "e") {
        lz_compress.compress(ec);
    } else {
        lz_compress.uncompress(ec);
    }

    std::cout << "ec = " << ec.message() << std::endl;
    return 0;
}
