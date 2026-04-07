#include "Pages.h"
#include "Database.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QInputDialog>
#include <QRegularExpression>

// --- LoginPage ---
LoginPage::LoginPage(QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);

    auto box = new QWidget(this);
    box->setFixedWidth(400);
    auto boxLayout = new QVBoxLayout(box);

    auto title = new QLabel("VibePay", box);
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);
    
    userEdit = new QLineEdit(box);
    userEdit->setPlaceholderText("Tên đăng nhập");
    
    passEdit = new QLineEdit(box);
    passEdit->setPlaceholderText("Mật khẩu");
    passEdit->setEchoMode(QLineEdit::Password);
    
    auto loginBtn = new QPushButton("Đăng nhập", box);
    loginBtn->setObjectName("accentButton");
    auto registerBtn = new QPushButton("Chưa có tài khoản? Đăng ký", box);
    
    boxLayout->addWidget(title);
    boxLayout->addWidget(userEdit);
    boxLayout->addWidget(passEdit);
    boxLayout->addWidget(loginBtn);
    boxLayout->addWidget(registerBtn);
    
    layout->addWidget(box);
    
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

    auto box = new QWidget(this);
    box->setFixedWidth(400);
    auto boxLayout = new QVBoxLayout(box);

    auto title = new QLabel("Đăng ký tài khoản", box);
    title->setObjectName("titleLabel");
    title->setAlignment(Qt::AlignCenter);
    
    userEdit = new QLineEdit(box);
    userEdit->setPlaceholderText("Tên đăng nhập");
    
    passEdit = new QLineEdit(box);
    passEdit->setPlaceholderText("Mật khẩu");
    passEdit->setEchoMode(QLineEdit::Password);
    
    strengthBar = new QProgressBar(box);
    strengthBar->setTextVisible(false);
    strengthBar->setFixedHeight(5);
    strengthLabel = new QLabel("Chưa nhập", box);
    
    confirmEdit = new QLineEdit(box);
    confirmEdit->setPlaceholderText("Xác nhận mật khẩu");
    confirmEdit->setEchoMode(QLineEdit::Password);
    
    pinEdit = new QLineEdit(box);
    pinEdit->setPlaceholderText("Mã PIN Giao Dịch (6 số)");
    pinEdit->setEchoMode(QLineEdit::Password);
    pinEdit->setMaxLength(6);
    
    auto registerBtn = new QPushButton("Tạo tài khoản", box);
    registerBtn->setObjectName("accentButton");
    auto backBtn = new QPushButton("Quay lại", box);
    
    boxLayout->addWidget(title);
    boxLayout->addWidget(userEdit);
    boxLayout->addWidget(passEdit);
    boxLayout->addWidget(strengthBar);
    boxLayout->addWidget(strengthLabel);
    boxLayout->addWidget(confirmEdit);
    boxLayout->addWidget(pinEdit);
    boxLayout->addWidget(registerBtn);
    boxLayout->addWidget(backBtn);
    
    layout->addWidget(box);
    
    connect(passEdit, &QLineEdit::textChanged, this, &RegisterPage::onPasswordChanged);
    connect(registerBtn, &QPushButton::clicked, this, &RegisterPage::onRegisterClicked);
    connect(backBtn, &QPushButton::clicked, this, &RegisterPage::goLogin);
}

void RegisterPage::onPasswordChanged(const QString& pwd) {
    int score = 0;
    if (pwd.length() >= 8) score += 25;
    if (pwd.contains(QRegularExpression("[A-Z]"))) score += 25;
    if (pwd.contains(QRegularExpression("[a-z]"))) score += 25;
    if (pwd.contains(QRegularExpression("[0-9]")) && pwd.contains(QRegularExpression("[!@#$%^&*]"))) score += 25;
    
    strengthBar->setValue(score);
    if (score < 50) {
        strengthLabel->setText("Yếu (cần 8+ ký tự, IN HOA, thường, số, ký tự đặc biệt)");
        strengthLabel->setStyleSheet("color: #ff5252;");
        strengthBar->setStyleSheet("QProgressBar::chunk { background-color: #ff5252; }");
    } else if (score < 100) {
        strengthLabel->setText("Trung bình");
        strengthLabel->setStyleSheet("color: #ffd740;");
        strengthBar->setStyleSheet("QProgressBar::chunk { background-color: #ffd740; }");
    } else {
        strengthLabel->setText("Mạnh");
        strengthLabel->setStyleSheet("color: #69f0ae;");
        strengthBar->setStyleSheet("QProgressBar::chunk { background-color: #69f0ae; }");
    }
}

