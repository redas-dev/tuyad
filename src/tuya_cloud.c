#include <cJSON.h>
#include "tuya_cacert.h"
#include <tuya_log.h>
#include <tuya_error_code.h>
#include <system_interface.h>
#include <mqtt_client_interface.h>

#include "tuya_cloud.h"
#include "utils.h"

#include <syslog.h>
#include <stdlib.h>

struct asd {
  char* test;
  int asd;
};

// Execute action sent from the cloud
extern void execute_action(tuya_mqtt_context_t* context, const char* action, const char* input)
{
  syslog(LOG_INFO, "Executing action: %s", action);

  if (strcmp(action, "write") == 0){
    FILE* fout = NULL;
    char* path = get_full_path(getenv("HOME"), "FileWithCloudInput.txt");

    if ((fout = fopen(path, "w")) == NULL)
    {
      free(path);
      return syslog(LOG_ERR, "Action '%s' failed", action);
    }

    fprintf(fout, "%s", input);
    free(path);
    fclose(fout);
  }

  syslog(LOG_INFO, "Action '%s' executed", action);
}

void on_connected(tuya_mqtt_context_t* context, void* user_data)
{
  syslog(LOG_INFO, "%s", "Connected to Tuya IOT");
}

void on_disconnect(tuya_mqtt_context_t* context, void* user_data)
{
  syslog(LOG_INFO, "%s", "Disconnected from Tuya IOT");
}

// Parse Tuya messages
void on_messages(tuya_mqtt_context_t* context, void* user_data, const tuyalink_message_t* msg)
{
  syslog(LOG_INFO, "On message id:%s, type:%d, code:%d", msg->msgid, msg->type, msg->code);

  switch (msg->type) {
    case THING_TYPE_PROPERTY_SET:
      syslog(LOG_INFO, "Property set: %s", msg->data_string);
      break;
    case THING_TYPE_ACTION_EXECUTE:
      syslog(LOG_INFO, "Action execute: %s", msg->data_string);

      cJSON* actionJSON = cJSON_Parse(msg->data_string);
      cJSON* action = cJSON_GetObjectItem(actionJSON, "actionCode");

      cJSON* inputJSON = cJSON_GetObjectItem(actionJSON, "inputParams");
      cJSON* input = cJSON_GetObjectItem(inputJSON, "input");

      execute_action(context, action->valuestring, input->valuestring);

      cJSON_DeleteItemFromObject(actionJSON, "actionCode");
      cJSON_DeleteItemFromObject(actionJSON, "inputParams");
      cJSON_Delete(actionJSON);
      break;
    default:
      syslog(LOG_INFO, "Default: %s", msg->data_string);
    break;
  }
}

extern int connect_to_tuya(tuya_mqtt_context_t* client, struct arguments args)
{
  syslog(LOG_INFO, "%s", "Connecting to Tuya IOT");

  int ret = OPRT_OK;

  struct asd* test = (struct asd*)malloc(sizeof(struct asd));
  test->test = "asd";
  test->asd = 50;

  // Initialize SDK
  ret = tuya_mqtt_init(client, &(const tuya_mqtt_config_t) {
    .host = "m1.tuyacn.com",
    .port = 8883,
    .cacert = tuya_cacert_pem,
    .cacert_len = sizeof(tuya_cacert_pem),
    .device_id = args.deviceId,
    .device_secret = args.deviceSecret,
    .keepalive = 60,
    .timeout_ms = 2000,
    .on_connected = on_connected,
    .on_disconnect = on_disconnect,
    .on_messages = on_messages,
    .user_data = test
  });

  if (ret != OPRT_OK){
    syslog(LOG_ERR, "%s", "Tuya IOT SDK initialization failed");
    return ret;
  }
  syslog(LOG_INFO, "%s", "Tuya IOT SDK initialization successful");

  // Connect to Tuya
  ret = tuya_mqtt_connect(client);
  if (ret != OPRT_OK){
    syslog(LOG_ERR, "%s", "Connection to Tuya IOT failed");
    return ret;
  }

  syslog(LOG_INFO, "%s", "Connection to Tuya IOT was successful");

  return ret;
}

// Disconnect from Tuya cloud and deinitialize the SDK
extern void disconnect_from_tuya(tuya_mqtt_context_t* client)
{
  tuya_mqtt_disconnect(client);
  tuya_mqtt_deinit(client);
}