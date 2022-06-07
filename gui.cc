#include <iostream>

#define GUILITE_ON	// Do not define this macro upon GuiLite.h once more
#include "GuiLite/GuiLite.h"

#include "systemdvars.h"
#include "util.h"




int main() {
    std::vector<std::string> entries = getSystemdBootEntries();


    if (entries.size() == 0) {
        // std::cout << getLastErrorAsString() << std::endl;
        return 1;
    }

    // print all entries with numbers
    size_t i = 1;
    for (std::string &s : entries) {
        std::cout << i << ": " << s << std::endl;
        i++;
    }

    // get user choice
    std::cout << "Your choice: ";
    size_t choice = 0;
    std::cin >> choice;
    if (std::cin.fail() || (choice-1) >= entries.size()) {
        std::cout << "Invalid choice" << std::endl;
        return 1;
    }

    // set entry
    std::string next_entry = entries[choice-1];
    if(!setNextSystemdBootEntry(next_entry)) {
        std::cout << "Failed to set entry to " << next_entry << std::endl;
        return 1;
    }

    std::cout << "Successfully set entry to " << next_entry << std::endl;

    std::cout << "Reboot Now?[y/n]: ";
    std::string response{};
    std::cin >> response;

    if(response.length() == 0 || response[0] == 'y' || response[0] == 'Y') {
        if (!reboot()) {
            std::cout << "Failed to reboot" << std::endl;
            return 1;
        }
    }

    return 0;
}
