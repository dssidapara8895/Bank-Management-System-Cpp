#include <cstdlib>
static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

#include "BankSystem.h"
#include "Utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <vector>

// ---------------- helpers ----------------

BankSystem::BankSystem() {}

int BankSystem::findAccountIndex(int accNo) const {
    for (int i = 0; i < (int)accounts.size(); i++) {
        if (accounts[i].getAccountNo() == accNo)
            return i;
    }
    return -1;
}

int BankSystem::getNextAccountNo() const {
    int maxNo = 1000;
    for (const auto& acc : accounts) {
        if (acc.getAccountNo() > maxNo)
            maxNo = acc.getAccountNo();
    }
    return maxNo + 1;
}

// ---------------- file handling ----------------

void BankSystem::loadAccounts() {
    accounts.clear();

    std::ifstream fin(accountsFile);
    if (!fin.is_open()) return;

    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        if (line[0] == '#') continue;

        std::stringstream ss(line);
        std::string part;

        // accNo|name|phone|pin|balance|failedAttempts|locked
        std::getline(ss, part, '|');
        int accNo = std::stoi(part);

        std::string name;
        std::getline(ss, name, '|');

        std::string phone;
        std::getline(ss, phone, '|');

        std::getline(ss, part, '|');
        int pin = std::stoi(part);

        std::getline(ss, part, '|');
        double balance = std::stod(part);

        int attempts = 0;
        int lockedInt = 0;

        if (std::getline(ss, part, '|')) attempts = std::stoi(part);
        if (std::getline(ss, part, '|')) lockedInt = std::stoi(part);

        Account acc(accNo, name, phone, pin, balance);

        for (int i = 0; i < attempts; i++) acc.addFailedAttempt();
        if (lockedInt == 1) acc.setLocked(true);

        accounts.push_back(acc);
    }
}

void BankSystem::saveAccounts() const {
    std::ofstream fout(accountsFile, std::ios::trunc);

    fout << "# ===== BANK ACCOUNTS DATABASE =====\n";
    fout << "# Format: accNo|name|phone|pin|balance|failedAttempts|locked\n";
    fout << "# ----------------------------------\n";

    for (const auto& acc : accounts) {
        fout << acc.getAccountNo() << "|"
             << acc.getName() << "|"
             << acc.getPhone() << "|"
             << acc.getPin() << "|"
             << std::fixed << std::setprecision(2) << acc.getBalance() << "|"
             << acc.getFailedAttempts() << "|"
             << (acc.isLocked() ? 1 : 0)
             << "\n";
    }
}

// ---------------- transactions ----------------

void BankSystem::logTransaction(int accNo, const std::string& type, double amount, double newBalance) {
    bool isEmpty = true;
    {
        std::ifstream check(transactionsFile);
        if (check.is_open())
            isEmpty = (check.peek() == std::ifstream::traits_type::eof());
    }

    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);

    std::ofstream fout(transactionsFile, std::ios::app);

    if (isEmpty) {
        fout << "# ===== TRANSACTION LOG =====\n";
        fout << "# Format: accNo|YYYY-MM-DD HH:MM|TYPE|AMOUNT|BALANCE_AFTER\n";
        fout << "# ----------------------------------\n";
    }

    fout << accNo << "|"
         << (t->tm_year + 1900) << "-"
         << std::setw(2) << std::setfill('0') << (t->tm_mon + 1) << "-"
         << std::setw(2) << std::setfill('0') << t->tm_mday << " "
         << std::setw(2) << std::setfill('0') << t->tm_hour << ":"
         << std::setw(2) << std::setfill('0') << t->tm_min << "|"
         << type << "|"
         << std::fixed << std::setprecision(2) << amount << "|"
         << std::fixed << std::setprecision(2) << newBalance
         << "\n";
}
void BankSystem::showLastTransactions(int accNo, int count) {
    std::ifstream fin(transactionsFile);
    if (!fin.is_open()) {
        std::cout << "No transactions yet.\n";
        return;
    }

    std::vector<std::string> lines;
    std::string line;

    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        if (line[0] == '#') continue;

        std::stringstream ss(line);
        std::string part;

        std::getline(ss, part, '|');
        int fileAccNo = std::stoi(part);

        if (fileAccNo == accNo)
            lines.push_back(line);
    }

    if (lines.empty()) {
        std::cout << "No transactions found.\n";
        return;
    }

    std::cout << "\nAccNo | DateTime         | Type         | Amount   | Balance\n";
    std::cout << "-----------------------------------------------------------\n";

    int start = (int)lines.size() - count;
    if (start < 0) start = 0;

    for (int i = start; i < (int)lines.size(); i++) {
        std::stringstream ss(lines[i]);
        std::string a, dt, type, amt, bal;

        std::getline(ss, a, '|');
        std::getline(ss, dt, '|');
        std::getline(ss, type, '|');
        std::getline(ss, amt, '|');
        std::getline(ss, bal, '|');

        std::cout << a << " | " << dt << " | "
                  << std::setw(11) << std::left << type << " | "
                  << std::setw(7) << std::right << amt << " | "
                  << bal << "\n";
    }
}


