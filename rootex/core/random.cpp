#include "random.h"

std::random_device Random::s_RandomDevice;
std::uniform_real_distribution<double> Random::s_Distribution(0.0f, 1.0f);
