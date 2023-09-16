#pragma once

#include <windows.h>

bool get_only_files_in_dir(const char* folder_path, Array<str>* filenames) {
    StrBuilder builder = make_str_builder();
    defer(free(builder.ptr));
    str_builder_append(&builder, folder_path);
    str_builder_append(&builder, "\\*");
    str_builder_append_raw(&builder, (u8)0);
    
    WIN32_FIND_DATA win_file_data = {};
    HANDLE handle = FindFirstFile((char*)builder.ptr, &win_file_data);
    defer(FindClose(handle));
    if(handle == INVALID_HANDLE_VALUE) {
        print("no initial file found");
        return false;
    }
    
    str_builder_remove_last_bytes(&builder, 2);
    StrBuilder copy = str_builder_copy(&builder);
    defer(free(copy.ptr));
    
    do {
        // reset str builder into the initial folder path
        copy.size = builder.size;
        str_builder_append(&copy, win_file_data.cFileName);
        str_builder_append_raw(&copy, (u8)0);
        
        // get info if it's a file or not
        auto result = GetFileAttributes((const char*)copy.ptr);
        if((int)result < 0) {
            print("ERROR: invalid file attributes");
            return false;
        }
        
        if(result & FILE_ATTRIBUTE_ARCHIVE) {
            str file_name = str_new_alloc(win_file_data.cFileName); // else it doesn't live outside this function
            array_append(filenames, file_name);
        }
        
    } while(FindNextFile(handle, &win_file_data));
    
    auto err = GetLastError();
    if(err != ERROR_NO_MORE_FILES) {
        print("error: %", (int)err);
        return false;
    }
    
    return true;
}
