#ifndef PAGES_H
#define PAGES_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStandardItemModel>
#include <QComboBox>
#include <QProgressBar>

// --- Authentication Pages ---

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
    void onPasswordChanged(const QString& pwd);
private:
    QLineEdit* userEdit;
    QLineEdit* passEdit;
    QLineEdit* confirmEdit;
    QLineEdit* pinEdit;
    QProgressBar* strengthBar;
    QLabel* strengthLabel;
};

// --- App Pages ---

class DashboardPage : public QWidget {
    Q_OBJECT
public:
    explicit DashboardPage(QWidget* parent = nullptr);
    void refresh();
signals:
    // none needed for dashboard itself if navigation is in sidebar
private:
    QLabel* greetingLabel;
    QLabel* netWorthLabel;
};

class WalletsPage : public QWidget {
    Q_OBJECT
public:
    explicit WalletsPage(QWidget* parent = nullptr);
    void refresh();
private slots:
    void onFundClicked();
private:
    QLabel* vndLabel;
    QLabel* btcLabel;
    QLineEdit* fundAmountEdit;
};

class TransferPage : public QWidget {
    Q_OBJECT
public:
    explicit TransferPage(QWidget* parent = nullptr);
    void refresh();
private slots:
    void onTransferClicked();
private:
    QComboBox* currencyCombo;
    QLineEdit* recipientEdit;
    QLineEdit* amountEdit;
    QLineEdit* noteEdit;
};

class SwapPage : public QWidget {
    Q_OBJECT
public:
    explicit SwapPage(QWidget* parent = nullptr);
    void refresh();
private slots:
    void onSwapClicked();
    void onAmountChanged(const QString& text);
private:
    QLineEdit* fromAmountEdit;
    QLabel* toAmountLabel;
    double currentRate; // VND per 1 BTC
};

class HistoryPage : public QWidget {
    Q_OBJECT
public:
    explicit HistoryPage(QWidget* parent = nullptr);
    void refresh();
private:
    QTableView* tableView;
    QStandardItemModel* model;
};

#endif // PAGES_H
