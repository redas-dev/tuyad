#include <tuyalink_core.h>
#include "arg_parser.h"

#ifndef _TUYA_CLOUD_
#define _TUYA_CLOUD_
    extern void disconnect_from_tuya(tuya_mqtt_context_t* client);
    extern int connect_to_tuya(tuya_mqtt_context_t* client, struct arguments args);
    extern void on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg);
    extern void on_disconnect(tuya_mqtt_context_t* context, void* user_data);
    extern void on_connected(tuya_mqtt_context_t* context, void* user_data);
    extern void execute_action(tuya_mqtt_context_t* context, const char* action, const char* input);
#endif