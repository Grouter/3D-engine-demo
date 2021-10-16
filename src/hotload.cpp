internal void hotload_watcher() {
    log_print("Hotload started!\n");

    HANDLE watch_handle = CreateFile(
        "./",   // the asset folder
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
        nullptr,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        nullptr
    );

    u8 *buffer = (u8 *)calloc(2048, 1);
    char file_name[2048];

    DWORD b_returned;

    while (1) {
        bool result = ReadDirectoryChangesW(
            watch_handle,
            buffer,
            2048,
            true,
            FILE_NOTIFY_CHANGE_LAST_WRITE,
            &b_returned,
            nullptr,
            nullptr
        );

        if (result == 0) {
            log_print("Read dir changes failed err code: %d\n", GetLastError());
            break;
        }

        FILE_NOTIFY_INFORMATION *info = (FILE_NOTIFY_INFORMATION *)buffer;

        if (info->Action != FILE_ACTION_MODIFIED) continue;

        u64 file_name_size = info->FileNameLength / sizeof(wchar_t);

        // Reset the filename buffer and convert wide chars to normal chars
        // while writing the the filename buffer
        memset(file_name, 0, 2048);
        for (u64 i = 0; i < file_name_size; i++) {
            file_name[i] = (char)info->FileName[i];
        }

        log_print("Hotload trigger: %s\n", file_name);

        // File extension
        // char *extension = file_name;
        // eat_until(&extension, '.');
        // extension += 1;

        // if (strncmp(extension, "glsl", 4) == 0) {

        // }
        // else {

        // }
    }

    free(buffer);
}