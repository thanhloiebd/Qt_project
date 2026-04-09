#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include <QWidget>
#include <QLabel>

class DashboardPage : public QWidget {
    Q_OBJECT
public:
    explicit DashboardPage(QWidget* parent = nullptr);
    void refresh();
private:
    QLabel* greetingLabel;
    QLabel* netWorthLabel;
};

#endif // DASHBOARDPAGE_H
