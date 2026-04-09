#include "LoginPage.h"
#include "../Database.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

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
