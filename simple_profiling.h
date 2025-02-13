#pragma once

/*
In this file: 
The simplest possible way to profile a single block/function. Run it once and gather block/function speed.
Since running a block once is VERY imprecise it's suggested to use functions/macros inside simple_benchmark,
but you'll still be able to get quick info using this module.
*/

//
// profiling of single blocks, usage: add PROFILE_FUNC or PROFILE_BLOCK("name") in the scope you want to profile,
// it will be executed once and immediately print you profiling informations (location, cycle count and execution time)
//

#ifndef SIMPLE_PROFILE
#define SIMPLE_PROFILE 1
#endif

#if SIMPLE_PROFILE

#ifndef GYOFIRST
    #include "first.h"
#endif
#ifndef GYOPERFORMANCE_COUNTER
    #include "performance_counter.h"
#endif

struct single_time_block {
    single_time_block(const char* func_name) {
        this->_label = func_name;
        this->_start = perf_cpu_timer();
    }
    
    ~single_time_block() {
        u64 total = perf_cpu_timer() - this->_start;
        u64 cpu_freq = perf_estimate_cpu_freq();
        f64 nanoseconds = total * 1000000000.0f / cpu_freq;
        if(nanoseconds < 1000) { print("function '%' took % cycles (%ns)", this->_label, total, nanoseconds); return; }
        nanoseconds /= 1000;
        if(nanoseconds < 1000) { print("function '%' took % cycles (%us)", this->_label, total, nanoseconds); return; }
        nanoseconds /= 1000;
        if(nanoseconds < 1000) { print("function '%' took % cycles (%ms)", this->_label, total, nanoseconds); return; }
        nanoseconds /= 1000;
        print("function '%' took % cycles (%s)", this->_label, total, nanoseconds);
    }
    
    const char* _label;
    u64 _start = 0;
};

#define PROFILE_FUNC single_time_block STRING_JOIN(t_, __LINE__)(__FUNCTION__)
#define PROFILE_BLOCK(name) single_time_block STRING_JOIN(t_, __LINE__)(name)

#else

#define PROFILE_FUNC
#define PROFILE_BLOCK(name)

#endif
