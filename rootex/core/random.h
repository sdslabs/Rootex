#pragma once

#include <random>

class Random
{
	static std::random_device s_RandomDevice;
	static std::uniform_real_distribution<double> s_Distribution;

public:
	static float Float() { return s_Distribution(s_RandomDevice); }
};
