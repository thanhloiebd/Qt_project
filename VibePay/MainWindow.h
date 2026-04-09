#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include "pages/LoginPage.h"
#include "pages/RegisterPage.h"
#include "pages/DashboardPage.h"
#include "pages/WalletsPage.h"
#include "pages/TransferPage.h"
#include "pages/SwapPage.h"
#include "pages/HistoryPage.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void performLogout();
    void showDashboard();
    void showWallets();
    void showTransfer();
    void showSwap();
    void showHistory();

    void onLoginSuccess();
    void onGoRegister();
    void onGoLogin();

private:
    void setupUi();
    void applyStyle();
    
    QWidget* createSidebar();

    QStackedWidget* rootStackedWidget; // Lớp thứ nhất: 0 = Đăng nhập, 1 = Giao diện App
    
    // Giao diện Authentication
    QStackedWidget* authStackedWidget;
    LoginPage* loginPage;
    RegisterPage* registerPage;

    // Giao diện App
    QWidget* appContainer;
    QWidget* sidebar;
    QStackedWidget* appStackedWidget; // Lớp thứ hai: Các module trong app
    
    DashboardPage* dashboardPage;
    WalletsPage* walletsPage;
    TransferPage* transferPage;
    SwapPage* swapPage;
    HistoryPage* historyPage;
};

#endif // MAINWINDOW_H
