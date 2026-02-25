#ifndef BANKSYSTEM_H
#define BANKSYSTEM_H

#include "Account.h"
#include <vector>
#include <string>

class BankSystem {
private:
    std::vector<Account> accounts;

    // simple text “database” files
    const std::string accountsFile = "accounts.txt";
    const std::string transactionsFile = "transactions.txt";

    int findAccountIndex(int accNo) const;
    int getNextAccountNo() const;

    void logTransaction(int accNo, const std::string& type, double amount, double newBalance);

    void showLastTransactions(int accNo, int count);
    void customerMenu(int index); // cleaner customer menu
    void adminMenu();
    void adminDeleteAccount();
    void removeTransactionsForAccount(int accNo);
    

public:
    BankSystem();

    void loadAccounts();
    void saveAccounts() const;

    void createAccount();
    void login();          // customer login
    void adminLogin();     // admin login (simple password)

    void mainMenu();
};

#endif