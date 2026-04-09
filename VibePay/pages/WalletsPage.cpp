#include "WalletsPage.h"
#include "../Database.h"
#include "../services/PaymentGateway.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QSettings>
#include <QDesktopServices>
#include <QUrl>

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
    auto fundTitle = new QLabel("Nạp tiền (Gateway Thật/Sandbox)", this);
    fundTitle->setObjectName("headerLabel");
    
    auto fundCard = new QWidget(this);
    fundCard->setObjectName("cardWidget");
    auto fLayout = new QHBoxLayout(fundCard);
    
    gatewayCombo = new QComboBox(fundCard);
    gatewayCombo->addItem("Cổng VNPay (Sandbox)", "VNPAY");
    gatewayCombo->addItem("Thẻ Quốc tế Stripe (Sandbox)", "STRIPE");
    
    fundAmountEdit = new QLineEdit(fundCard);
    fundAmountEdit->setPlaceholderText("Nhập số tiền nạp (VND)");
    
    auto fundBtn = new QPushButton("Thanh toán Web", fundCard);
    fundBtn->setObjectName("accentButton");
    
    fLayout->addWidget(gatewayCombo);
    fLayout->addWidget(fundAmountEdit);
    fLayout->addWidget(fundBtn);

    layout->addWidget(title);
    layout->addLayout(hLayout);
    layout->addSpacing(30);
    layout->addWidget(fundTitle);
    layout->addWidget(fundCard);
    layout->addStretch();
    
    connect(fundBtn, &QPushButton::clicked, this, &WalletsPage::onFundClicked);
    
    // Connect Gateway Signals
    connect(&PaymentGateway::instance(), &PaymentGateway::paymentUrlReceived, 
            this, &WalletsPage::onPaymentUrlReceived);
    connect(&PaymentGateway::instance(), &PaymentGateway::paymentUrlFailed, 
            this, &WalletsPage::onPaymentFailed);
    connect(&PaymentGateway::instance(), &PaymentGateway::paymentCompleted, 
            this, &WalletsPage::onPaymentCompleted);
    connect(&PaymentGateway::instance(), &PaymentGateway::paymentFailed, 
            this, &WalletsPage::onPaymentFailed);
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
    if (amount < 10000) {
        QMessageBox::warning(this, "Lỗi", "Số tiền phải từ 10,000 VND trở lên!");
        return;
    }
    
    QString gatewayCode = gatewayCombo->currentData().toString();
    
    // Gửi tín hiệu gọi API lên mạng
    fundAmountEdit->setEnabled(false);
    PaymentGateway::instance().createPaymentUrl(amount, gatewayCode);
}

void WalletsPage::onPaymentUrlReceived(const QString& url, const QString& txnId) {
    fundAmountEdit->setEnabled(true);
    
    // 1. Mở trình duyệt web hệ thống để giả tỷ người dùng điền thẻ
    QDesktopServices::openUrl(QUrl(url));
    
    // 2. Hiển thị thông báo yêu cầu chờ
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Đang thanh toán...");
    msgBox.setText(QString("Trình duyệt đã mở URL Thanh toán (%1).\nVui lòng thanh toán trên trình duyệt, sau đó nhấn Kiểm tra!").arg(gatewayCombo->currentText()));
    
    QPushButton* checkBtn = msgBox.addButton("Tôi đã thanh toán (Kiểm tra)", QMessageBox::ActionRole);
    msgBox.addButton("Hủy bỏ", QMessageBox::RejectRole);
    
    msgBox.exec();
    
    if (msgBox.clickedButton() == checkBtn) {
        onCheckStatusClicked(txnId);
    }
}

void WalletsPage::onCheckStatusClicked(const QString& txnId) {
    // Gọi API kiểm tra thanh toán (Mã giao dịch)
    PaymentGateway::instance().checkPaymentStatus(txnId);
}

void WalletsPage::onPaymentCompleted(double amount, const QString& gateway) {
    int userId = QSettings("VibePay", "App").value("currentUserId", -1).toInt();
    
    // Cập nhật Database với Ghi chú tương ứng của Gateway
    QString note = QString("Nạp tiền qua Cổng %1").arg(gateway);
    
    if (Database::instance().fundFromBank(userId, amount, note)) {
        QMessageBox::information(this, "Thành công", QString("Nạp %1 ₫ thành công qua %2!").arg(amount).arg(gateway));
        fundAmountEdit->clear();
        refresh();
    } else {
        QMessageBox::warning(this, "Lỗi", "Giao dịch thành công ở Bank nhưng lỗi Database!");
    }
}

void WalletsPage::onPaymentFailed(const QString& errorMsg) {
    fundAmountEdit->setEnabled(true);
    QMessageBox::warning(this, "Thanh toán thất bại", errorMsg);
}
