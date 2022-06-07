#ifdef __linux__
#include "util.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/reboot.h>


// taken from https://stackoverflow.com/a/17387176
std::string getLastErrorAsString()
{
    return strerror(errno);
}

bool getPriviliges() {
    return true;
}

bool reboot() {
    sync();
    reboot(RB_AUTOBOOT);
    return true;
}

#endif
