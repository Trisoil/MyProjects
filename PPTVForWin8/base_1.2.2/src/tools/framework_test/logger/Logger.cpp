// Logger.cpp

#include "tools/framework_test/Common.h"

#include <framework/configure/Config.h>
#include <framework/logger/Logger.h>
#include <framework/logger/ListRecord.h>
#include <framework/logger/FormatRecord.h>
#include <framework/logger/StreamRecord.h>
#include <framework/logger/DataRecord.h>
#include <framework/logger/StringRecord.h>
using namespace framework::configure;
using namespace framework::logger;

std::vector<int> func()
{
    return std::vector<int>(2, 1);
}

FRAMEWORK_LOGGER_DECLARE_MODULE("TestLogger");

static void test_logger(Config & conf)
{
    int i = 1;
    float f = 2.0;
    std::string s = "str";
    char m[] = "str";

    //glog.log_r(logger_param_record(0, 0, "TestLog", LoggerParamsBeg() << LOG_NVP(i) << LOG_NVP(f) << LoggerParamsEnd()));
    LOG_L(0, (i)(f)(s)(m));

    LOG_F(0, "(i=%i,f=%f,s=%s)" % i % f % s.c_str());

    LOG_F(0, "(i=%1%,f=%2%,s=%3%)" % i % f % s.c_str());

    LOG_S(0, "(i=" << i << ",f=" << f << ",s=" << s << "m" << m << ")");

    LOG_DATA(0, ("llll", (unsigned char const *)"dsfgasdasdwefgsadfgasdtqwfsagwegawegasdgwegaeggeawgwagwgweg", 35));

    LOG_STR(0, ("llll", "dsfgasdasdwefgsadfgasdtqwfsagwegawegasdgwegaeggeawgwagwgweg", 21));
}

static TestRegister test("logger", test_logger);
