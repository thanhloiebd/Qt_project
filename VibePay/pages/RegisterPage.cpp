#include "RegisterPage.h"
#include "../Database.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QRegularExpression>

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
