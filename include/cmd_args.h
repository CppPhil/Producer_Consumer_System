#ifndef INCG_CMD_ARGS_H
#define INCG_CMD_ARGS_H
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool    isOk; /*!< Must be checked before other members are accessed */
    int32_t producerCount;
    int32_t consumerCount;
    int32_t producerSleepTime; /*!< in seconds */
    int32_t consumerSleepTime; /*!< in seconds */
} CmdArgs;

/*!
 * \brief Parses the command line arguments given.
 * \param argc The count of command line arguments from main.
 * \param argv The command line arguments from main.
 * \return The result parsed.
 **/
CmdArgs parseCmdArgs(int argc, char **argv);
#endif /* INCG_CMD_ARGS_H */
