#ifndef REGISTERPAGE_H
#define REGISTERPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QProgressBar>
#include <QLabel>

class RegisterPage : public QWidget {
    Q_OBJECT
public:
    explicit RegisterPage(QWidget* parent = nullptr);
signals:
    void registerSuccess();
    void goLogin();
private slots:
    void onRegisterClicked();
    void onPasswordChanged(const QString& pwd);
private:
    QLineEdit* userEdit;
    QLineEdit* passEdit;
    QLineEdit* confirmEdit;
    QLineEdit* pinEdit;
    QProgressBar* strengthBar;
    QLabel* strengthLabel;
};

#endif // REGISTERPAGE_H
