#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QSettings>
#include <QList>

struct Transaction {
    int id;
    int fromUserId;
    int toUserId;
    double amount;
    QString note;
    QString timestamp;
    QString status;
};

class Database {
public:
    static Database& instance();
    bool init();
    
    // Authentication
    int login(const QString& username, const QString& password);
    bool registerUser(const QString& username, const QString& password);
    
    // User Info
    double getBalance(int userId);
    QString getUsername(int userId);
    int getUserId(const QString& username);
    
    // Transactions
    bool transfer(int fromUserId, int toUserId, double amount, const QString& note);
    QList<Transaction> getHistory(int userId);

private:
    Database();
    ~Database();
    QString hashPassword(const QString& password);
    
    QSqlDatabase db;
};

#endif // DATABASE_H
