#ifndef HISTORYPAGE_H
#define HISTORYPAGE_H

#include <QWidget>
#include <QTableView>
#include <QStandardItemModel>

class HistoryPage : public QWidget {
    Q_OBJECT
public:
    explicit HistoryPage(QWidget* parent = nullptr);
    void refresh();
private:
    QTableView* tableView;
    QStandardItemModel* model;
};

#endif // HISTORYPAGE_H
