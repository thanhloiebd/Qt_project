#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setMinimumSize(1024, 768);
    resize(1200, 800);
    setWindowTitle("VibePay - Nền tảng Tài chính Đa tiền tệ");
    setupUi();
    applyStyle();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    rootStackedWidget = new QStackedWidget(this);
    setCentralWidget(rootStackedWidget);

    // 1. Setup phần Authentication
    authStackedWidget = new QStackedWidget(this);
    loginPage = new LoginPage(this);
    registerPage = new RegisterPage(this);
    authStackedWidget->addWidget(loginPage);
    authStackedWidget->addWidget(registerPage);

    // 2. Setup phần App Container
    appContainer = new QWidget(this);
    auto appLayout = new QHBoxLayout(appContainer);
    appLayout->setContentsMargins(0, 0, 0, 0);
    appLayout->setSpacing(0);

    sidebar = createSidebar();
    
    appStackedWidget = new QStackedWidget(this);
    dashboardPage = new DashboardPage(this);
    walletsPage = new WalletsPage(this);
    transferPage = new TransferPage(this);
    swapPage = new SwapPage(this);
    historyPage = new HistoryPage(this);
    
    appStackedWidget->addWidget(dashboardPage);
    appStackedWidget->addWidget(walletsPage);
    appStackedWidget->addWidget(transferPage);
    appStackedWidget->addWidget(swapPage);
    appStackedWidget->addWidget(historyPage);

    appLayout->addWidget(sidebar);
    appLayout->addWidget(appStackedWidget);

    // 3. Đưa vào root
    rootStackedWidget->addWidget(authStackedWidget);
    rootStackedWidget->addWidget(appContainer);

    // Các Connects (Auth)
    connect(loginPage, &LoginPage::loginSuccess, this, &MainWindow::onLoginSuccess);
    connect(loginPage, &LoginPage::goRegister, this, &MainWindow::onGoRegister);
    connect(registerPage, &RegisterPage::goLogin, this, &MainWindow::onGoLogin);
}

QWidget* MainWindow::createSidebar() {
    QWidget* sb = new QWidget(this);
    sb->setObjectName("sidebar");
    sb->setFixedWidth(250);
    
    auto layout = new QVBoxLayout(sb);
    layout->setContentsMargins(10, 30, 10, 30);
    layout->setSpacing(15);
    
    auto logo = new QLabel("VibePay", sb);
    logo->setObjectName("logoLabel");
    logo->setAlignment(Qt::AlignCenter);
    layout->addWidget(logo);
    layout->addSpacing(30);

    auto btnDash = new QPushButton("Trang chủ", sb);
    auto btnWallet = new QPushButton("Ví của tôi", sb);
    auto btnTransfer = new QPushButton("Chuyển tiền", sb);
    auto btnSwap = new QPushButton("Quy đổi (Swap)", sb);
    auto btnHistory = new QPushButton("Lịch sử", sb);
    
    btnDash->setObjectName("sidebarButton");
    btnWallet->setObjectName("sidebarButton");
    btnTransfer->setObjectName("sidebarButton");
    btnSwap->setObjectName("sidebarButton");
    btnHistory->setObjectName("sidebarButton");

    layout->addWidget(btnDash);
    layout->addWidget(btnWallet);
    layout->addWidget(btnTransfer);
    layout->addWidget(btnSwap);
    layout->addWidget(btnHistory);
    layout->addStretch();
    
    auto btnOut = new QPushButton("Đăng xuất", sb);
    btnOut->setStyleSheet("QPushButton { background-color: transparent; color: #ff5252; border: 1px solid #ff5252; } QPushButton:hover { background-color: #ff5252; color: white; }");
    layout->addWidget(btnOut);

    connect(btnDash, &QPushButton::clicked, this, &MainWindow::showDashboard);
    connect(btnWallet, &QPushButton::clicked, this, &MainWindow::showWallets);
    connect(btnTransfer, &QPushButton::clicked, this, &MainWindow::showTransfer);
    connect(btnSwap, &QPushButton::clicked, this, &MainWindow::showSwap);
    connect(btnHistory, &QPushButton::clicked, this, &MainWindow::showHistory);
    connect(btnOut, &QPushButton::clicked, this, &MainWindow::performLogout);

    return sb;
}

