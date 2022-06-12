#include "lib/systemdvars.h"


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
