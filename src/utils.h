#include "arg_parser.h"
#include "ubus_utils.h"

#ifndef _JSON_UTILS_
#define _JSON_UTILS_
    extern char* construct_JSON_data(struct MemData memory);
    extern char* get_full_path(const char* homeDir, const char* FILENAME);

    struct lua_script {
        const char* scriptName;
        const char* scriptPath;
    };

    struct lua_script* get_lua_scripts(const char* path);
    void execute_lua_script(struct lua_script script);
#endif