void RegisterPage::onRegisterClicked() {
    if (strengthBar->value() < 100) {
        QMessageBox::warning(this, "Lỗi", "Mật khẩu chưa đủ mạnh!");
        return;
    }
    if (passEdit->text() != confirmEdit->text()) {
        QMessageBox::warning(this, "Lỗi", "Mật khẩu không khớp!");
        return;
    }
    if (pinEdit->text().length() != 6) {
        QMessageBox::warning(this, "Lỗi", "Mã PIN phải đúng 6 số!");
        return;
    }
    
    if (Database::instance().registerUser(userEdit->text(), passEdit->text(), pinEdit->text())) {
        QMessageBox::information(this, "Thành công", "Đăng ký thành công!");
        userEdit->clear(); passEdit->clear(); confirmEdit->clear(); pinEdit->clear();
        emit goLogin();
    } else {
        QMessageBox::warning(this, "Lỗi", "Tên đăng nhập đã tồn tại!");
    }
}

// --- DashboardPage ---
DashboardPage::DashboardPage(QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    
    greetingLabel = new QLabel(this);
    greetingLabel->setObjectName("headerLabel");
    
    auto cardWidget = new QWidget(this);
    cardWidget->setObjectName("cardWidget");
    auto cardLayout = new QVBoxLayout(cardWidget);
    
    auto label1 = new QLabel("Tổng Net Worth ước tính (VND)", cardWidget);
    netWorthLabel = new QLabel(cardWidget);
    netWorthLabel->setObjectName("balanceLabel");
    
    cardLayout->addWidget(label1);
    cardLayout->addWidget(netWorthLabel);
    
    layout->addWidget(greetingLabel);
    layout->addWidget(cardWidget);
    layout->addStretch();
}

void DashboardPage::refresh() {
    int userId = QSettings("VibePay", "App").value("currentUserId", -1).toInt();
    if (userId != -1) {
        greetingLabel->setText("Xin chào, " + Database::instance().getUsername(userId) + "!");
        
        double vnd = Database::instance().getBalance(userId, "VND");
        double btc = Database::instance().getBalance(userId, "BTC");
        // Giả định 1 BTC = 1.5 tỷ VND
        double total = vnd + (btc * 1500000000.0);
        netWorthLabel->setText(QString::number(total, 'f', 0) + " VND");
    }
}

// --- WalletsPage ---
WalletsPage::WalletsPage(QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);

    auto title = new QLabel("Ví của tôi", this);
    title->setObjectName("titleLabel");

    auto hLayout = new QHBoxLayout();
    
    // VND Card
    auto vndCard = new QWidget(this);
    vndCard->setObjectName("cardWidget");
    auto vndLayout = new QVBoxLayout(vndCard);
    vndLayout->addWidget(new QLabel("Số dư VND", vndCard));
    vndLabel = new QLabel(vndCard);
    vndLabel->setObjectName("balanceLabel");
    vndLayout->addWidget(vndLabel);
    
    // BTC Card
    auto btcCard = new QWidget(this);
    btcCard->setObjectName("cardWidget");
    auto btcLayout = new QVBoxLayout(btcCard);
    btcLayout->addWidget(new QLabel("Số dư BTC", btcCard));
    btcLabel = new QLabel(btcCard);
    btcLabel->setObjectName("balanceLabel");
    btcLayout->addWidget(btcLabel);

    hLayout->addWidget(vndCard);
    hLayout->addWidget(btcCard);

    // Fund
    auto fundTitle = new QLabel("Nạp tiền từ Ngân hàng (Mô phỏng)", this);
    fundTitle->setObjectName("headerLabel");
    
    auto fundCard = new QWidget(this);
    fundCard->setObjectName("cardWidget");
    auto fLayout = new QHBoxLayout(fundCard);
    fundAmountEdit = new QLineEdit(fundCard);
    fundAmountEdit->setPlaceholderText("Nhập số tiền nạp (VND)");
    auto fundBtn = new QPushButton("Nạp Tiền", fundCard);
    fundBtn->setObjectName("accentButton");
    
    fLayout->addWidget(fundAmountEdit);
    fLayout->addWidget(fundBtn);

    layout->addWidget(title);
    layout->addLayout(hLayout);
    layout->addSpacing(30);
    layout->addWidget(fundTitle);
    layout->addWidget(fundCard);
    layout->addStretch();
    
    connect(fundBtn, &QPushButton::clicked, this, &WalletsPage::onFundClicked);
}

