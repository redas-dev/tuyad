#define _DEFAULT_SOURCE
#include <cJSON.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <dirent.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "utils.h"

extern char* construct_JSON_data(struct MemData memory)
{
    cJSON* const temp = cJSON_CreateObject();
    cJSON_AddNumberToObject(temp, "Total_Memory", memory.total);
    cJSON_AddNumberToObject(temp, "Free_Memory", memory.free);
    cJSON_AddNumberToObject(temp, "Shared_Memory", memory.shared);
    cJSON_AddNumberToObject(temp, "Buffered_Memory", memory.buffered);

    if (temp == NULL){
        syslog(LOG_ERR, "%s", "Failed to create JSON object.");
        return NULL;
    }
    syslog(LOG_INFO, "%s", "JSON object created successfully");

    char* report = cJSON_PrintUnformatted(temp);

    cJSON_DeleteItemFromObject(temp, "Total_Memory");
    cJSON_DeleteItemFromObject(temp, "Free_Memory");
    cJSON_DeleteItemFromObject(temp, "Shared_Memory");
    cJSON_DeleteItemFromObject(temp, "Buffered_Memory");
    cJSON_Delete(temp);

    return report;
}

char* get_full_path(const char* homeDir, const char* FILENAME)
{
    int fullLen = strlen(homeDir) + strlen(FILENAME) + 2;   // +2 to account for '/' symbol and '\0'.

    char* fullPath = (char*)malloc(fullLen * sizeof(char));

    if (fullPath == NULL) return NULL;

    sprintf(fullPath, "%s/%s", homeDir, FILENAME);

    return fullPath;
}

static const char* defaultPath = "/usr/lib/tuya/modules";

struct lua_script* get_lua_scripts(const char* path)
{
    struct lua_script* scripts = NULL;
    
    if (path == NULL) path = defaultPath;

    DIR* dir = opendir(path);

    if (dir == NULL) return NULL;

    struct dirent* entry = NULL;
    int count = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".lua") != NULL)
        {
            count++;
            scripts = (struct lua_script*)realloc(scripts, count * sizeof(struct lua_script));
            scripts[count - 1].scriptName = entry->d_name;
            scripts[count - 1].scriptPath = get_full_path(path, entry->d_name);
        }
    }

    closedir(dir);

    return scripts;
}

void execute_lua_script(struct lua_script script)
{
    syslog(LOG_INFO, "Executing script: %s", script.scriptPath);
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    luaL_loadfile(L, script.scriptPath);

    if (lua_pcall(L, 0, 0, 0) != 0)
    {
        syslog(LOG_ERR, "Error running script: %s", lua_tostring(L, -1));
        lua_close(L);
        return;
    }

    lua_getglobal(L, "getData");

    if (lua_pcall(L, 0, 1, 0) != 0)
    {
        syslog(LOG_ERR, "Error running script: %s", lua_tostring(L, -1));
        lua_close(L);
        return;
    }

    luaL_checktype(L, -1, LUA_TTABLE);

    lua_pushstring(L, "total");
    lua_gettable(L, -2);
    int total = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L, "free");
    lua_gettable(L, -2);
    int free = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L, "shared");
    lua_gettable(L, -2);
    int shared = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    lua_pushstring(L, "buffered");
    lua_gettable(L, -2);
    int buffered = luaL_checknumber(L, -1);
    lua_pop(L, 1);

    struct MemData memory = {
        .total = total,
        .free = free,
        .shared = shared,
        .buffered = buffered
    };

    syslog(LOG_INFO, "Data from script: %s", construct_JSON_data(memory));

    lua_close(L);
    
}