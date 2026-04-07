#include "Pages.h"
#include "Database.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QInputDialog>

// --- LoginPage ---
LoginPage::LoginPage(QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    auto title = new QLabel("VibePay", this);
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);
    
    userEdit = new QLineEdit(this);
    userEdit->setPlaceholderText("Tên đăng nhập");
    
    passEdit = new QLineEdit(this);
    passEdit->setPlaceholderText("Mật khẩu");
    passEdit->setEchoMode(QLineEdit::Password);
    
    auto loginBtn = new QPushButton("Đăng nhập", this);
    loginBtn->setObjectName("accentButton");
    
    auto registerBtn = new QPushButton("Đăng ký", this);
    
    layout->addWidget(title);
    layout->addWidget(userEdit);
    layout->addWidget(passEdit);
    layout->addWidget(loginBtn);
    layout->addWidget(registerBtn);
    
    connect(loginBtn, &QPushButton::clicked, this, &LoginPage::onLoginClicked);
    connect(registerBtn, &QPushButton::clicked, this, &LoginPage::goRegister);
}

void LoginPage::onLoginClicked() {
    int userId = Database::instance().login(userEdit->text(), passEdit->text());
    if (userId != -1) {
        userEdit->clear();
        passEdit->clear();
        emit loginSuccess();
    } else {
        QMessageBox::warning(this, "Lỗi", "Sai tên đăng nhập hoặc mật khẩu!");
    }
}

// --- RegisterPage ---
RegisterPage::RegisterPage(QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    auto title = new QLabel("Tạo tài khoản", this);
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);
    
    userEdit = new QLineEdit(this);
    userEdit->setPlaceholderText("Tên đăng nhập");
    
    passEdit = new QLineEdit(this);
    passEdit->setPlaceholderText("Mật khẩu");
    passEdit->setEchoMode(QLineEdit::Password);
    
    confirmEdit = new QLineEdit(this);
    confirmEdit->setPlaceholderText("Xác nhận mật khẩu");
    confirmEdit->setEchoMode(QLineEdit::Password);
    
    auto registerBtn = new QPushButton("Tạo tài khoản", this);
    registerBtn->setObjectName("accentButton");
    auto backBtn = new QPushButton("Quay lại đăng nhập", this);
    
    layout->addWidget(title);
    layout->addWidget(userEdit);
    layout->addWidget(passEdit);
    layout->addWidget(confirmEdit);
    layout->addWidget(registerBtn);
    layout->addWidget(backBtn);
    
    connect(registerBtn, &QPushButton::clicked, this, &RegisterPage::onRegisterClicked);
    connect(backBtn, &QPushButton::clicked, this, &RegisterPage::goLogin);
}

void RegisterPage::onRegisterClicked() {
    if (userEdit->text().isEmpty() || passEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng điền đủ thông tin!");
        return;
    }
    if (passEdit->text() != confirmEdit->text()) {
        QMessageBox::warning(this, "Lỗi", "Mật khẩu không khớp!");
        return;
    }
    
    if (Database::instance().registerUser(userEdit->text(), passEdit->text())) {
        QMessageBox::information(this, "Thành công", "Đăng ký thành công!");
        userEdit->clear();
        passEdit->clear();
        confirmEdit->clear();
        emit goLogin();
    } else {
        QMessageBox::warning(this, "Lỗi", "Tên đăng nhập có thể đã tồn tại!");
    }
}

// --- DashboardPage ---
DashboardPage::DashboardPage(QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    layout->setContentsMargins(20, 40, 20, 20);

    userLabel = new QLabel(this);
    userLabel->setObjectName("headerLabel");
    
    auto cardWidget = new QWidget(this);
    cardWidget->setObjectName("cardWidget");
    auto cardLayout = new QVBoxLayout(cardWidget);
    
    auto balanceTitle = new QLabel("Số dư của bạn", cardWidget);
    balanceLabel = new QLabel(cardWidget);
    balanceLabel->setObjectName("balanceLabel");
    
    cardLayout->addWidget(balanceTitle);
    cardLayout->addWidget(balanceLabel);
    
    auto transferBtn = new QPushButton("Chuyển tiền", this);
    transferBtn->setObjectName("accentButton");
    auto historyBtn = new QPushButton("Lịch sử", this);
    auto logoutBtn = new QPushButton("Đăng xuất", this);
    logoutBtn->setStyleSheet("background-color: #ff5252; color: white;");
    
    layout->addWidget(userLabel);
    layout->addSpacing(20);
    layout->addWidget(cardWidget);
    layout->addSpacing(30);
    layout->addWidget(transferBtn);
    layout->addWidget(historyBtn);
    layout->addStretch();
    layout->addWidget(logoutBtn);
    
    connect(transferBtn, &QPushButton::clicked, this, &DashboardPage::goTransfer);
    connect(historyBtn, &QPushButton::clicked, this, &DashboardPage::goHistory);
    connect(logoutBtn, &QPushButton::clicked, this, &DashboardPage::logout);
}

