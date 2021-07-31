#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "sleep_thread.h"

void sleepThread(int32_t seconds)
{
#ifdef _WIN32
    Sleep(/* dwMilliseconds */ (DWORD)(seconds * 1000));
#else
    sleep((unsigned) seconds);
#endif
}
