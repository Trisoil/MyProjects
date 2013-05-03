#include "pp_enclib.h"
#include "iostream"

int main()
{
    
    while(1)
    {
        std::string original_string;
        std::getline(cin, original_string);
        if (cin.eof())
        {
            break;
        }

        /***
        解压后的加密日志格式
        124.78.75.71, -, 25/Nov/2011:04:00:02 +0800, -, -, -, -, -, -, 200, -, GET, /1.html, sdPqys7ZopqPpq2pnKOcm92amZbW1KbFksyVnMubqZa5npKfp6KulqWxraSlnqagm5yho7mipLGdrq6opaasppKsi66ml56ipJGNnZWgj6qt7exJ7u6FuVISFpicp5zT2d7Zn5%2Bfp5mPmZafnJOhoKyPkJ6dkLCilrizkYW0opyalqGhqpWWka%2BnmpahpqeXhbainpmepqCmh6uolqOZm6SinK6cm4u4ppWWv7OWkJ6Wn4%2B1raKompijmJC6oqeWyJ6VkbinmYvErayUk5G6p5mLxq2ukZejnJDAoqKir5qXnovCppapoKyVkZG%2Bp4%2B/raKskYWslqebnKOknKOQqJaeop2hp5ykkKiWkK2WraWokpGcm5Culq2h,
        ***/

        
        int i = original_string.find_first_of(",");
        int j = original_string.find_first_of("+");
        if (i == std::string::npos || j == std::string::npos)
        {
            continue;
        }

        std::string end_point = original_string.substr(0, i); 
        std::string log_time = original_string.substr(i + 5, j - (i + 6));
        
        
        int k = original_string.rfind(",", original_string.size() - 3);
        if (k == std::string::npos)
        {
            continue;
        }
        std::string temp_string = original_string.substr(k + 2, original_string.size() - k - 3);
        std::cout << Decode(temp_string) << "&EndPoint=" << end_point << "&LogTime=" << log_time << std::endl;
    }
    
    return 0;
}