#include "Database.h"
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>

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

QString Database::hashPassword(const QString& password) {
    QByteArray hash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
}

bool Database::init() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("vibepay.db");
    
    if (!db.open()) {
        qDebug() << "Lỗi: Không thể kết nối cơ sở dữ liệu";
        return false;
    }
    
    QSqlQuery query;
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "username TEXT UNIQUE, "
               "password TEXT, "
               "balance REAL DEFAULT 0.0)");
               
    query.exec("CREATE TABLE IF NOT EXISTS transactions ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "from_user_id INTEGER, "
               "to_user_id INTEGER, "
               "amount REAL, "
               "note TEXT, "
               "timestamp TEXT, "
               "status TEXT)");
               
    // Tạo sẵn user1 nếu chưa có
    query.prepare("SELECT id FROM users WHERE username = 'user1'");
    query.exec();
    if (!query.next()) {
        query.prepare("INSERT INTO users (username, password, balance) VALUES (?, ?, ?)");
        query.addBindValue("user1");
        query.addBindValue(hashPassword("pass1"));
        query.addBindValue(500000.0);
        query.exec();
    }
    
    // Tạo sẵn user2 nếu chưa có
    query.prepare("SELECT id FROM users WHERE username = 'user2'");
    query.exec();
    if (!query.next()) {
        query.prepare("INSERT INTO users (username, password, balance) VALUES (?, ?, ?)");
        query.addBindValue("user2");
        query.addBindValue(hashPassword("pass2"));
        query.addBindValue(200000.0);
        query.exec();
    }
    
    return true;
}

int Database::login(const QString& username, const QString& password) {
    QSqlQuery query;
    query.prepare("SELECT id FROM users WHERE username = ? AND password = ?");
    query.addBindValue(username);
    query.addBindValue(hashPassword(password));
    
    if (query.exec() && query.next()) {
        int userId = query.value(0).toInt();
        QSettings settings("VibePay", "App");
        settings.setValue("currentUserId", userId);
        return userId;
    }
    return -1;
}

bool Database::registerUser(const QString& username, const QString& password) {
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password, balance) VALUES (?, ?, 0.0)");
    query.addBindValue(username);
    query.addBindValue(hashPassword(password));
    return query.exec();
}

double Database::getBalance(int userId) {
    QSqlQuery query;
    query.prepare("SELECT balance FROM users WHERE id = ?");
    query.addBindValue(userId);
    if (query.exec() && query.next()) {
        return query.value(0).toDouble();
    }
    return 0.0;
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

bool Database::transfer(int fromUserId, int toUserId, double amount, const QString& note) {
    db.transaction();
    
    double senderBalance = getBalance(fromUserId);
    if (senderBalance < amount) {
        db.rollback();
        return false;
    }
    
    QSqlQuery query;
    // Trừ tiền người gửi
    query.prepare("UPDATE users SET balance = balance - ? WHERE id = ?");
    query.addBindValue(amount);
    query.addBindValue(fromUserId);
    if (!query.exec()) { db.rollback(); return false; }
    
    // Cộng tiền người nhận
    query.prepare("UPDATE users SET balance = balance + ? WHERE id = ?");
    query.addBindValue(amount);
    query.addBindValue(toUserId);
    if (!query.exec()) { db.rollback(); return false; }
    
    // Ghi lại giao dịch
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    query.prepare("INSERT INTO transactions (from_user_id, to_user_id, amount, note, timestamp, status) "
                  "VALUES (?, ?, ?, ?, ?, 'Success')");
    query.addBindValue(fromUserId);
    query.addBindValue(toUserId);
    query.addBindValue(amount);
    query.addBindValue(note);
    query.addBindValue(timestamp);
    if (!query.exec()) { db.rollback(); return false; }
    
    db.commit();
    return true;
}

QList<Transaction> Database::getHistory(int userId) {
    QList<Transaction> history;
    QSqlQuery query;
    query.prepare("SELECT id, from_user_id, to_user_id, amount, note, timestamp, status "
                  "FROM transactions WHERE from_user_id = ? OR to_user_id = ? ORDER BY timestamp DESC");
    query.addBindValue(userId);
    query.addBindValue(userId);
    
    if (query.exec()) {
        while (query.next()) {
            Transaction t;
            t.id = query.value(0).toInt();
            t.fromUserId = query.value(1).toInt();
            t.toUserId = query.value(2).toInt();
            t.amount = query.value(3).toDouble();
            t.note = query.value(4).toString();
            t.timestamp = query.value(5).toString();
            t.status = query.value(6).toString();
            history.append(t);
        }
    }
    return history;
}
