#pragma once

#include "vendor/OpenAL/include/al.h"
#include "vendor/OpenAL/include/alc.h"
#include "vendor/OpenAL/include/alut.h"

#ifndef ALUT_CHECK
#ifdef _DEBUG
#define ALUT_CHECK(alutFunction)                                        \
	do                                                                  \
	{                                                                   \
		alutFunction;                                                   \
		AudioSystem::CheckALUTError(#alutFunction, __FILE__, __LINE__); \
	} while (0)
#else
#define ALUT_CHECK(alutFunction) alutFunction
#endif
#endif

#ifndef AL_CHECK
#ifdef _DEBUG
#define AL_CHECK(alFunction)                                        \
	do                                                              \
	{                                                               \
		alFunction;                                                 \
		AudioSystem::CheckALError(#alFunction, __FILE__, __LINE__); \
	} while (0)
#else
#define AL_CHECK(alFunction) alFunction
#endif
#endif
