#pragma once

#include <sntp.h>


namespace system_config
{


constexpr const char* ntpServer1 = "pool.ntp.org";
constexpr const char* ntpServer2 = "time.nist.gov";

const long gmtOffset_sec = 8 * 3600; // GMT+8
const int daylightOffset_sec = 0;


} // namespace system