void WalletsPage::refresh() {
    int userId = QSettings("VibePay", "App").value("currentUserId", -1).toInt();
    if (userId != -1) {
        vndLabel->setText(QString::number(Database::instance().getBalance(userId, "VND"), 'f', 0) + " ₫");
        btcLabel->setText(QString::number(Database::instance().getBalance(userId, "BTC"), 'f', 4) + " ₿");
    }
}

void WalletsPage::onFundClicked() {
    double amount = fundAmountEdit->text().toDouble();
    if (amount <= 0) {
        QMessageBox::warning(this, "Lỗi", "Số tiền không hợp lệ!");
        return;
    }
    int userId = QSettings("VibePay", "App").value("currentUserId", -1).toInt();
    if (Database::instance().fundFromBank(userId, amount)) {
        QMessageBox::information(this, "Thành công", "Nạp tiền thành công!");
        fundAmountEdit->clear();
        refresh();
    } else {
        QMessageBox::warning(this, "Lỗi", "Lỗi nạp tiền!");
    }
}

// --- TransferPage ---
TransferPage::TransferPage(QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    auto title = new QLabel("Chuyển tiền", this);
    title->setObjectName("titleLabel");

    auto card = new QWidget(this);
    card->setObjectName("cardWidget");
    auto cardLayout = new QVBoxLayout(card);

    currencyCombo = new QComboBox(card);
    currencyCombo->addItem("VND");
    currencyCombo->addItem("BTC");
    
    recipientEdit = new QLineEdit(card);
    recipientEdit->setPlaceholderText("Tên đăng nhập người nhận");
    
    amountEdit = new QLineEdit(card);
    amountEdit->setPlaceholderText("Số tiền");
    
    noteEdit = new QLineEdit(card);
    noteEdit->setPlaceholderText("Ghi chú");
    
    auto transferBtn = new QPushButton("Xác nhận chuyển", card);
    transferBtn->setObjectName("accentButton");

    cardLayout->addWidget(new QLabel("Loại tiền", card));
    cardLayout->addWidget(currencyCombo);
    cardLayout->addWidget(recipientEdit);
    cardLayout->addWidget(amountEdit);
    cardLayout->addWidget(noteEdit);
    cardLayout->addWidget(transferBtn);
    
    layout->addWidget(title);
    layout->addWidget(card);
    layout->addStretch();
    
    connect(transferBtn, &QPushButton::clicked, this, &TransferPage::onTransferClicked);
}

void TransferPage::refresh() {
    recipientEdit->clear();
    amountEdit->clear();
    noteEdit->clear();
}

void TransferPage::onTransferClicked() {
    QString recipient = recipientEdit->text();
    double amount = amountEdit->text().toDouble();
    if (recipient.isEmpty() || amount <= 0) return;
    
    int toUserId = Database::instance().getUserId(recipient);
    if (toUserId == -1) {
        QMessageBox::warning(this, "Lỗi", "Người dùng không tồn tại!");
        return;
    }
    
    int currentId = QSettings("VibePay", "App").value("currentUserId", -1).toInt();
    if (currentId == toUserId) return;
    
    // Yêu cầu PIN
    bool ok;
    QString pin = QInputDialog::getText(this, "Xác minh PIN", "Nhập mã PIN:", QLineEdit::Password, "", &ok);
    if (!ok) return;
    
    if (!Database::instance().verifyPin(currentId, pin)) {
        QMessageBox::warning(this, "Lỗi", "Mã PIN không đúng!");
        return;
    }
    
    if (Database::instance().transfer(currentId, toUserId, currencyCombo->currentText(), amount, noteEdit->text())) {
        QMessageBox::information(this, "Thành công", "Giao dịch thành công!");
        refresh();
    } else {
        QMessageBox::warning(this, "Lỗi", "Số dư không đủ!");
    }
}

