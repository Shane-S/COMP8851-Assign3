#pragma once
#include <cstdint>

std::uint64_t MurmurHash64(const void * key, int len, unsigned int seed);