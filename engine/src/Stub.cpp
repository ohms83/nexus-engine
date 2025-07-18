// A temporary file using purely for integration testing
#define XXH_STATIC_LINKING_ONLY
#define XXH_IMPLEMENTATION
#include <xxhash.h>

// This file exists only to force CMake to generate the header
// It doesn't need to do anything else.
#include <nexus/Nexus.h>