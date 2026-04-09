#include "PaymentGateway.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QUuid>
#include <QTimer>

PaymentGateway& PaymentGateway::instance() {
    static PaymentGateway instance;
    return instance;
}

PaymentGateway::PaymentGateway(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
}

void PaymentGateway::createPaymentUrl(double amount, const QString& gatewayCode) {
    pendingAmount = amount;
    pendingGateway = gatewayCode;
    currentTransactionId = QUuid::createUuid().toString(QUuid::WithoutBraces);

    // Endpoint giả lập cho Sandbox (Sử dụng https://httpbin.org/post để test POST)
    QUrl url("https://httpbin.org/post");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["amount"] = amount;
    json["gateway"] = gatewayCode;
    json["transaction_id"] = currentTransactionId;
    
    // Nếu bạn có API Key, truyền vào đây
    // request.setRawHeader("Authorization", "Bearer YOUR_SECRET_KEY");

    QJsonDocument doc(json);
    QNetworkReply* reply = manager->post(request, doc.toJson());
    
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onPaymentUrlReady(reply);
    });
}

void PaymentGateway::onPaymentUrlReady(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        // Parse the response
        QByteArray response_data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(response_data);
        QJsonObject root = doc.object();
        
        // Trong hệ thống VNPay thật, họ sẽ trả JSON dạng { "checkout_url": "https://..." }
        // Tuy nhiên vì ở đây dùng httpbin nên ta lấy lại json truyền đi dể confirm gọi mạng thành công.
        
        // Trả URL web giả lập kèm transaction_id của người dùng để mở ở trình duyệt (ví dụ search google)
        QString mockPaymentUrl = "https://sandbox.vnpayment.vn/paymentv2/vpcpay.html?txn_id=" + currentTransactionId;
        
        emit paymentUrlReceived(mockPaymentUrl, currentTransactionId);
    } else {
        emit paymentUrlFailed("Không thể kết nối (Lỗi Mạng): " + reply->errorString());
    }
    reply->deleteLater();
}

void PaymentGateway::checkPaymentStatus(const QString& transactionId) {
    // Trong thực tế, bạn gọi API check hệ thống: manager->get(".../api/check?txn=" + transactionId)
    // Ở đây, ta giả lập rằng cứ gọi là "Giao dịch đã được duyệt ở Bank" sau 1.5s chờ
    
    QTimer::singleShot(1500, this, [this, transactionId]() {
        if (transactionId == currentTransactionId) {
             emit paymentCompleted(pendingAmount, pendingGateway);
        } else {
             emit paymentFailed("Mã giao dịch không hợp lệ hoặc đã hết hạn.");
        }
    });
}
