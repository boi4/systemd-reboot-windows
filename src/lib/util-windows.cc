#ifdef _WIN32
#include "lib/util.h"

#include <windows.h>
#include <iostream>
#include <filesystem>



// taken from https://stackoverflow.com/a/17387176
std::string getLastErrorAsString()
{
    //Get the error message ID, if any.
    DWORD errorMessageID = ::GetLastError();
    if(errorMessageID == 0) {
        return std::string(); //No error message has been recorded
    }

    LPSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    //Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);

    return message;
}

bool getPrivilege(std::string privilegeName) {
    bool ok;
    TOKEN_PRIVILEGES tp;
    HANDLE at;
    LUID sys_env_priv_luid;


    // get privilege luid
    ok = LookupPrivilegeValueA(NULL, privilegeName.c_str(), &sys_env_priv_luid);
    if (!ok) {
        return false;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0] = {sys_env_priv_luid, SE_PRIVILEGE_ENABLED};

    // get process token
    ok = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES, &at);
    if (!ok) {
        return false;
    }

    // add
    ok = AdjustTokenPrivileges(at, false, &tp, 0, NULL, NULL);
    if (!ok) {
        CloseHandle(at);
        return false;
    }


    return true;
}

bool getPriviliges() {
    if(!getPrivilege("SeSystemEnvironmentPrivilege")) {
        std::cout << getLastErrorAsString() << std::endl;
        return false;
    }
    if(!getPrivilege("SeShutdownPrivilege")) {
        std::cout << getLastErrorAsString() << std::endl;
        return false;
    }
    return true;
}

bool reboot() {
    return 0 == InitiateShutdownA(NULL, NULL, 0, SHUTDOWN_RESTART, SHTDN_REASON_MAJOR_APPLICATION | SHTDN_REASON_MINOR_OTHER | SHTDN_REASON_FLAG_PLANNED);
}


std::filesystem::path get_config_file_path() {
    // Beware, brain-compiled code ahead!
    char *appdata = getenv("APPDATA");
    std::filesystem::path result{appdata};
    result = result / "systemd-reboot";
    return result;
}

#endif
