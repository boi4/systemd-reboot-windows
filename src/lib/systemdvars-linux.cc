#ifdef __linux__

#include "lib/systemdvars.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/ioctl.h>
#include <linux/fs.h>

std::string EFIPATH{"/sys/firmware/efi/efivars/"};
//LoaderEntries-4a67b082-0a4c-41cf-b6c7-440b29bb8c4f

std::string getVarPath(std::string varname) {
    return EFIPATH + varname + "-" + SYSTEMD_GUID;
}

bool setMutable(FILE *f) {
    int attrs;
    if (ioctl(fileno(f), FS_IOC_GETFLAGS, &attrs) == -1) {
        return false;
    }
    attrs &= ~FS_IMMUTABLE_FL;
    if (ioctl(fileno(f), FS_IOC_SETFLAGS, &attrs) == -1) {
        return false;
    }
    return true;
}

bool setImmutable(FILE *f) {
    int attrs;
    if (ioctl(fileno(f), FS_IOC_GETFLAGS, &attrs) == -1) {
        return false;
    }
    attrs |= FS_IMMUTABLE_FL;
    if (ioctl(fileno(f), FS_IOC_SETFLAGS, &attrs) == -1) {
        return false;
    }
    return true;
}


std::optional<std::string> readSystemdVar(const std::string varname) {
    std::string name;

    std::ifstream fin(getVarPath(varname), std::ios::binary | std::ios::in);
    if (!fin.is_open()) {
        std::cout << "Failed to open efivars for reading" << std::endl;
        return {};
    }

    std::stringstream buf;
    buf << fin.rdbuf();
    std::string s = buf.str();
    fin.close();

    s.erase(0,4); // remove efivars attributes

    size_t slen = s.length() / 2;
    std::u16string u{reinterpret_cast<char16_t const *>(s.data()), slen};
    std::string res = u16tou8(u);

    res.pop_back(); // remove systemd terminating null byte

    return res;
}


bool setSystemdVar(const std::string varname, const std::string value) {
    std::u16string u = u8tou16(value);
    u.push_back(L'\0'); // add systemd terminating character

    size_t slen = u.length() * 2;
    std::string s{reinterpret_cast<char const *>(u.data()), slen};

    s.insert(0, 1, '\x00'); // add efivars attributes
    s.insert(0, 1, '\x00'); // add efivars attributes
    s.insert(0, 1, '\x00'); // add efivars attributes
    s.insert(0, 1, '\x07'); // add efivars attributes


    std::string fname = getVarPath(varname);

    // make mutable
    // TOCTOU problem
    FILE *f = fopen(fname.data(), "r");
    if (f != NULL) {
        // asume if its NULL if it doesnt exist
        setMutable(f);
        fclose(f);
    }

    // reopen for writing
    f = fopen(fname.data(), "w");
    if (f == NULL) {
        std::cout << "Failed to open efivars for writing" << std::endl;
        return false;
    }

    if (fwrite(s.data(), 1, s.size(), f) != s.size()) {
        std::cout << "Failed to write to efivars" << std::endl;
        return false;
    }
    fclose(f);

    // make mutable
    f = fopen(fname.data(), "r");
    if (f == NULL) {
        std::cout << "Failed to open file for changing attr" << std::endl;
        return false;
    }
    setImmutable(f);
    fclose(f);


    return true;
}

#endif
