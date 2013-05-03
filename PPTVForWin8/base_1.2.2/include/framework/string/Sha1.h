// Sha1.h

#ifndef _FRAMEWORK_STRING_SHA1_H_
#define _FRAMEWORK_STRING_SHA1_H_

#include <boost/array.hpp>

namespace framework
{
    namespace string
    {

        namespace detail
        {
            struct Sha1Ctx;
        }

        class Sha1
        {
        public:
            static size_t const block_size = 64; // in bytes

            static size_t const output_size = 20; // in bytes

            typedef boost::array<boost::uint8_t, output_size> bytes_type;

        public:
            Sha1();

            ~Sha1();

        public:
            void init();

            void update(
                boost::uint8_t const * buf, 
                size_t len);

            void final();

            boost::uint8_t * digest() const;

        public:
            static bytes_type apply(
                boost::uint8_t const * buf, 
                size_t len);

        public:
            bytes_type to_bytes() const;

            void from_bytes(
                bytes_type const & str);

        public:
            std::string to_string() const;

            boost::system::error_code from_string(
                std::string const & str);

        private:
            detail::Sha1Ctx * ctx_;
        };

        inline Sha1::bytes_type sha1(
            boost::uint8_t const * buf, 
            size_t len)
        {
            return Sha1::apply(buf, len);
        }

        inline Sha1::bytes_type sha1(
            std::string const & data)
        {
            return Sha1::apply((boost::uint8_t const *)data.c_str(), data.size());
        }

        inline Sha1::bytes_type sha1(
            std::vector<boost::uint8_t> const & data)
        {
            return Sha1::apply(data.empty() ? NULL : &data.at(0), data.size());
        }

        template <
            size_t N
        >
        inline Sha1::bytes_type sha1(
            boost::uint8_t const (& data)[N])
        {
            return Sha1::apply(data, N);
        }

        template <
            size_t N
        >
        inline Sha1::bytes_type sha1(
            boost::array<boost::uint8_t, N> const & data)
        {
            return Sha1::apply(data.data(), data.size());
        }

    } // namespace string
} // namespace framework

#endif // _FRAMEWORK_STRING_SHA1_H_
