#include "Utils.h"
#include <iostream>
#include <limits>

int Utils::readInt(const std::string& message) {
    int value;

    while (true) {
        std::cout << message;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }

        std::cout << "Invalid number. Please try again.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

double Utils::readDouble(const std::string& message) {
    double value;

    while (true) {
        std::cout << message;
        if (std::cin >> value) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return value;
        }

        std::cout << "Invalid amount. Please enter again.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string Utils::readLine(const std::string& message) {
    std::string text;
    std::cout << message;
    std::getline(std::cin, text);
    return text;
}

void Utils::pause() {
    std::cout << "\nPress ENTER to continue...";
    std::cin.get();
}