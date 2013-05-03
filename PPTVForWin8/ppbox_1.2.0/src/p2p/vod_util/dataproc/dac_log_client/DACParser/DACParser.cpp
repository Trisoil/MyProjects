#include "Common.h"

#include "Base64.h"
#include "URLEncode.h"
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace Util;
namespace po = boost::program_options;
namespace fs = boost::filesystem;

std::string Decode(const std::string & url)
{
    const char* PPL_KEY = "pplive";
    const size_t PPL_KEY_LENGTH = strlen(PPL_KEY);

    std::string url2 = Util::Base64::Decode(url);
    std::string result;
    result.resize(url2.size());
    for (size_t i = 0; i < url2.size(); ++i)
    {
        size_t keyIndex = i % PPL_KEY_LENGTH;
        result[i] = url2[i] - PPL_KEY[keyIndex];
    }
    return result;
}

bool inline is_openservice(const string & str)
{
    if (str.size() > 12 && string(&str[0], &str[13]) == "Action=12&A=8")
        return true;
    else 
        return false;
}

bool inline is_live(const string & str)
{
    if (str.size() > 12 && string(&str[0], &str[13]) == "Action=12&A=0")
        return true;
    else 
        return false;
}

int main(int argc, char* argv[])
{
    po::options_description desc("Allowed Options");
    desc.add_options()
        ("help,h", "produce help message")
        ("output-file,o", po::value<string>(), "output file path")
        ("input-file,i", po::value<vector<string>>(), "input file path");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    std::string ip, user, password, database;
    std::vector<string> input_files;
    std::string output_file;

    if (vm.count("help"))
    {
        cout << desc << endl;
        return 0;
    }

    if (vm.count("input-file") == 0)
    {
        cout << "lost input-file" << endl
            << desc << endl;
        return 1;
    }
    else
        input_files = vm["input-file"].as<vector<string>>();

    if (vm.count("output-file") == 0 && vm.count("ip") == 0)
    {
        cout << "lost out-file and database ip" << endl
            << desc << endl;
        return 1;
    }
    else
    {
        if (vm.count("output-file"))
            output_file = vm["output-file"].as<string>();
    }
    
    // initialize output file stream
    std::ofstream fout(fs::path(output_file).file_string().c_str(), ios_base::app | ios_base::out);
    if (!fout)
    {
        cout << "create or open output file failed!" << endl;
    }

    for(size_t finx = 0; finx < input_files.size(); ++finx)
    {
        std::ifstream fin(fs::path(input_files[finx]).file_string().c_str());
        if (!fin)
        {
            cout << input_files[finx] << " is not exist, continue" <<std::endl;
            continue;
        }

        std::string key_time;              //��ǰ��������ݵ�ʱ�䣬��ȷ����
        std::vector<string> keys;      // ��ǰ��ȫ�������ݵĹؼ��֣���¼���ǵ�������Ϊ��ȥ��1s�ڵ��ظ�����
        std::string key;                     // һ�еĹؼ���
        std::vector<string> values;   // ������������
        std::string line;
        size_t line_num = 0;

        using namespace boost::gregorian; 
        using namespace boost::local_time;
        using namespace boost::posix_time;
        ptime time_t_epoch(date(1970,1,1)); 
        getline(fin, line);

        // deal with every line
        while (!fin.eof())
        {
            ++line_num;
            if (line_num % 1000 == 0)
            {
                cout << "line:" << line_num<< "\r";
            }

            std::string ip(&line[0], line.find_first_of(','));          // ��ȡ�ύ���ݿͻ��˵�IP
            line.erase(0, line.find_first_of(',')+2);                   // ɾ��IP����
            line.erase(0, line.find_first_of(',')+2);                   // ɾ��һ����������
            std::string date(&line[0], line.find_first_of(','));        // ��ȡ����
            line.erase(0, line.find_first_of(',')+2);                   // ɾ����������
            std::string time(&line[0], line.find_first_of(','));        // ��ȡʱ��
            line.erase(0, line.find_first_of(',')+2);                   // ɾ��ʱ������
            line.erase(line.find_last_of(','), line.size());            // ɾ��һ����������','
            line.erase(0, line.find_last_of(',')+2);                    // ɾ����־������ǰ��ȫ������
            string output(Decode(line));
            getline(fin, line);

            if (is_live(output))
            {
                fout << output << endl;
            }
        }
        
    }

    return 0;
}