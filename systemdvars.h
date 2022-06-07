#pragma once
#include <optional>
#include <string>
#include <cstring>
#include <vector>

#include "util.h"

#ifdef __linux__

#elif _WIN32
    #include <windows.h>
#else
#error Platform not supported
#endif


#define SYSTEMD_GUID "4a67b082-0a4c-41cf-b6c7-440b29bb8c4f"
#define EFIVAR_ENTRIES "LoaderEntries"
#define EFIVAR_NEXT_ENTRY "LoaderEntryOneShot"



std::vector<std::string> getSystemdBootEntries();

bool setNextSystemdBootEntry(const std::string value);


std::optional<std::string> readSystemdVar(const std::string varname);

bool setSystemdVar(const std::string varname, const std::string value);
