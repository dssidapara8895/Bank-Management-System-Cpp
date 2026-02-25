#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>

class Account {
private:
    int accountNo;
    std::string name;
    std::string phone;
    int pin;
    double balance;
    int failedAttempts;
    bool locked;

public:
    Account();
    Account(int accNo, const std::string& name,
            const std::string& phone, int pin, double balance);
            int getPin() const;

    // getters
    int getAccountNo() const;
    std::string getName() const;
    std::string getPhone() const;
    double getBalance() const;

    // small checks / updates
    bool checkPin(int enteredPin) const;
    void setName(const std::string& newName);
    void setPhone(const std::string& newPhone);
    void setPin(int newPin);

    // money actions
    bool deposit(double amount);
    bool withdraw(double amount);

    

int getFailedAttempts() const;
bool isLocked() const;

void resetFailedAttempts();
void addFailedAttempt();
void setLocked(bool value);
};

#endif