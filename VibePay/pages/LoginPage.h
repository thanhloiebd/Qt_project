#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>
#include <QLineEdit>

class LoginPage : public QWidget {
    Q_OBJECT
public:
    explicit LoginPage(QWidget* parent = nullptr);
signals:
    void loginSuccess();
    void goRegister();
private slots:
    void onLoginClicked();
private:
    QLineEdit* userEdit;
    QLineEdit* passEdit;
};

#endif // LOGINPAGE_H
