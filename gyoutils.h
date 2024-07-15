#pragma once
/*
Configuration macros
- NO_ASSERT
    used to disable all types of assert, improves performance but disables (some) safety checks
- DISABLE_INCLUDES
    used to disable all the internal includes for a custom implementation
- PROFILING_V1
    if you want to deactivate profiling v1, simply add '#define PROFILING_V1 0`
- SIMPLE_PROFILE
    if you want to deactivate simple profiling, simply add '#define SIMPLE_PROFILE 0`
- SIMPLE_BENCHMARK
    if you want to deactivate benchmarking, simply add '#define SIMPLE_BENCHMARK 0`
*/

#include "first.h"
#include "math.h"
#include "allocators.h"

#include "array.h"
#include "str.h"
#include "hashmap.h"

#include "performance_counter.h"
#include "simple_profiling.h"
#include "profiling_v1.h"
#include "simple_benchmark.h"

#if _WIN32
    #include "win64_basic.h"
    #include "win64_files.h"
#endif