void DashboardPage::refresh() {
    QSettings settings("VibePay", "App");
    int userId = settings.value("currentUserId", -1).toInt();
    if (userId != -1) {
        userLabel->setText("Xin chào, " + Database::instance().getUsername(userId) + "!");
        balanceLabel->setText(QString::number(Database::instance().getBalance(userId), 'f', 0) + " VND");
    }
}

// --- PaymentPage ---
PaymentPage::PaymentPage(QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    auto title = new QLabel("Chuyển tiền", this);
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);
    
    recipientEdit = new QLineEdit(this);
    recipientEdit->setPlaceholderText("Tên người nhận");
    
    amountEdit = new QLineEdit(this);
    amountEdit->setPlaceholderText("Số tiền");
    
    noteEdit = new QLineEdit(this);
    noteEdit->setPlaceholderText("Ghi chú");
    
    auto transferBtn = new QPushButton("Xác nhận", this);
    transferBtn->setObjectName("accentButton");
    auto backBtn = new QPushButton("Hủy", this);
    
    layout->addWidget(title);
    layout->addWidget(recipientEdit);
    layout->addWidget(amountEdit);
    layout->addWidget(noteEdit);
    layout->addWidget(transferBtn);
    layout->addWidget(backBtn);
    
    connect(transferBtn, &QPushButton::clicked, this, &PaymentPage::onTransferClicked);
    connect(backBtn, &QPushButton::clicked, this, &PaymentPage::goDashboard);
}

void PaymentPage::onTransferClicked() {
    QString recipientName = recipientEdit->text();
    double amount = amountEdit->text().toDouble();
    QString note = noteEdit->text();
    
    if (recipientName.isEmpty() || amount <= 0) {
        QMessageBox::warning(this, "Lỗi", "Vui lòng nhập thông tin hợp lệ!");
        return;
    }
    
    int toUserId = Database::instance().getUserId(recipientName);
    if (toUserId == -1) {
        QMessageBox::warning(this, "Lỗi", "Người nhận không tồn tại!");
        return;
    }
    
    QSettings settings("VibePay", "App");
    int currentUserId = settings.value("currentUserId", -1).toInt();
    if (currentUserId == toUserId) {
        QMessageBox::warning(this, "Lỗi", "Không thể chuyển tiền cho chính mình!");
        return;
    }
    
    // Yêu cầu OTP giả
    bool ok;
    QString text = QInputDialog::getText(this, "Xác thực OTP",
                                         "Mã OTP (Nhập 123456):", QLineEdit::Normal,
                                         "", &ok);
    if (!ok || text != "123456") {
        QMessageBox::warning(this, "Lỗi", "Mã OTP không đúng!");
        return;
    }
    
    if (Database::instance().transfer(currentUserId, toUserId, amount, note)) {
        QMessageBox::information(this, "Thành công", "Chuyển tiền thành công!");
        recipientEdit->clear();
        amountEdit->clear();
        noteEdit->clear();
        emit goDashboard();
    } else {
        QMessageBox::warning(this, "Lỗi", "Giao dịch thất bại! Số dư không đủ.");
    }
}

// --- HistoryPage ---
HistoryPage::HistoryPage(QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    
    auto title = new QLabel("Lịch sử giao dịch", this);
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);
    
    tableView = new QTableView(this);
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"Thời gian", "Số tiền (VND)", "Người nhận/Gửi", "Trạng thái"});
    tableView->setModel(model);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    auto backBtn = new QPushButton("Trở về", this);
    
    layout->addWidget(title);
    layout->addWidget(tableView);
    layout->addWidget(backBtn);
    
    connect(backBtn, &QPushButton::clicked, this, &HistoryPage::goDashboard);
}

void HistoryPage::loadHistory() {
    model->removeRows(0, model->rowCount());
    QSettings settings("VibePay", "App");
    int currentUserId = settings.value("currentUserId", -1).toInt();
    
    auto history = Database::instance().getHistory(currentUserId);
    int row = 0;
    for (const auto& t : history) {
        QString otherUser = (t.fromUserId == currentUserId) ? "Đến: " + Database::instance().getUsername(t.toUserId) 
                                                            : "Từ: " + Database::instance().getUsername(t.fromUserId);
        QString sign = (t.fromUserId == currentUserId) ? "-" : "+";
        
        QStandardItem* timeItem = new QStandardItem(t.timestamp);
        QStandardItem* amountItem = new QStandardItem(sign + QString::number(t.amount, 'f', 0));
        QStandardItem* userItem = new QStandardItem(otherUser + " (" + t.note + ")");
        QStandardItem* statusItem = new QStandardItem(t.status);
        
        if (t.fromUserId == currentUserId) {
            amountItem->setForeground(QColor("#ff5252")); // Màu đỏ cho tiền đi
        } else {
            amountItem->setForeground(QColor("#69f0ae")); // Màu xanh cho tiền đến
        }
        
        model->insertRow(row);
        model->setItem(row, 0, timeItem);
        model->setItem(row, 1, amountItem);
        model->setItem(row, 2, userItem);
        model->setItem(row, 3, statusItem);
        row++;
    }
}
