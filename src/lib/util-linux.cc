#ifdef __linux__
#include "lib/util.h"

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
std::filesystem::path get_config_file_path() {
    // Beware, brain-compiled code ahead!
    char *appdata = getenv("XDG_CONFIG_HOME");
    std::filesystem::path result;
    if (appdata != NULL) {
        result = std::filesystem::path(appdata);
    } else {
        result = std::filesystem::path(getenv("HOME"));
        result = result / ".config";
    }
    result = result / "systemd-reboot";
    return result;
}

#endif
