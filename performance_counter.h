//TODO(cogno): TEST THIS

/*
USAGE:
- add 'END_OF_COMPILATION_UNIT' as the last line of the file with your app entry point
- call begin_profile() when you want to start profiling
- call end_and_print_profile() when you want to get profiling information
- add TIME_FUNC or TIME_BLOCK("name") to every block you want to profile

if you want to deactivate profiling simply add '#define PROFILING_V1 0` anywhere you want before this file

This simple profiling can handle functions both nested and recursive.
It has a limit of 4086 timed blocks.
*/

//API(cogno): I think to properly handle different platforms it would be better to separate this into 2 files, the platform dependent part (up to estimate_cpu_frequency()) and the independent part (the profiling v1)

#ifndef PROFILING_V1
#define PROFILING_V1 1
#endif

#if _WIN32

#include <windows.h>

static u64 get_os_timer_freq() {
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return freq.QuadPart;
}

static u64 read_os_timer() {
	LARGE_INTEGER value;
	QueryPerformanceCounter(&value);
	return value.QuadPart;
}

inline u64 read_cpu_timer() {
	return __rdtsc();
}

#else

#include <x86intrin.h>
#include <sys/time.h>

static u64 get_os_timer_freq() {
	return 1000000;
}

static u64 read_os_timer() {
	timeval value;
	gettimeofday(&value, 0);
	
	u64 result = get_os_timer_freq()*(u64)value.tv_sec + (u64)value.tv_usec;
	return result;
}

inline u64 read_cpu_timer() {
    return 0; //TODO(cogno): rdtsc on arm? I don't know which one to use
}
#endif


u64 estimate_cpu_frequency(int ms_to_wait = 1) {
    // calculate how many os clocks to wait
    u64 os_timer_freq = get_os_timer_freq();
    u64 os_timer_to_wait = os_timer_freq * ms_to_wait / 1000;
    
    // wait for those clocks, saving cpu timer start and end
    u64 os_clocks_elapsed = 0;
    u64 os_counter_end = 0;
    u64 os_counter_start = read_os_timer();
    u64 cpu_counter_start = read_cpu_timer();
    while(os_clocks_elapsed < os_timer_to_wait) {
        os_counter_end = read_os_timer();
        os_clocks_elapsed = os_counter_end - os_counter_start;
    }
    u64 cpu_counter_end = read_cpu_timer();
    
    // estimate cpu freq from os timer, formula comes from this:
    // cpu timer / cpu freq = os timer / os freq
    // cpu freq = cpu timer * os freq / os timer
    u64 freq_estimate = (cpu_counter_end - cpu_counter_start) * os_timer_freq / os_clocks_elapsed;
    return freq_estimate;
}



struct single_time_block {
    single_time_block(const char* func_name) {
        this->_label = func_name;
        this->_start = read_cpu_timer();
    }
    
