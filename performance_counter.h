#pragma once
#define GYOPERFORMANCE_COUNTER

/*
In this file:
Mostly performance counter related functions.
- perf_os_timer_freq() to know the frequency of OS timers
- perf_os_timer() to get the current os timer clock cycle
- perf_estimate_cpu_freq() to estimate the cpu rdtsc timer frequency.
- perf_cpu_timer() to get the current cpu timer clock cycle (rdtsc is faster than os timers)
*/

#if _WIN32

#ifndef DISABLE_INCLUDES
	#include <windows.h>
    #ifdef __GNUC__
    #include <x86intrin.h>
    #include <sys/time.h>
    #endif
#endif

static u64 perf_os_timer_freq() {
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return freq.QuadPart;
}

static u64 perf_os_timer() {
	LARGE_INTEGER value;
	QueryPerformanceCounter(&value);
	return value.QuadPart;
}

inline u64 perf_cpu_timer() {
	return __rdtsc();
}


#else

#ifndef DISABLE_INCLUDES
    #include <sys/time.h>
    #if defined(__x86_64__) || defined(__i386__)
    #include <x86intrin.h>
    #endif
#endif

static u64 perf_os_timer_freq() {
	return 1000000;
}

static u64 perf_os_timer() {
	timeval value;
	gettimeofday(&value, 0);
	
	u64 result = perf_os_timer_freq()*(u64)value.tv_sec + (u64)value.tv_usec;
	return result;
}

// Fast monotonic counter for profiling. On x86 this is the TSC (rdtsc).
// On ARM64 there is no userspace-equivalent of the core cycle counter;
// CNTVCT_EL0 is the architectural virtual timer (fixed frequency, typically
// ~24 MHz on Apple Silicon). Frequency is still recovered via
// perf_estimate_cpu_freq(), so seconds come out correct either way.
inline u64 perf_cpu_timer() {
#if defined(__aarch64__) || defined(__arm64__)
    u64 value;
    __asm__ volatile("mrs %0, cntvct_el0" : "=r"(value));
    return value;
#elif defined(__x86_64__) || defined(__i386__)
    return __rdtsc();
#else
    // Last-resort platforms: fall back to the OS timer.
    return perf_os_timer();
#endif
}
#endif

u64 perf_estimate_cpu_freq(int ms_to_wait = 1) {
    // calculate how many os clocks to wait
    u64 os_timer_freq = perf_os_timer_freq();
    u64 os_timer_to_wait = os_timer_freq * ms_to_wait / 1000;
    
    // wait for those clocks, saving cpu timer start and end
    u64 os_clocks_elapsed = 0;
    u64 os_counter_end = 0;
    u64 os_counter_start = perf_os_timer();
    u64 cpu_counter_start = perf_cpu_timer();
    while(os_clocks_elapsed < os_timer_to_wait) {
        os_counter_end = perf_os_timer();
        os_clocks_elapsed = os_counter_end - os_counter_start;
    }
    u64 cpu_counter_end = perf_cpu_timer();
    
    // estimate cpu freq from os timer, formula comes from this:
    // cpu timer / cpu freq = os timer / os freq
    // cpu freq = cpu timer * os freq / os timer
    u64 freq_estimate = (cpu_counter_end - cpu_counter_start) * os_timer_freq / os_clocks_elapsed;
    return freq_estimate;
}