// ---------------- customer menu ----------------

void BankSystem::customerMenu(int index) {
    int accNo = accounts[index].getAccountNo();
    clearScreen();

std::cout << "=====================================\n";
std::cout << "           CUSTOMER DASHBOARD        \n";
std::cout << "=====================================\n";
std::cout << "Name       : " << accounts[index].getName() << "\n";
std::cout << "Account No : " << accounts[index].getAccountNo() << "\n";
std::cout << "Balance    : $" << std::fixed << std::setprecision(2)
          << accounts[index].getBalance() << "\n";
std::cout << "-------------------------------------\n";

    while (true) {
        std::cout << "\n------ Customer Menu ------\n";
        std::cout << "1) Check Balance\n";
        std::cout << "2) Deposit\n";
        std::cout << "3) Withdraw\n";
        std::cout << "4) View Last 10 Transactions\n";
        std::cout << "5) Change PIN\n";
        std::cout << "6) Update Profile (Name/Phone)\n";
        std::cout << "7) Transfer Money\n";
        std::cout << "8) Close Account\n";
        std::cout << "0) Logout\n";

        int choice = Utils::readInt("Choose: ");

        if (choice == 1) {
            std::cout << "Balance: $" << std::fixed << std::setprecision(2)
                      << accounts[index].getBalance() << "\n";
            Utils::pause();
        }
        else if (choice == 2) {
            double amount = Utils::readDouble("Deposit amount: ");
            if (accounts[index].deposit(amount)) {
                saveAccounts();
                logTransaction(accNo, "DEPOSIT", amount, accounts[index].getBalance());
                std::cout << "Deposit done.\n";
            } else {
                std::cout << "Invalid amount.\n";
            }
            Utils::pause();
        }
        else if (choice == 3) {
            double amount = Utils::readDouble("Withdraw amount: ");
            if (accounts[index].withdraw(amount)) {
                saveAccounts();
                logTransaction(accNo, "WITHDRAW", amount, accounts[index].getBalance());
                std::cout << "Withdraw done.\n";
            } else {
                std::cout << "Not enough balance / invalid amount.\n";
            }
            Utils::pause();
        }
        else if (choice == 4) {
            showLastTransactions(accNo, 10);
            Utils::pause();
        }
        else if (choice == 5) {
            int oldPin = Utils::readInt("Enter current PIN: ");
            if (!accounts[index].checkPin(oldPin)) {
                std::cout << "Wrong current PIN.\n";
                Utils::pause();
                continue;
            }

            int newPin;
            while (true) {
                newPin = Utils::readInt("Enter new 4-digit PIN: ");
                if (newPin >= 1000 && newPin <= 9999) break;
                std::cout << "PIN must be 4 digits.\n";
            }

            accounts[index].setPin(newPin);
            saveAccounts();
            std::cout << "PIN updated.\n";
            Utils::pause();
        }
        else if (choice == 6) {
            std::string newName = Utils::readLine("New name (leave blank to skip): ");
            std::string newPhone = Utils::readLine("New phone (leave blank to skip): ");

            if (!newName.empty()) accounts[index].setName(newName);
            if (!newPhone.empty()) accounts[index].setPhone(newPhone);

            saveAccounts();
            std::cout << "Profile updated.\n";
            Utils::pause();
        }
        else if (choice == 7) {
            int toAcc = Utils::readInt("Transfer to Account Number: ");
            if (toAcc == accNo) {
                std::cout << "You cannot transfer to same account.\n";
                Utils::pause();
                continue;
            }

            int toIndex = findAccountIndex(toAcc);
            if (toIndex == -1) {
                std::cout << "Receiver account not found.\n";
                Utils::pause();
                continue;
            }

            if (accounts[toIndex].isLocked()) {
                std::cout << "Receiver account is locked.\n";
                Utils::pause();
                continue;
            }

            double amount = Utils::readDouble("Transfer amount: ");
            if (amount <= 0) {
                std::cout << "Invalid amount.\n";
                Utils::pause();
                continue;
            }

            if (!accounts[index].withdraw(amount)) {
                std::cout << "Not enough balance.\n";
                Utils::pause();
                continue;
            }

            accounts[toIndex].deposit(amount);

            saveAccounts();
            logTransaction(accNo, "TRANSFER_OUT", amount, accounts[index].getBalance());
            logTransaction(toAcc, "TRANSFER_IN", amount, accounts[toIndex].getBalance());

            std::cout << "Transfer successful.\n";
            Utils::pause();
        }
        else if (choice == 8) {
            int pin = Utils::readInt("Confirm PIN to close account: ");
            if (!accounts[index].checkPin(pin)) {
                std::cout << "Wrong PIN.\n";
                Utils::pause();
                continue;
            }

            std::string confirm = Utils::readLine("Type YES to confirm closing: ");
            if (confirm == "YES") {
                accounts.erase(accounts.begin() + index);
                saveAccounts();
                removeTransactionsForAccount(accNo);
                std::cout << "Account closed successfully.\n";
                Utils::pause();
                return;
            }

            std::cout << "Cancelled.\n";
            Utils::pause();
        }
        else if (choice == 0) {
            return;
        }
        else {
            std::cout << "Invalid choice.\n";
        }
    }
}

