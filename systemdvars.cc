#include "systemdvars.hpp"

#include <iostream>

auto SYSTEMD_GUID{"{4a67b082-0a4c-41cf-b6c7-440b29bb8c4f}"};

auto EFIVAR_ENTRIES{"LoaderEntries"};
auto EFIVAR_NEXT_ENTRY{"LoaderEntryOneShot"};



//// debug
//std::string hex(const std::string& s)
//{
//    char ds[] = "0123456789ABCDEF";
//
//    std::string res;
//    for (uint8_t c : s) {
//        res.push_back(ds[c>>4]);
//        res.push_back(ds[c&0xf]);
//        res.push_back(' ');
//    }
//
//    return res;
//}
//
//// debug
//void printStringInfo(std::string s) {
//    std::cout << "Len: ";
//    std::cout << s.size() << std::endl;
//    std::cout << "Str: ";
//    std::cout << s << std::endl;
//    std::cout << "Hex: ";
//    std::cout << hex(s) << std::endl;
//}


std::optional<std::string> readSystemdVar(const std::string varname) {
    wchar_t raw[0x1000] = {0};
    PVOID pBuffer{&raw};
    DWORD nSize{0x1000};

    size_t varLength{0};
    size_t varNumChars{0};

    varLength = GetFirmwareEnvironmentVariableA(varname.c_str(), SYSTEMD_GUID, pBuffer, nSize);

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

    bool ret = SetFirmwareEnvironmentVariableA(varname.c_str(), SYSTEMD_GUID, pBuffer, nSize) != 0;
    delete [] raw;
    return ret;
}


std::vector<std::string> getSystemdBootEntries() {
    std::vector<std::string> res{};

    auto entries_var = readSystemdVar(EFIVAR_ENTRIES);
    if (!entries_var) {
        return {};
    }

    char const *base = entries_var->c_str();
    for (char const *p = base; (size_t)(p - base) < entries_var->length(); p += strlen(p) + 1) {
        std::string entry{p};
        if (entry.size()) {
            res.push_back(entry);
        }
    }


    return res;
}



bool setNextSystemdBootEntry(const std::string value) {
    if(!setSystemdVar(EFIVAR_NEXT_ENTRY, value)) {
        return false;
    }
    return true;
}