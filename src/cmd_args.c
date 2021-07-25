#include <stdio.h>
#include <string.h>

#include "cmd_args.h"

static bool isValidIndex(int argc, int index)
{
  return index >= 0 && index < argc;
}

static bool isDigit(char character)
{
  return character >= '0' && character <= '9';
}

static bool parseNumber(const char* string, int32_t* number)
{
  int32_t value = 0;

  for (const char* p = string; *p != '\0'; ++p) {
    if (!isDigit(*p)) { return false; }

    value += *p - '0';
    value *= 10;
  }

  value /= 10;

  *number = value;
  return true;
}

static void printUsage(const char* programName)
{
  fprintf(stderr, "%s: Invalid command line parameters\n\n", programName);
  fprintf(
    stderr,
    "usage: %s --producerCount <prodCount> --consumerCount <consCount> "
    "--producerSleepTime <prodSleepTimeSeconds> --consumerSleepTime "
    "<consSleepTimeSeconds>\n\n",
    programName);
  fprintf(stderr, "Example:\n");
  fprintf(
    stderr,
    "  %s --producerCount 3 --consumerCount 2 --producerSleepTime 5 "
    "--consumerSleepTime 3\n\n",
    programName);
}

CmdArgs parseCmdArgs(int argc, char** argv)
{
  CmdArgs retVal = {false, 0, 0, 0, 0};

  const int expectedArgc = 9; /* 8 arguments plus the program name */

  if (argc != expectedArgc) {
    fprintf(
      stderr,
      "\nInvalid count of command line arguments.\nExpected: %d\nActual  : "
      "%d\n\n",
      expectedArgc - 1,
      argc - 1);
    goto error;
  }

  for (int index = 1; index < argc; ++index) {
    const char* const arg       = argv[index];
    const int         nextIndex = index + 1;

#define TRY_PARSE(what)                                                  \
  do {                                                                   \
    if (strcmp("--" #what, arg) == 0) {                                  \
      if (isValidIndex(argc, nextIndex)) {                               \
        if (!parseNumber(argv[nextIndex], &retVal.what)) { goto error; } \
      }                                                                  \
      else {                                                             \
        goto error;                                                      \
      }                                                                  \
    }                                                                    \
  } while (false)

    TRY_PARSE(producerCount);
    TRY_PARSE(consumerCount);
    TRY_PARSE(producerSleepTime);
    TRY_PARSE(consumerSleepTime);
  }

  retVal.isOk = true;
  return retVal;

error:
  printUsage(argv[0]);
  return (CmdArgs){false, 0, 0, 0, 0};
}
