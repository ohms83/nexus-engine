#include "nexus/math/Random.h"
#include <chrono>

using namespace nxs;

Random::Random()
{
    m_rng = std::mt19937(std::chrono::high_resolution_clock::now().time_since_epoch().count());
}