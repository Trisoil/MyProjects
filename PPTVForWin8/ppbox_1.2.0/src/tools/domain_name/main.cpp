// main.cpp

#include <ppbox/common/Common.h>
#include <ppbox/common/DomainName.h>

#include <iostream>

int main(int argc, char * argv[])
{
    if (argc != 2 && argc != 3 && argc != 4) {
        std::cout << "usage: domain_name file [domain] [value]" << std::endl;
        return 1;
    }

    std::string file = argv[1];
    boost::system::error_code ec;

    if (argc == 2) {
	std::vector<std::pair<std::string, std::string> > domain_values;
        ec = ppbox::common::domain_name_get_all(file, domain_values);
        if (!ec) {
            for (size_t i = 0; i < domain_values.size(); ++i) {
                std::cout << domain_values[i].first << ": " << domain_values[i].second << std::endl;
            }
        }
    } else if (argc == 3) {
    	std::string domain = argv[2];
        std::string value;
        ec = ppbox::common::domain_name_get(file, domain, value);
        if (!ec) {
            std::cout << value << std::endl;
        }
    } else {
    	std::string domain = argv[2];
        std::string value = argv[3];
        ec = ppbox::common::domain_name_set(file, domain, value);
    }
    if (ec) {
        std::cerr << file << ": " << ec.message() << std::endl;
    }
    return ec.value();
}
