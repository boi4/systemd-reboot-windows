#include "util.h"

#include <codecvt>
#include <locale>

std::string u16tou8(std::u16string s) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
    return convert.to_bytes(s);
}

std::u16string u8tou16(std::string s) {
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>,char16_t> convert;
    return convert.from_bytes(s);
}
