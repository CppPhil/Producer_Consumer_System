#ifndef INCG_CMD_ARGS_H
#define INCG_CMD_ARGS_H
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool    isOk;
  int32_t producerCount;
  int32_t consumerCount;
  int32_t producerSleepTime; /*!< in seconds */
  int32_t consumerSleepTime; /*!< in seconds */
} CmdArgs;

CmdArgs parseCmdArgs(int argc, char** argv);
#endif /* INCG_CMD_ARGS_H */
