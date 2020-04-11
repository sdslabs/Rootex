#pragma once

#include <random>

/// A random number generator.
class Random
{
	static std::random_device s_RandomDevice;
	static std::uniform_real_distribution<double> s_Distribution;

public:
	/// Returns a random float between 0.0f and 1.0f.
	static float Float() { return s_Distribution(s_RandomDevice); }
};