// ---------------- main features ----------------

void BankSystem::createAccount() {
    std::string name = Utils::readLine("Enter full name: ");
    std::string phone = Utils::readLine("Enter phone number: ");

    int pin;
    while (true) {
        pin = Utils::readInt("Create 4-digit PIN: ");
        if (pin >= 1000 && pin <= 9999) break;
        std::cout << "PIN must be 4 digits only.\n";
    }

    double startMoney = Utils::readDouble("Initial deposit: ");
    if (startMoney < 0) startMoney = 0;

    int newAccNo = getNextAccountNo();
    accounts.push_back(Account(newAccNo, name, phone, pin, startMoney));
    saveAccounts();

    std::cout << "\nAccount created successfully!\n";
    std::cout << "Your Account Number: " << newAccNo << "\n";
    Utils::pause();
}

void BankSystem::login() {
    int accNo = Utils::readInt("Account Number: ");

    int index = findAccountIndex(accNo);
    if (index == -1) {
        std::cout << "Account not found.\n";
        Utils::pause();
        return;
    }

    if (accounts[index].isLocked()) {
        std::cout << "This account is locked. Contact admin.\n";
        Utils::pause();
        return;
    }

    while (true) {
        int pin = Utils::readInt("PIN: ");

        if (accounts[index].checkPin(pin)) {
            accounts[index].resetFailedAttempts();
            saveAccounts();
            customerMenu(index);
            return;
        }

        accounts[index].addFailedAttempt();
        saveAccounts();

        if (accounts[index].isLocked()) {
            std::cout << "Too many wrong tries. Account locked.\n";
            Utils::pause();
            return;
        }

        std::cout << "Wrong PIN. Try again.\n";
    }
}

