#define _DEFAULT_SOURCE

#include <stdlib.h>
#include <signal.h>

#include <tuya_error_code.h>
#include <tuya_log.h>

#include "arg_parser.h"
#include "utils.h"
#include "tuya_cloud.h"
#include "make_daemon.h"
#include "entry.h"
#include "ubus_utils.h"

#include <syslog.h>

tuya_mqtt_context_t client_instance;

volatile sig_atomic_t signal_sent = 0;

// Handle sigint and sigquit signals
void sig_handler(int signum)
{
    signal_sent = 1;
}

int entry(int argc, char** argv)
{
    openlog("tuyad", LOG_PID, LOG_DAEMON);

    struct lua_script* scripts = get_lua_scripts(NULL);
    execute_lua_script(scripts[1]);
    execute_lua_script(scripts[0]);
    
    struct sigaction sa = { .sa_handler = sig_handler };
    sigaction(SIGINT, &sa, 0);
    sigaction(SIGQUIT, &sa, 0);

    int ret = OPRT_OK;

    struct arguments args = parse_args(argc, argv);

    if(args.isDaemon)
        make_daemon(0);

    tuya_mqtt_context_t* client = &client_instance;
    ret = connect_to_tuya(client, args);

    if(ret != OPRT_OK){
        disconnect_from_tuya(client);
        closelog();
        return EXIT_FAILURE;
    }

    struct ubus_context* ctx;
    ret = connect_to_ubus(&ctx);

    if (ret != 0){
        disconnect_from_ubus(&ctx);
        closelog();
        return EXIT_FAILURE;
    }

    struct MemData mem = { 0 };

    // Send router memory usage data to Tuya
    while (!signal_sent) {
        tuya_mqtt_loop(client);

        get_ubus_system_info(ctx, &mem);
        char* report = construct_JSON_data(mem);

        tuyalink_thing_property_report(client, args.deviceId, report);
        free(report);
    }

    // Disconnect and free memory
    syslog(LOG_INFO, "%s", "Starting cleanup");

    disconnect_from_tuya(client);
    disconnect_from_ubus(&ctx);

    syslog(LOG_INFO, "%s", "Cleanup ended");
    closelog();

    return 0;
}