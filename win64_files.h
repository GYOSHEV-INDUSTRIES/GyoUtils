bool get_only_files_in_dir(const char* folder_path, Array<str>* filenames) {
    StringBuilder builder = make_string_builder();
    defer(free(builder.data));
    string_builder_append(&builder, folder_path);
    string_builder_append(&builder, "\\*");
    string_builder_append_raw(&builder, (u8)0);
    
    WIN32_FIND_DATA win_file_data = {};
    HANDLE handle = FindFirstFile((char*)builder.data, &win_file_data);
    defer(FindClose(handle));
    if(handle == INVALID_HANDLE_VALUE) {
        print("no initial file found");
        return false;
    }
    
    string_builder_remove_last_bytes(&builder, 2);
    StringBuilder copy = string_builder_copy(&builder);
    defer(free(copy.data));
    
    do {
        // reset str builder into the initial folder path
        copy.size = builder.size;
        string_builder_append(&copy, win_file_data.cFileName);
        string_builder_append_raw(&copy, (u8)0);
        
        // get info if it's a file or not
        auto result = GetFileAttributes((const char*)copy.data);
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