    ~single_time_block() {
        u64 total = read_cpu_timer() - this->_start;
        u64 cpu_freq = estimate_cpu_frequency();
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



#if PROFILING_V1

//API(cogno): we can very easily save the index of the parent to have better ui, but it costs more, is it worth it?
struct TimeAnchor {
    u64 elapsed_exclusive;
    u64 elapsed_inclusive;
    u64 hit_count;
    const char* label;
};

#define ANCHORS_AMT 4096
TimeAnchor anchors[ANCHORS_AMT] = {};
int current_parent_index = 0;
u64 profile_start = 0;
u64 profile_end = 0;


struct time_block {
    time_block(int counter, const char* func_name) {
        this->_parent = current_parent_index;
        this->_current = counter;
        this->_label = func_name;
        this->_old_elapsed_at_root = anchors[counter].elapsed_inclusive;
        
        current_parent_index = counter;
        this->_start = read_cpu_timer();
    }
    
    ~time_block() {
        u64 total = read_cpu_timer() - this->_start;
        current_parent_index = this->_parent;
        
        anchors[this->_parent ].elapsed_exclusive -= total;
        anchors[this->_current].elapsed_exclusive += total;
        anchors[this->_current].elapsed_inclusive = this->_old_elapsed_at_root + total;
        anchors[this->_current].hit_count++;
        
        anchors[this->_current].label = this->_label; // yes I know, it gets replaced multiple times, I hate C++
    }
    
    const char* _label;
    u64 _start = 0;
    int _current = 0; //save them so we can keep them (inner blocks edit global ones)
    int _parent = 0;  //save them so we can keep them (inner blocks edit global ones)
    int _old_elapsed_at_root = 0;
};

#define TIME_FUNC time_block STRING_JOIN(t_, __LINE__)(__COUNTER__ + 1, __FUNCTION__)
#define TIME_BLOCK(name) time_block STRING_JOIN(t_, __LINE__)(__COUNTER__ + 1, name)
#define END_OF_COMPILATION_UNIT static_assert(__COUNTER__ < ANCHORS_AMT, "Number of profile points exceeds size of profiler::Anchors array")

void begin_profile() {
    profile_start = read_cpu_timer();
}

void pad_right(int to_add) {
    for(int i = 0; i < to_add; i++) putchar(' ');
}

void end_and_print_profile() {
    profile_end = read_cpu_timer();
    u64 total_elapsed = profile_end - profile_start;
    
    //calculate lengths of texts for vertical formatting
    int max_label_len = 0;
    int max_time_length = 0;
    int max_incl_time_length = 0;
    bool one_has_childrens = false;
    for(int i = 0; i < 4086; i++) {
        TimeAnchor t = anchors[i];
        if(t.label == nullptr) continue;
        
        int label_len = 0;
        while(t.label[label_len++]) {}
        if(label_len > max_label_len) max_label_len = label_len;
        
        int digits_count = log10(t.elapsed_exclusive) + 1;
        if (digits_count > max_time_length) max_time_length = digits_count;
        
        int incl_digits_count = log10(t.elapsed_inclusive) + 1;
        if (incl_digits_count > max_incl_time_length) max_incl_time_length = incl_digits_count;
        
        if(t.elapsed_inclusive != t.elapsed_exclusive) one_has_childrens = true;
    }
    
    // PERF(cogno): can be optimized for printing speed, no need for now since it's a report tool which prints once at the end of the whole program
    for(int i = 0; i < 4086; i++) {
        TimeAnchor t = anchors[i];
        if (t.label == nullptr) continue;
        
        float percentage_exclusive     = 100.0f * t.elapsed_exclusive / total_elapsed;
        float percentage_with_children = 100.0f * t.elapsed_inclusive / total_elapsed;
        
        printf("%d=> ", i);
        if(t.elapsed_inclusive != t.elapsed_exclusive) {
            //there was an inner block, report complete info
            printf("w/ childrens | ");
        } else {
            //no inner block, less info required
            printf("basic ");
            if(one_has_childrens) printf("       ");
            printf("| ");
        }
        
        //align all label names vertically
        printf("%s", t.label);
        int label_len = 0;
        while(t.label[label_len++]) {}
        pad_right(max_label_len - label_len);
        putchar(':');
        putchar(' ');
        
        //total time
        printf("total=%lld ", t.elapsed_exclusive);
        int digits_count = log10(t.elapsed_exclusive) + 1;
        pad_right(max_time_length - digits_count);
        
        //total time percentage
        printf("(%5.2f%%), ", percentage_exclusive);
        
        //children time
        if(t.elapsed_inclusive != t.elapsed_exclusive) {
            printf("incl=%lld ", t.elapsed_inclusive);
            int incl_digits_count = log10(t.elapsed_inclusive) + 1;
            pad_right(max_incl_time_length - incl_digits_count);
            printf("(%5.2f%% w/ childs), ", percentage_with_children);
        } else if(one_has_childrens) {
            pad_right(5 + max_incl_time_length + 21);
        }
        
        //hit count
        if(t.hit_count == 1) printf("hit once");
        else {
            printf("hit %d time", t.hit_count);
            if(t.hit_count > 1) putchar('s');
        }
        putchar('\n');
    }
    
    //TODO(cogno): print total cycles/seconds from start to end?
}

#else

#define TIME_FUNC
#define TIME_BLOCK(name)
#define END_OF_COMPILATION_UNIT


void begin_profile() {}
void end_and_print_profile() {}


#endif