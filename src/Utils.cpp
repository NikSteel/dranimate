#include "Utils.h"

//http://stackoverflow.com/questions/874134/find-if-string-endswith-another-string-in-c

bool Utils::hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}