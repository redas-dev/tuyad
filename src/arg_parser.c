#include <argp.h>
#include <string.h>
#include "arg_parser.h"
#include <stdlib.h>

static struct argp_option options[] = {
  { "product-id", 'p', "value", OPTION_ARG_OPTIONAL, "Product ID"},
  { "device-id" , 'd', "value", OPTION_ARG_OPTIONAL, "Device ID"},
  { "secret"    , 's', "value", OPTION_ARG_OPTIONAL, "Device secret"},
  { "daemon"    , 'D', 0      , OPTION_ARG_OPTIONAL, "Make program a daemon"},
  { 0 }
};
struct argp argp = { options, parse_opt, NULL, _ARG_DESC_ };

extern error_t parse_opt(int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;
  switch (key)
  {
    case 'p':
      if (arg != NULL){
        arguments->productId = arg;
      }
      break;
    case 'd':
      if (arg != NULL){
        arguments->deviceId = arg;
      }
      break;
    case 's':
      if (arg != NULL){
        arguments->deviceSecret = arg;
      }
      break;
    case 'D':
      arguments->isDaemon = 1;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}


extern struct arguments parse_args(int argc, char** argv)
{
  struct arguments args;

  args.deviceId = "";
  args.deviceSecret = "";
  args.productId = "";
  args.isDaemon = 0;

  argp_parse(&argp, argc, argv, 0, 0, &args);
  return args;
}