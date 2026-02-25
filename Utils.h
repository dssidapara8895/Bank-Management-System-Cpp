#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace Utils {

    int readInt(const std::string& message);
    double readDouble(const std::string& message);
    std::string readLine(const std::string& message);

    void pause();   // small helper to stop screen

}

#endif