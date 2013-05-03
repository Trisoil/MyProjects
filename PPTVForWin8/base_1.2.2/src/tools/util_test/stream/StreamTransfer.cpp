// StreamTransfer.cpp

#include "tools/util_test/Common.h"
#include "util/stream/StreamTransfer.h"
using namespace util::stream;

using namespace framework::configure;

#include <boost/asio/buffer.hpp>
#include <boost/asio/ip/tcp.hpp>
using namespace boost::asio;

static char buf[1024];

void nop_handler(
    boost::system::error_code const & ec, 
    transfer_size const &)
{
}

void test_stream_transfer(Config & conf)
{
    mutable_buffers_1 buffers1(buf, sizeof(buf));

    TransferBuffers buffers2(1024);

    util::buffers::CycleBuffers<mutable_buffers_1> buffers3(buffers1);

    io_service io_svc;
    ip::tcp::socket s1(io_svc), s2(io_svc);

    transfer(s1, s2, buffers3);
    async_transfer(s1, s2, buffers1, nop_handler);
}

static TestRegister test("stream_transfer", test_stream_transfer);
