#include <argp.h>

#ifndef _ARG_DESC_
#define _ARG_DESC_ "Daemon app that exchanges information with Tuya cloud."
#endif

#ifndef _ARG_PARSER_
#define _ARG_PARSER_
  struct arguments
  {
    char* deviceId;
    char* productId;
    char* deviceSecret;
    int isDaemon;
  };

  extern error_t parse_opt (int key, char *arg, struct argp_state *state);
  extern struct argp argp;
  extern struct arguments parse_args(int argc, char** argv);
#endif