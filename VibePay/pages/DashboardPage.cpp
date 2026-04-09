#include "DashboardPage.h"
#include "../Database.h"
#include <QVBoxLayout>
#include <QSettings>

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
