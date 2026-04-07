#include "MainWindow.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setFixedSize(400, 600); // Tỉ lệ giống điện thoại
    setWindowTitle("VibePay");
    setupUi();
    applyStyle();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    loginPage = new LoginPage(this);
    registerPage = new RegisterPage(this);
    dashboardPage = new DashboardPage(this);
    paymentPage = new PaymentPage(this);
    historyPage = new HistoryPage(this);

    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(registerPage);
    stackedWidget->addWidget(dashboardPage);
    stackedWidget->addWidget(paymentPage);
    stackedWidget->addWidget(historyPage);

    // Kêt nối các trang
    // Đăng nhập -> Dashboard hoặc Đăng ký
    connect(loginPage, &LoginPage::loginSuccess, [this]() {
        dashboardPage->refresh();
        switchPage(dashboardPage);
    });
    connect(loginPage, &LoginPage::goRegister, [this]() {
        switchPage(registerPage);
    });

    // Đăng ký -> Đăng nhập
    connect(registerPage, &RegisterPage::goLogin, [this]() {
        switchPage(loginPage);
    });

    // Dashboard -> Chuyển tiền, Lịch sử, Đăng xuất
    connect(dashboardPage, &DashboardPage::goTransfer, [this]() {
        switchPage(paymentPage);
    });
    connect(dashboardPage, &DashboardPage::goHistory, [this]() {
        historyPage->loadHistory();
        switchPage(historyPage);
    });
    connect(dashboardPage, &DashboardPage::logout, [this]() {
        switchPage(loginPage);
    });

    // Chuyển tiền -> Dashboard
    connect(paymentPage, &PaymentPage::goDashboard, [this]() {
        dashboardPage->refresh();
        switchPage(dashboardPage);
    });

    // Lịch sử -> Dashboard
    connect(historyPage, &HistoryPage::goDashboard, [this]() {
        dashboardPage->refresh();
        switchPage(dashboardPage);
    });
}

void MainWindow::switchPage(QWidget* targetPage) {
    if (stackedWidget->currentWidget() == targetPage) return;

    stackedWidget->setCurrentWidget(targetPage);
    
    // Hiệu ứng fade in dùng QPropertyAnimation và QGraphicsOpacityEffect
    QGraphicsOpacityEffect *inEffect = new QGraphicsOpacityEffect(targetPage);
    targetPage->setGraphicsEffect(inEffect);
    QPropertyAnimation *inAnimation = new QPropertyAnimation(inEffect, "opacity");
    inAnimation->setDuration(300);
    inAnimation->setStartValue(0.0);
    inAnimation->setEndValue(1.0);
    
    connect(inAnimation, &QPropertyAnimation::finished, [targetPage]() {
        targetPage->setGraphicsEffect(nullptr);
    });

    inAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::applyStyle() {
    QString qss = R"(
        QMainWindow {
            background-color: #1e1e2f;
        }
        QWidget {
            color: white;
            font-family: 'Segoe UI';
            font-size: 10pt;
        }
        QLabel#titleLabel {
            font-size: 24pt;
            font-weight: bold;
            color: #ffffff;
            margin-bottom: 20px;
        }
        QLabel#headerLabel {
            font-size: 16pt;
            font-weight: bold;
        }
        QLabel#balanceLabel {
            font-size: 28pt;
            font-weight: bold;
            color: #69f0ae;
        }
        QWidget#cardWidget {
            background-color: #2d2d44;
            border-radius: 12px;
            padding: 20px;
        }
        QLineEdit {
            background-color: #2d2d44;
            border: 1px solid #3d3d5c;
            border-radius: 8px;
            padding: 10px;
            color: white;
            margin-bottom: 10px;
        }
        QLineEdit:focus {
            border: 1px solid #69f0ae;
        }
        QPushButton {
            background-color: #3d3d5c;
            color: white;
            border-radius: 12px;
            padding: 12px;
            font-weight: bold;
            margin-top: 5px;
        }
        QPushButton:hover {
            background-color: #4d4d73;
        }
        QPushButton#accentButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #00c6ff, stop:1 #0072ff);
        }
        QPushButton#accentButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0072ff, stop:1 #00c6ff);
        }
        QTableView {
            background-color: #2d2d44;
            alternate-background-color: #31314a;
            border: none;
            border-radius: 12px;
            color: white;
            gridline-color: #3d3d5c;
            selection-background-color: #4d4d73;
        }
        QHeaderView::section {
            background-color: #1e1e2f;
            color: #8c8cba;
            padding: 8px;
            border: none;
            font-weight: bold;
        }
    )";
    qApp->setStyleSheet(qss);
}