// --- SwapPage ---
SwapPage::SwapPage(QWidget* parent) : QWidget(parent) {
    currentRate = 1500000000.0; // 1.5 Ty VND cho 1 BTC
    auto layout = new QVBoxLayout(this);
    
    auto title = new QLabel("Quy đổi (Swap VND -> BTC)", this);
    title->setObjectName("titleLabel");

    auto card = new QWidget(this);
    card->setObjectName("cardWidget");
    auto cardLayout = new QVBoxLayout(card);

    fromAmountEdit = new QLineEdit(card);
    fromAmountEdit->setPlaceholderText("Số tiền chi ra (VND)");
    
    toAmountLabel = new QLabel("Nhận được: 0 BTC", card);
    toAmountLabel->setObjectName("headerLabel");
    
    auto rateLabel = new QLabel("Tỷ giá: 1 BTC = 1.500.000.000 VND", card);
    
    auto swapBtn = new QPushButton("Thực hiện Swap", card);
    swapBtn->setObjectName("accentButton");

    cardLayout->addWidget(rateLabel);
    cardLayout->addSpacing(10);
    cardLayout->addWidget(fromAmountEdit);
    cardLayout->addWidget(toAmountLabel);
    cardLayout->addWidget(swapBtn);
    
    layout->addWidget(title);
    layout->addWidget(card);
    layout->addStretch();
    
    connect(fromAmountEdit, &QLineEdit::textChanged, this, &SwapPage::onAmountChanged);
    connect(swapBtn, &QPushButton::clicked, this, &SwapPage::onSwapClicked);
}

void SwapPage::refresh() {
    fromAmountEdit->clear();
}

void SwapPage::onAmountChanged(const QString& text) {
    double vnd = text.toDouble();
    double btc = vnd / currentRate;
    toAmountLabel->setText("Nhận được: " + QString::number(btc, 'f', 6) + " BTC");
}

void SwapPage::onSwapClicked() {
    double vnd = fromAmountEdit->text().toDouble();
    if (vnd <= 0) return;
    
    int currentId = QSettings("VibePay", "App").value("currentUserId", -1).toInt();
    
    // Verify PIN
    bool ok;
    QString pin = QInputDialog::getText(this, "Xác minh PIN", "Nhập mã PIN để Swap:", QLineEdit::Password, "", &ok);
    if (!ok) return;
    if (!Database::instance().verifyPin(currentId, pin)) {
        QMessageBox::warning(this, "Lỗi", "Sai PIN!");
        return;
    }
    
    double btc = vnd / currentRate;
    if (Database::instance().swap(currentId, "VND", "BTC", vnd, btc)) {
        QMessageBox::information(this, "Thành công", "Quy đổi thành công!");
        refresh();
    } else {
        QMessageBox::warning(this, "Lỗi", "Số dư không đủ!");
    }
}

// --- HistoryPage ---
HistoryPage::HistoryPage(QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    auto title = new QLabel("Lịch sử giao dịch", this);
    title->setObjectName("titleLabel");

    tableView = new QTableView(this);
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"Thời gian", "Tiền", "Tài sản", "Đối tác / Ghi chú"});
    tableView->setModel(model);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    layout->addWidget(title);
    layout->addWidget(tableView);
}

void HistoryPage::refresh() {
    model->removeRows(0, model->rowCount());
    int currentId = QSettings("VibePay", "App").value("currentUserId", -1).toInt();
    auto history = Database::instance().getHistory(currentId);
    
    int row = 0;
    for (const auto& t : history) {
        QString otherUser = (t.fromUserId == currentId) ? "Gửi tới: " + Database::instance().getUsername(t.toUserId)
                                                        : "Nhận từ: " + Database::instance().getUsername(t.fromUserId);
        if (t.fromUserId == 0) otherUser = "Hệ thống / Ngân hàng";
        
        QString sign = (t.fromUserId == currentId) ? "-" : "+";
        
        model->insertRow(row);
        model->setItem(row, 0, new QStandardItem(t.timestamp));
        
        auto amtItem = new QStandardItem(sign + QString::number(t.amount, 'f', 4));
        if (t.fromUserId == currentId) amtItem->setForeground(QColor("#ff5252"));
        else amtItem->setForeground(QColor("#69f0ae"));
        
        model->setItem(row, 1, amtItem);
        model->setItem(row, 2, new QStandardItem(t.currency));
        model->setItem(row, 3, new QStandardItem(otherUser + " (" + t.note + ")"));
        row++;
    }
}
