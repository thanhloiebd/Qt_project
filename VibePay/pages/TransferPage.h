#ifndef TRANSFERPAGE_H
#define TRANSFERPAGE_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>

class TransferPage : public QWidget {
    Q_OBJECT
public:
    explicit TransferPage(QWidget* parent = nullptr);
    void refresh();
private slots:
    void onTransferClicked();
private:
    QComboBox* currencyCombo;
    QLineEdit* recipientEdit;
    QLineEdit* amountEdit;
    QLineEdit* noteEdit;
};

#endif // TRANSFERPAGE_H
