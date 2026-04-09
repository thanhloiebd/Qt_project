#ifndef WALLETSPAGE_H
#define WALLETSPAGE_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>

class WalletsPage : public QWidget {
    Q_OBJECT
public:
    explicit WalletsPage(QWidget* parent = nullptr);
    void refresh();
private slots:
    void onFundClicked();
private:
    QLabel* vndLabel;
    QLabel* btcLabel;
    QLineEdit* fundAmountEdit;
};

#endif // WALLETSPAGE_H
