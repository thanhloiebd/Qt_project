#include "WalletsPage.h"
#include "../Database.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QSettings>

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
