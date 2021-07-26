#include <stdio.h>
#include <string.h>

#include "cmd_args.h"

/*!
 * \brief Checks if a given index is valid for argc.
 * \param argc The argument count from main.
 * \param index The index to check.
 * \return true if `index` is valid for `argc`; otherwise false.
 **/
static bool isValidIndex(int argc, int index)
{
  return index >= 0 && index < argc;
}

/*!
 * \brief Checks if a given character contains a decimal digit.
 * \param character The character to check.
 * \return true if `character` contains a decimal digit; otherwise false.
 **/
static bool isDigit(char character)
{
  return character >= '0' && character <= '9';
}

/*!
 * \brief Parses a number out of a string.
 * \param string The string to parse.
 * \param number Output parameter for the number that has been parsed out of the string.
 * \return true if the function exits successfully; otherwise false.
 * \warning The return value must be checked before *number may be accessed.
 **/
static bool parseNumber(const char* string, int32_t* number)
{
  int32_t value = 0; // Buffer to hold the number.

  for (const char* p = string; *p != '\0'; ++p) {
    // If it has no digit -> failure.
    if (!isDigit(*p)) { return false; }

    // Add the current digit.
    value += *p - '0';

    // Shift the digit to the left by one position.
    // We do this one too many times (it's not right)
    // for the very last digit (reading from the right hand side).
    // Therefore we have to divide by 10 after the loop to undo
    // one of these multiplications.
    value *= 10;
  }

  // Correct the value.
  // Doing this outside of the loop is (probably) more efficient
  // than introducing a branch in the for loop.
  value /= 10;

  // On success: write the value through the output parameter.
  *number = value;
  return true;
}

/*!
 * \brief Prints usage instructions from this application.
 * \param programName Should be argv[0].
 **/
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

// Macro to parse a number; does a goto error on failure.
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

    // Parse the arguments.
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
