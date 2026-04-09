#ifndef SWAPPAGE_H
#define SWAPPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>

class SwapPage : public QWidget {
    Q_OBJECT
public:
    explicit SwapPage(QWidget* parent = nullptr);
    void refresh();
private slots:
    void onSwapClicked();
    void onAmountChanged(const QString& text);
private:
    QLineEdit* fromAmountEdit;
    QLabel* toAmountLabel;
    double currentRate; // VND per 1 BTC
};

#endif // SWAPPAGE_H
