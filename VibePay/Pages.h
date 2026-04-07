#ifndef PAGES_H
#define PAGES_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>
#include <QStandardItemModel>

class LoginPage : public QWidget {
    Q_OBJECT
public:
    explicit LoginPage(QWidget* parent = nullptr);
signals:
    void loginSuccess();
    void goRegister();
private slots:
    void onLoginClicked();
private:
    QLineEdit* userEdit;
    QLineEdit* passEdit;
};

class RegisterPage : public QWidget {
    Q_OBJECT
public:
    explicit RegisterPage(QWidget* parent = nullptr);
signals:
    void registerSuccess();
    void goLogin();
private slots:
    void onRegisterClicked();
private:
    QLineEdit* userEdit;
    QLineEdit* passEdit;
    QLineEdit* confirmEdit;
};

class DashboardPage : public QWidget {
    Q_OBJECT
public:
    explicit DashboardPage(QWidget* parent = nullptr);
    void refresh();
signals:
    void goTransfer();
    void goHistory();
    void logout();
private:
    QLabel* balanceLabel;
    QLabel* userLabel;
};

class PaymentPage : public QWidget {
    Q_OBJECT
public:
    explicit PaymentPage(QWidget* parent = nullptr);
signals:
    void goDashboard();
private slots:
    void onTransferClicked();
private:
    QLineEdit* amountEdit;
    QLineEdit* recipientEdit;
    QLineEdit* noteEdit;
};

class HistoryPage : public QWidget {
    Q_OBJECT
public:
    explicit HistoryPage(QWidget* parent = nullptr);
    void loadHistory();
signals:
    void goDashboard();
private:
    QTableView* tableView;
    QStandardItemModel* model;
};

#endif // PAGES_H
