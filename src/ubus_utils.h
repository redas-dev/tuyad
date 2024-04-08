#include <libubox/blobmsg_json.h>
#include <libubus.h>

#ifndef _UBUS_UTILS_
#define _UBUS_UTILS_
    struct MemData {
        long total;
        long free;
        long shared;
        long buffered;
    };

    enum {
        TOTAL_MEMORY,
        FREE_MEMORY,
        SHARED_MEMORY,
        BUFFERED_MEMORY,
        MEMORY_MAX,
    };

    enum {
        MEMORY_DATA,
        INFO_MAX,
    };
    int connect_to_ubus(struct ubus_context** ctx);
    int get_ubus_system_info(struct ubus_context* ctx, struct MemData *memory);
    void disconnect_from_ubus(struct ubus_context** ctx);
#endif