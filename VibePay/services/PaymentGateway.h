#ifndef PAYMENTGATEWAY_H
#define PAYMENTGATEWAY_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class PaymentGateway : public QObject {
    Q_OBJECT
public:
    explicit PaymentGateway(QObject *parent = nullptr);
    static PaymentGateway& instance();

    // Gọi API xin URL thanh toán (MoMo, VNPay, Stripe)
    void createPaymentUrl(double amount, const QString& gatewayCode);
    
    // Kiểm tra giao dịch đã hoàn tất trên hệ thống chưa
    void checkPaymentStatus(const QString& transactionId);

signals:
    void paymentUrlReceived(const QString& url, const QString& transactionId);
    void paymentUrlFailed(const QString& errorMsg);

    void paymentCompleted(double amount, const QString& gatewayCode);
    void paymentFailed(const QString& errorMsg);

private slots:
    void onPaymentUrlReady(QNetworkReply* reply);

private:
    QNetworkAccessManager* manager;
    
    // Lưu nháp trạng thái giả lập pending
    double pendingAmount;
    QString pendingGateway;
    QString currentTransactionId;
};

#endif // PAYMENTGATEWAY_H
