/*
In this file: the basics most useful functions to work with the windows api, in a convenient way
- win64_print_error(): a fast way to print the last windows error
- win64_alloc(...): allocate memory
- win64_free(...): free memory
*/

#ifndef DISABLE_INCLUDES
    #include <windows.h>
#endif

void win64_print_error() {
    char buf[255]; // TODO(cogno): is this enough?
    DWORD dwError = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 0, dwError, 0, buf, sizeof(buf), 0);
    print((const char*)buf);
}

void* win64_alloc(u64 size) {
    return VirtualAlloc(NULL, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}

void win64_free(void* to_free) {
    if(to_free) VirtualFree(to_free, 0, MEM_RELEASE);
}

// TODO(cogno): add crash handler