#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
using namespace std;

// ================= ENCRYPTION =================
string encryptData(string s) {
    for (char &c : s) c += 3;
    return s;
}
string decryptData(string s) {
    for (char &c : s) c -= 3;
    return s;
}

// ================= TRANSACTION STRUCT =================
struct Transaction {
    string accNumber;
    string type;
    double amount;
    string timeStamp;
};

// ================= ACCOUNT BASE =================
class Account {
public:
    string accNumber;
    string name;
    string branch;
    string pin;
    double balance;

    Account() { balance = 0; }

    virtual void deposit(double amt) {
        balance += amt;
    }

    virtual bool withdraw(double amt) {
        if (amt > balance) return false;
        balance -= amt;
        return true;
    }

    virtual void display() {
        cout << "\nAccount: " << accNumber;
        cout << "\nName: " << name;
        cout << "\nBalance: R" << balance << endl;
    }
};

// ================= DERIVED ACCOUNTS =================
class Savings : public Account {};
class Cheque : public Account {};
class FixedDeposit : public Account {};
class Student : public Account {};

// ================= STORAGE =================
vector<Account*> accounts;
vector<Transaction> transactions;

// ================= HELPERS =================
string generateAcc(string branch) {
    return "ACC-" + branch + "-" + to_string(rand() % 90000 + 10000);
}

string generatePIN() {
    return to_string(rand() % 90000 + 10000);
}

string timeNow() {
    time_t t = time(0);
    return ctime(&t);
}

// ================= SAVE DATA =================
void saveAccounts() {
    ofstream file("customers.dat", ios::binary);
    for (auto a : accounts) {
        file.write((char*)a, sizeof(*a));
    }
    file.close();
}

void saveTransactions() {
    ofstream file("transactions.dat", ios::app);
    for (auto &t : transactions) {
        file << t.accNumber << "," << t.type << "," << t.amount << "," << t.timeStamp;
    }
    file.close();
}

// ================= CREATE ACCOUNT =================
void createAccount() {
    Account* acc = new Account();

    cout << "Name: ";
    cin >> acc->name;

    cout << "Branch: ";
    cin >> acc->branch;

    acc->accNumber = generateAcc(acc->branch);
    acc->pin = encryptData(generatePIN());

    cout << "\nACCOUNT CREATED\n";
    cout << "Account Number: " << acc->accNumber << endl;
    cout << "PIN (SAVE): " << decryptData(acc->pin) << endl;

    accounts.push_back(acc);
    saveAccounts();
}

// ================= LOGIN =================
Account* login() {
    string acc, pin;
    int attempts = 3;

    while (attempts--) {
        cout << "Account: ";
        cin >> acc;
        cout << "PIN: ";
        cin >> pin;

        for (auto a : accounts) {
            if (a->accNumber == acc && decryptData(a->pin) == pin) {
                cout << "Login successful!\n";
                return a;
            }
        }

        cout << "Wrong details. Attempts left: " << attempts << endl;
    }

    cout << "LOCKED OUT!\n";
    return nullptr;
}

// ================= TRANSACTIONS =================
void doTransaction(Account* acc) {
    int choice;
    double amt;

    do {
        cout << "\n1. Deposit\n2. Withdraw\n3. View\n4. Exit\n";
        cin >> choice;

        if (choice == 1) {
            cout << "Amount: ";
            cin >> amt;
            acc->deposit(amt);

            transactions.push_back({acc->accNumber, "DEPOSIT", amt, timeNow()});
        }

        else if (choice == 2) {
            cout << "Amount: ";
            cin >> amt;

            if (acc->withdraw(amt)) {
                transactions.push_back({acc->accNumber, "WITHDRAW", amt, timeNow()});
            } else {
                cout << "Insufficient funds\n";
            }
        }

        else if (choice == 3) {
            acc->display();
        }

    } while (choice != 4);

    saveTransactions();
}

// ================= REPORT =================
void report() {
    cout << "\n===== TRANSACTION REPORT =====\n";
    for (auto &t : transactions) {
        cout << t.accNumber << " | " << t.type << " | R" << t.amount << " | " << t.timeStamp;
    }
}

// ================= MAIN =================
int main() {
    srand(time(0));

    int choice;

    do {
        cout << "\n===== BANK SYSTEM =====\n";
        cout << "1. Create Account\n";
        cout << "2. Login\n";
        cout << "3. Reports\n";
        cout << "4. Exit\n";
        cin >> choice;

        if (choice == 1) {
            createAccount();
        }

        else if (choice == 2) {
            Account* acc = login();
            if (acc) doTransaction(acc);
        }

        else if (choice == 3) {
            report();
        }

    } while (choice != 4);

    return 0;
}