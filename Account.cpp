#include "Account.h"

// empty account (used when loading from file etc.)
Account::Account() {
    accountNo = 0;
    name = "";
    phone = "";
    pin = 0;
    balance = 0.0;
    failedAttempts = 0;
    locked = false;
}

// real account constructor
Account::Account(int accNo, const std::string& name,
                 const std::string& phone, int pin, double balance) {
    this->accountNo = accNo;
    this->name = name;
    this->phone = phone;
    this->pin = pin;
    this->balance = balance;

    failedAttempts = 0;
    locked = false;
}

// basic info
int Account::getAccountNo() const {
    return accountNo;
}



std::string Account::getName() const {
    return name;
}

std::string Account::getPhone() const {
    return phone;
}

double Account::getBalance() const {
    return balance;
}

// security
bool Account::checkPin(int enteredPin) const {
    return enteredPin == pin;
}

// updates
void Account::setName(const std::string& newName) {
    name = newName;
}

void Account::setPhone(const std::string& newPhone) {
    phone = newPhone;
}

void Account::setPin(int newPin) {
    pin = newPin;
}

// money logic
bool Account::deposit(double amount) {
    if (amount <= 0) return false;

    balance += amount;
    return true;
}

bool Account::withdraw(double amount) {
    if (amount <= 0) return false;
    if (amount > balance) return false;

    balance -= amount;
    return true;
}

int Account::getPin() const {
    return pin;
}

int Account::getFailedAttempts() const {
    return failedAttempts;
}

bool Account::isLocked() const {
    return locked;
}

void Account::resetFailedAttempts() {
    failedAttempts = 0;
}

void Account::addFailedAttempt() {
    failedAttempts++;

    if (failedAttempts >= 3) {
        locked = true;
    }
}

void Account::setLocked(bool value) {
    locked = value;
    if (!locked) {
        failedAttempts = 0; // unlock means reset attempts
    }
}