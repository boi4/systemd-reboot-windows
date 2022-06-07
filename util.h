#pragma once

#include <string>

std::string getLastErrorAsString();

bool getPriviliges();

bool reboot();

std::string u16tou8(std::u16string s);

std::u16string u8tou16(std::string s);
