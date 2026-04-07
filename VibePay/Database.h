#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QSettings>
#include <QList>
#include <QMap>

struct Transaction {
    int id;
    int fromUserId;
    int toUserId;
    QString currency;
    double amount;
    QString note;
    QString timestamp;
    QString status;
};

struct LinkedBank {
    int id;
    QString bankName;
    QString accountNumber;
};

class Database {
public:
    static Database& instance();
    bool init();
    
    // Đăng nhập / Đăng ký
    int login(const QString& username, const QString& password);
    bool registerUser(const QString& username, const QString& password, const QString& pinCode);
    bool verifyPin(int userId, const QString& pinCode);
    
    // Thông tin cơ bản
    QString getUsername(int userId);
    int getUserId(const QString& username);
    
    // Ví và Số dư (Đa tiền tệ: VND, BTC...)
    double getBalance(int userId, const QString& currency);
    QMap<QString, double> getAllBalances(int userId);
    
    // Ngân hàng Liên kết (Funding)
    bool linkBank(int userId, const QString& bankName, const QString& accountNumber);
    QList<LinkedBank> getLinkedBanks(int userId);
    bool fundFromBank(int userId, double amount); // Nạp VND từ ngân hàng giả lập
    
    // Giao dịch và Quy đổi
    // transfer trả về true nếu thành công
    bool transfer(int fromUserId, int toUserId, const QString& currency, double amount, const QString& note);
    bool swap(int userId, const QString& fromCurrency, const QString& toCurrency, double fromAmount, double toAmount);
    
    QList<Transaction> getHistory(int userId);

private:
    Database();
    ~Database();
    QString hashData(const QString& data);
    void seed100Users();
    
    QSqlDatabase db;
};

#endif // DATABASE_H