// ---------------- admin ----------------

void BankSystem::adminLogin() {
    int pass = Utils::readInt("Enter admin password (1234): ");

    if (pass != 1234) {
        std::cout << "Wrong password.\n";
        Utils::pause();
        return;
    }

    adminMenu();
}

void BankSystem::adminMenu() {
    while (true) {
        std::cout << "\n====== ADMIN PANEL ======\n";
        std::cout << "1) View All Accounts\n";
        std::cout << "2) Delete Account\n";
        std::cout << "0) Back\n";

        int choice = Utils::readInt("Choose: ");

        if (choice == 1) {
            std::cout << "\n--- All Accounts ---\n";
            for (const auto& acc : accounts) {
                std::cout << "Acc: " << acc.getAccountNo()
                          << " | Name: " << acc.getName()
                          << " | Balance: $" << std::fixed << std::setprecision(2) << acc.getBalance()
                          << " | Locked: " << (acc.isLocked() ? "Yes" : "No")
                          << "\n";
            }
            Utils::pause();
        }
        else if (choice == 2) {
            adminDeleteAccount();
        }
        else if (choice == 0) {
            return;
        }
        else {
            std::cout << "Invalid choice.\n";
        }
    }
}

void BankSystem::adminDeleteAccount() {
    int accNo = Utils::readInt("Enter Account Number to delete: ");

    int index = findAccountIndex(accNo);
    if (index == -1) {
        std::cout << "Account not found.\n";
        Utils::pause();
        return;
    }

    std::cout << "\nYou are deleting:\n";
    std::cout << "Acc: " << accounts[index].getAccountNo()
              << " | Name: " << accounts[index].getName()
              << " | Balance: $" << std::fixed << std::setprecision(2) << accounts[index].getBalance()
              << "\n";

    std::string confirm = Utils::readLine("Type DELETE to confirm: ");
    if (confirm != "DELETE") {
        std::cout << "Cancelled.\n";
        Utils::pause();
        return;
    }

    accounts.erase(accounts.begin() + index);
    saveAccounts();
    removeTransactionsForAccount(accNo);

    std::cout << "Account deleted successfully.\n";
    Utils::pause();
}

void BankSystem::removeTransactionsForAccount(int accNo) {
    std::ifstream fin(transactionsFile);
    if (!fin.is_open()) return;

    std::vector<std::string> kept;
    std::string line;

    while (std::getline(fin, line)) {
        if (line.empty()) continue;

        if (line[0] == '#') {
            kept.push_back(line);
            continue;
        }

        std::stringstream ss(line);
        std::string part;

        std::getline(ss, part, '|');

        int fileAccNo = -1;
        try {
            fileAccNo = std::stoi(part);
        } catch (...) {
            kept.push_back(line);
            continue;
        }

        if (fileAccNo != accNo) {
            kept.push_back(line);
        }
    }

    fin.close();

    std::ofstream fout(transactionsFile, std::ios::trunc);
    for (const auto& s : kept) {
        fout << s << "\n";
    }
}

// ---------------- menu ----------------

void BankSystem::mainMenu() {
    loadAccounts();

    while (true) {
        clearScreen();
        std::cout << "=====================================\n";
std::cout << "        BANK MANAGEMENT SYSTEM       \n";
std::cout << "=====================================\n";
std::cout << "1) Create Account\n";
std::cout << "2) Login\n";
std::cout << "3) Admin Panel\n";
std::cout << "0) Exit\n";
std::cout << "-------------------------------------\n";

        int choice = Utils::readInt("Choose: ");

        if (choice == 1) createAccount();
        else if (choice == 2) login();
        else if (choice == 3) adminLogin();
        else if (choice == 0) break;
        else std::cout << "Invalid choice.\n";
    }
}