#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "Pages.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void switchPage(QWidget* targetPage);

private:
    void setupUi();
    void applyStyle();

    QStackedWidget* stackedWidget;
    LoginPage* loginPage;
    RegisterPage* registerPage;
    DashboardPage* dashboardPage;
    PaymentPage* paymentPage;
    HistoryPage* historyPage;
};

#endif // MAINWINDOW_H