// --- Navigation Logic ---

void MainWindow::onLoginSuccess() {
    dashboardPage->refresh();
    rootStackedWidget->setCurrentWidget(appContainer);
    appStackedWidget->setCurrentWidget(dashboardPage);
}

void MainWindow::onGoRegister() {
    authStackedWidget->setCurrentWidget(registerPage);
}

void MainWindow::onGoLogin() {
    authStackedWidget->setCurrentWidget(loginPage);
}

void MainWindow::performLogout() {
    rootStackedWidget->setCurrentWidget(authStackedWidget);
    authStackedWidget->setCurrentWidget(loginPage);
}

void MainWindow::showDashboard() {
    dashboardPage->refresh();
    appStackedWidget->setCurrentWidget(dashboardPage);
}

void MainWindow::showWallets() {
    walletsPage->refresh();
    appStackedWidget->setCurrentWidget(walletsPage);
}

void MainWindow::showTransfer() {
    transferPage->refresh();
    appStackedWidget->setCurrentWidget(transferPage);
}

void MainWindow::showSwap() {
    swapPage->refresh();
    appStackedWidget->setCurrentWidget(swapPage);
}

void MainWindow::showHistory() {
    historyPage->refresh();
    appStackedWidget->setCurrentWidget(historyPage);
}

// --- STYLING ---
void MainWindow::applyStyle() {
    QString qss = R"(
        QMainWindow {
            background-color: #12121a;
        }
        QWidget {
            color: #ffffff;
            font-family: 'Segoe UI', Arial, sans-serif;
            font-size: 11pt;
        }
        QWidget#sidebar {
            background-color: #1a1a24;
            border-right: 1px solid #2d2d44;
        }
        QLabel#logoLabel {
            font-size: 28pt;
            font-weight: bold;
            color: #69f0ae;
        }
        QLabel#titleLabel {
            font-size: 24pt;
            font-weight: bold;
            margin-bottom: 20px;
        }
        QLabel#headerLabel {
            font-size: 16pt;
            font-weight: bold;
            margin-bottom: 10px;
        }
        QLabel#balanceLabel {
            font-size: 24pt;
            font-weight: bold;
            color: #69f0ae;
        }
        QWidget#cardWidget {
            background-color: #212130;
            border-radius: 12px;
            padding: 20px;
            border: 1px solid #2d2d44;
        }
        QLineEdit, QComboBox {
            background-color: #2b2b40;
            border: 1px solid #3d3d5c;
            border-radius: 8px;
            padding: 12px;
            color: white;
            margin-bottom: 10px;
        }
        QLineEdit:focus, QComboBox:focus {
            border: 1px solid #69f0ae;
        }
        QPushButton {
            background-color: #3d3d5c;
            color: white;
            border-radius: 8px;
            padding: 12px;
            font-weight: bold;
            margin-top: 5px;
        }
        QPushButton:hover {
            background-color: #4d4d73;
        }
        QPushButton#accentButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #00c6ff, stop:1 #0072ff);
            border: none;
        }
        QPushButton#accentButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #0072ff, stop:1 #00c6ff);
        }
        QPushButton#sidebarButton {
            background-color: transparent;
            text-align: left;
            padding-left: 20px;
            border-radius: 0px;
            font-size: 12pt;
        }
        QPushButton#sidebarButton:hover {
            background-color: #2b2b40;
            border-left: 4px solid #69f0ae;
        }
        QTableView {
            background-color: #212130;
            alternate-background-color: #272738;
            border: 1px solid #2d2d44;
            border-radius: 8px;
            color: white;
            gridline-color: #3d3d5c;
            selection-background-color: #4d4d73;
        }
        QHeaderView::section {
            background-color: #1a1a24;
            color: #8c8cba;
            padding: 10px;
            border: none;
            border-bottom: 1px solid #2d2d44;
            font-weight: bold;
        }
        QProgressBar {
            background-color: #2b2b40;
            border-radius: 2px;
            text-align: center;
        }
    )";
    qApp->setStyleSheet(qss);
}
