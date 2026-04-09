#ifndef WALLETSPAGE_H
#define WALLETSPAGE_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QProgressDialog>

class WalletsPage : public QWidget {
    Q_OBJECT
public:
    explicit WalletsPage(QWidget* parent = nullptr);
    void refresh();
private slots:
    void onFundClicked();
    
    // Xử lý luồng cổng thanh toán
    void onPaymentUrlReceived(const QString& url, const QString& txnId);
    void onPaymentCompleted(double amount, const QString& gateway);
    void onPaymentFailed(const QString& errorMsg);
    void onCheckStatusClicked(const QString& txnId);

private:
    QLabel* vndLabel;
    QLabel* btcLabel;
    QLineEdit* fundAmountEdit;
    QComboBox* gatewayCombo; // Chọn VNPay / Stripe
};

#endif // WALLETSPAGE_H
