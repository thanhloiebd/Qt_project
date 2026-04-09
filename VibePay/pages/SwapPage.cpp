#include "SwapPage.h"
#include "../Database.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QSettings>

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
