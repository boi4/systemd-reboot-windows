#pragma once

#include <string>
#include <filesystem>

std::string getLastErrorAsString();

bool getPriviliges();

bool reboot();

std::string u16tou8(std::u16string s);

std::u16string u8tou16(std::string s);


std::filesystem::path get_config_file_path();