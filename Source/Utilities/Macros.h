#pragma once

#include <varargs.h>
#include "LoggingModule/Logger.h"

#define LOG(FormatString, Intensity, ...) Logger::DebugPrint(__FILE__, __LINE__,  Intensity, (FormatString), ##_VA_ARGS__)
