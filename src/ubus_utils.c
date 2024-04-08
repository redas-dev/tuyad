#include <stdlib.h>
#include <syslog.h>

#include "ubus_utils.h"

static const struct blobmsg_policy memory_policy[MEMORY_MAX] = {
	[TOTAL_MEMORY]	  = { .name = "total",    .type = BLOBMSG_TYPE_INT64 },
	[FREE_MEMORY]	  = { .name = "free",     .type = BLOBMSG_TYPE_INT64 },
	[SHARED_MEMORY]	  = { .name = "shared",   .type = BLOBMSG_TYPE_INT64 },
	[BUFFERED_MEMORY] = { .name = "buffered", .type = BLOBMSG_TYPE_INT64 },
};

static const struct blobmsg_policy info_policy[INFO_MAX] = {
	[MEMORY_DATA] = { .name = "memory", .type = BLOBMSG_TYPE_TABLE },
};

int connect_to_ubus(struct ubus_context** ctx)
{
    *ctx = ubus_connect(NULL);
    if (!(*ctx)){
        ubus_free(*ctx);
        syslog(LOG_ERR, "%s", "Failed to connect to ubus");
        return -1;
    }
    syslog(LOG_INFO, "%s", "Connected to ubus");

    return 0;
}

static void callback(struct ubus_request *req, int type, struct blob_attr *msg) 
{
    struct MemData *memoryData = (struct MemData *)req->priv;
	struct blob_attr *tb[INFO_MAX];
	struct blob_attr *info[MEMORY_MAX];

	blobmsg_parse(info_policy, INFO_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[MEMORY_DATA]) {
		syslog(LOG_INFO, "%s", "No memory data received\n");
		return;
	}

	blobmsg_parse(memory_policy, MEMORY_MAX, info, blobmsg_data(tb[MEMORY_DATA]), blobmsg_data_len(tb[MEMORY_DATA]));

	memoryData->total    = blobmsg_get_u64(info[TOTAL_MEMORY]);
	memoryData->free     = blobmsg_get_u64(info[FREE_MEMORY]);
	memoryData->shared   = blobmsg_get_u64(info[SHARED_MEMORY]);
	memoryData->buffered = blobmsg_get_u64(info[BUFFERED_MEMORY]);
}

int get_ubus_system_info(struct ubus_context* ctx, struct MemData *memory)
{
    int res = 0;
    uint32_t id = 0;

    res = ubus_lookup_id(ctx, "system", &id);
    if (res != 0){
        syslog(LOG_INFO, "%s", "Failed to get id of system");
        return res;
    }

    res = ubus_invoke(ctx, id, "info", NULL, callback, memory, 3000);

    if (res != 0){
        syslog(LOG_INFO, "%s", "Couldn't invoke info service");
        return res;
    }

    syslog(LOG_INFO, "Total: %ld\n", memory->total);
    syslog(LOG_INFO, "Free: %ld\n", memory->free);
    syslog(LOG_INFO, "Shared: %ld\n", memory->shared);
    syslog(LOG_INFO, "Buffered: %ld\n", memory->buffered);

    return res;
}

void disconnect_from_ubus(struct ubus_context** ctx)
{
    ubus_free(*ctx);
    syslog(LOG_INFO, "%s", "Disconected from ubus");
}