
#ifndef LOGGING_H
#define LOGGING_H


#define LOG_DEBUG 0
#define LOG_INFO 1
#define LOG_WARNING 2
#define LOG_ERROR 3
#define NO_LOGGING 4

#define DEBUG_LEVEL LOG_DEBUG

#if DEBUG_LEVEL < NO_LOGGING
#include "os_api.h"
#include "logger/serial.h"
#endif

#endif // LOGGING_H
