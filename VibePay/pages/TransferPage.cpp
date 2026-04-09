#include "TransferPage.h"
#include "../Database.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QInputDialog>
#include <QSettings>

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
