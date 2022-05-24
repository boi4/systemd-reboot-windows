#pragma once
#include <optional>
#include <string>
#include <vector>

#include <windows.h>

std::vector<std::string> getSystemdBootEntries();

bool setNextSystemdBootEntry(const std::string value);