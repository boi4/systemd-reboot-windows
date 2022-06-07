#ifdef _WIN32

#include "systemdvars.h"

// TODO: get rid of wchar_t and use utf-16

std::optional<std::string> readSystemdVar(const std::string varname) {
    wchar_t raw[0x1000] = {0};
    PVOID pBuffer{&raw};
    DWORD nSize{0x1000};

    size_t varLength{0};
    size_t varNumChars{0};

    varLength = GetFirmwareEnvironmentVariableA(varname.c_str(), "{" SYSTEMD_GUID "}", pBuffer, nSize);

    if (varLength == 0) {
        return {};
    }

    varNumChars = varLength / 2;

    std::wstring w(raw, varNumChars);
    std::string s(w.begin(), w.end());

    return s;
}

bool setSystemdVar(const std::string varname, const std::string value) {
    std::wstring w(value.begin(),value.end());
    w.append(1,L'\0');

    wchar_t *raw = new wchar_t[w.length()];
    for (size_t i = 0; i < w.length(); i++) {
        raw[i] = w[i];
    }
    PVOID pBuffer{raw};
    DWORD nSize{2 * (uint32_t)w.length()}; // size in bytes

    bool ret = SetFirmwareEnvironmentVariableA(varname.c_str(), "{" SYSTEMD_GUID "}", pBuffer, nSize) != 0;
    delete [] raw;
    return ret;
}



#endif
