#ifndef    _PPBOX_COMPRESS_LV_H_
#define    _PPBOX_COMPRESS_LV_H_

#include <boost/filesystem/path.hpp>
#include <boost/system/error_code.hpp>

namespace ppbox
{
    namespace compress
    {
        struct Buffer;

        class LempelZiv 
        {
        public:
            LempelZiv(boost::filesystem::path const & in, 
               boost::filesystem::path const & out);
            ~LempelZiv();
            boost::system::error_code compress(boost::system::error_code & ec);
            boost::system::error_code uncompress(boost::system::error_code & ec);

        private:
            boost::system::error_code file_to_buffer(
                std::string const & filename,
                Buffer & buf,
                boost::system::error_code & ec);

            boost::system::error_code buffer_to_file(
                std::string const & filename,
                Buffer const & buf,
                boost::system::error_code & ec);

            boost::uint32_t file_size(std::ifstream & is);

        private:
            boost::filesystem::path in_file_path_;
            boost::filesystem::path out_file_path_;
        };
    }
}
#endif // _PPBOX_COMPRESS_LV_H_
