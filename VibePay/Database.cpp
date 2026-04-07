#include "Database.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QRandomGenerator>

Database& Database::instance() {
    static Database instance;
    return instance;
}

Database::Database() {
}

Database::~Database() {
    if (db.isOpen()) {
        db.close();
    }
}

QString Database::hashData(const QString& data) {
    QByteArray hash = QCryptographicHash::hash(data.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool Database::init() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("vibepay_v2.db"); // Tạo file mớI để không xung đột cấu trúc cũ
    
    if (!db.open()) {
        qDebug() << "Lỗi: Không thể kết nối cơ sở dữ liệu";
        return false;
    }
    
    QSqlQuery query;
    // Bảng Users
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "username TEXT UNIQUE, "
               "password TEXT, "
               "pin_code TEXT)");
               
    // Bảng Wallets (Mỗi user có nhiều ví cho nhiều loại tiền)
    query.exec("CREATE TABLE IF NOT EXISTS wallets ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "user_id INTEGER, "
               "currency TEXT, "
               "balance REAL DEFAULT 0.0, "
               "UNIQUE(user_id, currency))");
               
    // Bảng Ngân hàng liên kết
    query.exec("CREATE TABLE IF NOT EXISTS linked_banks ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "user_id INTEGER, "
               "bank_name TEXT, "
               "account_number TEXT)");
               
    // Bảng Lịch sử giao dịch (cập nhật có currency)
    query.exec("CREATE TABLE IF NOT EXISTS transactions ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "from_user_id INTEGER, "
               "to_user_id INTEGER, "
               "currency TEXT, "
               "amount REAL, "
               "note TEXT, "
               "timestamp TEXT, "
               "status TEXT)");
               
    // Kiểm tra và seed 100 User nếu DB trống
    query.exec("SELECT COUNT(*) FROM users");
    if (query.next() && query.value(0).toInt() == 0) {
        seed100Users();
    }
    
    return true;
}

void Database::seed100Users() {
    qDebug() << "Tạo hệ thống 100 Users mẫu...";
    db.transaction();
    QSqlQuery query;
    QString defaultPassword = hashData("Password@123");
    QString defaultPin = hashData("123456");
    
    for (int i = 1; i <= 100; ++i) {
        QString username = QString("user%1").arg(i);
        
        query.prepare("INSERT INTO users (username, password, pin_code) VALUES (?, ?, ?)");
        query.addBindValue(username);
        query.addBindValue(defaultPassword);
        query.addBindValue(defaultPin);
        if (query.exec()) {
            int newUserId = query.lastInsertId().toInt();
            
            // Cấp phát số dư ảo: VND (1tr - 100tr) và BTC (0 - 1.5 BTC)
            double randVnd = QRandomGenerator::global()->bounded(1000000, 100000000);
            double randBtc = QRandomGenerator::global()->bounded(150) / 100.0; 
            
            QSqlQuery wQuery;
            wQuery.prepare("INSERT INTO wallets (user_id, currency, balance) VALUES (?, 'VND', ?)");
            wQuery.addBindValue(newUserId);
            wQuery.addBindValue(randVnd);
            wQuery.exec();
            
            wQuery.prepare("INSERT INTO wallets (user_id, currency, balance) VALUES (?, 'BTC', ?)");
            wQuery.addBindValue(newUserId);
            wQuery.addBindValue(randBtc);
            wQuery.exec();
        }
    }
    db.commit();
}

int Database::login(const QString& username, const QString& password) {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE username = ? AND password = ?");
    query.addBindValue(username);
    query.addBindValue(hashData(password));
    
    if (query.exec() && query.next()) {
        int userId = query.value(0).toInt();
        QSettings settings("VibePay", "App");
        settings.setValue("currentUserId", userId);
        return userId;
    }
    return -1;
}

bool Database::registerUser(const QString& username, const QString& password, const QString& pinCode) {
    db.transaction();
    
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password, pin_code) VALUES (?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(hashData(password));
    query.addBindValue(hashData(pinCode));
    
    if (query.exec()) {
        int newUserId = query.lastInsertId().toInt();
        // Khởi tạo ví 0 VND, 0 BTC cho user mới
        QSqlQuery wQuery;
        wQuery.exec(QString("INSERT INTO wallets (user_id, currency, balance) VALUES (%1, 'VND', 0.0)").arg(newUserId));
        wQuery.exec(QString("INSERT INTO wallets (user_id, currency, balance) VALUES (%1, 'BTC', 0.0)").arg(newUserId));
        
        db.commit();
        return true;
    }
    db.rollback();
    return false;
}

bool Database::verifyPin(int userId, const QString& pinCode) {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE id = ? AND pin_code = ?");
    query.addBindValue(userId);
    query.addBindValue(hashData(pinCode));
    return (query.exec() && query.next());
}

QString Database::getUsername(int userId) {
    QSqlQuery query;
    query.prepare("SELECT username FROM users WHERE id = ?");
    query.addBindValue(userId);
    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }
    return "";
}

int Database::getUserId(const QString& username) {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE username = ?");
    query.addBindValue(username);
    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

double Database::getBalance(int userId, const QString& currency) {
    QSqlQuery query;
    query.prepare("SELECT balance FROM wallets WHERE user_id = ? AND currency = ?");
    query.addBindValue(userId);
    query.addBindValue(currency);
    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }
    return 0.0;
}

QMap<QString, double> Database::getAllBalances(int userId) {
    QMap<QString, double> balances;
    QSqlQuery query;
    query.prepare("SELECT currency, balance FROM wallets WHERE user_id = ?");
    query.addBindValue(userId);
    if (query.exec()) {
        while (query.next()) {
            balances[query.value(0).toString()] = query.value(1).toDouble();
        }
    }
    return balances;
}

bool Database::linkBank(int userId, const QString& bankName, const QString& accountNumber) {
    QSqlQuery query;
    query.prepare("INSERT INTO linked_banks (user_id, bank_name, account_number) VALUES (?, ?, ?)");
    query.addBindValue(userId);
    query.addBindValue(bankName);
    query.addBindValue(accountNumber);
    return query.exec();
}

QList<LinkedBank> Database::getLinkedBanks(int userId) {
    QList<LinkedBank> banks;
    QSqlQuery query;
    query.prepare("SELECT id, bank_name, account_number FROM linked_banks WHERE user_id = ?");
    query.addBindValue(userId);
    if (query.exec()) {
        while (query.next()) {
            LinkedBank b;
            b.id = query.value(0).toInt();
            b.bankName = query.value(1).toString();
            b.accountNumber = query.value(2).toString();
            banks.append(b);
        }
    }
    return banks;
}

bool Database::fundFromBank(int userId, double amount) {
    if (amount <= 0) return false;
    
    QSqlQuery query;
    // Nạp tiền mô phỏng: Chỉ việc cộng số dư VND
    query.prepare("UPDATE wallets SET balance = balance + ? WHERE user_id = ? AND currency = 'VND'");
    query.addBindValue(amount);
    query.addBindValue(userId);
    if(query.exec()) {
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        QSqlQuery tQuery;
        tQuery.prepare("INSERT INTO transactions (from_user_id, to_user_id, currency, amount, note, timestamp, status) "
                      "VALUES (0, ?, 'VND', ?, 'Nạp tiền từ Ngân hàng', ?, 'Success')");
        tQuery.addBindValue(userId);
        tQuery.addBindValue(amount);
        tQuery.addBindValue(timestamp);
        tQuery.exec();
        return true;
    }
    return false;
}

bool Database::transfer(int fromUserId, int toUserId, const QString& currency, double amount, const QString& note) {
    if (amount <= 0) return false;
    db.transaction();
    
    double senderBalance = getBalance(fromUserId, currency);
    if (senderBalance < amount) {
        db.rollback();
        return false;
    }
    
    QSqlQuery query;
    // Trừ tiền
    query.prepare("UPDATE wallets SET balance = balance - ? WHERE user_id = ? AND currency = ?");
    query.addBindValue(amount);
    query.addBindValue(fromUserId);
    query.addBindValue(currency);
    if (!query.exec()) { db.rollback(); return false; }
    
    // Cộng tiền
    query.prepare("UPDATE wallets SET balance = balance + ? WHERE user_id = ? AND currency = ?");
    query.addBindValue(amount);
    query.addBindValue(toUserId);
    query.addBindValue(currency);
    if (!query.exec()) {
        // Tồn tại trường hợp người nhận chưa có ví loại này, tạo mới
        query.prepare("INSERT INTO wallets (user_id, currency, balance) VALUES (?, ?, ?)");
        query.addBindValue(toUserId);
        query.addBindValue(currency);
        query.addBindValue(amount);
        if(!query.exec()) { db.rollback(); return false; }
    }
    
    // Ghi sổ
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    query.prepare("INSERT INTO transactions (from_user_id, to_user_id, currency, amount, note, timestamp, status) "
                  "VALUES (?, ?, ?, ?, ?, ?, 'Success')");
    query.addBindValue(fromUserId);
    query.addBindValue(toUserId);
    query.addBindValue(currency);
    query.addBindValue(amount);
    query.addBindValue(note);
    query.addBindValue(timestamp);
    if (!query.exec()) { db.rollback(); return false; }
    
    db.commit();
    return true;
}

bool Database::swap(int userId, const QString& fromCurrency, const QString& toCurrency, double fromAmount, double toAmount) {
    db.transaction();
    // Simple verification
    double bal = getBalance(userId, fromCurrency);
    if (bal < fromAmount) {
        db.rollback();
        return false;
    }
    
    QSqlQuery query;
    query.prepare("UPDATE wallets SET balance = balance - ? WHERE user_id = ? AND currency = ?");
    query.addBindValue(fromAmount);
    query.addBindValue(userId);
    query.addBindValue(fromCurrency);
    if(!query.exec()) { db.rollback(); return false; }
    
    query.prepare("UPDATE wallets SET balance = balance + ? WHERE user_id = ? AND currency = ?");
    query.addBindValue(toAmount);
    query.addBindValue(userId);
    query.addBindValue(toCurrency);
    if(!query.exec()) {
        query.prepare("INSERT INTO wallets (user_id, currency, balance) VALUES (?, ?, ?)");
        query.addBindValue(userId);
        query.addBindValue(toCurrency);
        query.addBindValue(toAmount);
        if(!query.exec()) { db.rollback(); return false; }
    }
    
    db.commit();
    return true;
}

QList<Transaction> Database::getHistory(int userId) {
    QList<Transaction> history;
    QSqlQuery query;
    query.prepare("SELECT id, from_user_id, to_user_id, currency, amount, note, timestamp, status "
                  "FROM transactions WHERE from_user_id = ? OR to_user_id = ? ORDER BY timestamp DESC");
    query.addBindValue(userId);
    query.addBindValue(userId);
    
    if (query.exec()) {
        while (query.next()) {
            Transaction t;
            t.id = query.value(0).toInt();
            t.fromUserId = query.value(1).toInt();
            t.toUserId = query.value(2).toInt();
            t.currency = query.value(3).toString();
            t.amount = query.value(4).toDouble();
            t.note = query.value(5).toString();
            t.timestamp = query.value(6).toString();
            t.status = query.value(7).toString();
            history.append(t);
        }
    }
    return history;
}
