#include "HistoryPage.h"
#include "../Database.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QSettings>

HistoryPage::HistoryPage(QWidget* parent) : QWidget(parent) {
    auto layout = new QVBoxLayout(this);
    auto title = new QLabel("Lịch sử giao dịch", this);
    title->setObjectName("titleLabel");

    tableView = new QTableView(this);
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderLabels({"Thời gian", "Tiền", "Tài sản", "Đối tác / Ghi chú"});
    tableView->setModel(model);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    layout->addWidget(title);
    layout->addWidget(tableView);
}

void HistoryPage::refresh() {
    model->removeRows(0, model->rowCount());
    int currentId = QSettings("VibePay", "App").value("currentUserId", -1).toInt();
    auto history = Database::instance().getHistory(currentId);
    
    int row = 0;
    for (const auto& t : history) {
        QString otherUser = (t.fromUserId == currentId) ? "Gửi tới: " + Database::instance().getUsername(t.toUserId)
                                                        : "Nhận từ: " + Database::instance().getUsername(t.fromUserId);
        if (t.fromUserId == 0) otherUser = "Hệ thống / Ngân hàng";
        
        QString sign = (t.fromUserId == currentId) ? "-" : "+";
        
        model->insertRow(row);
        model->setItem(row, 0, new QStandardItem(t.timestamp));
        
        auto amtItem = new QStandardItem(sign + QString::number(t.amount, 'f', 4));
        if (t.fromUserId == currentId) amtItem->setForeground(QColor("#ff5252"));
        else amtItem->setForeground(QColor("#69f0ae"));
        
        model->setItem(row, 1, amtItem);
        model->setItem(row, 2, new QStandardItem(t.currency));
        model->setItem(row, 3, new QStandardItem(otherUser + " (" + t.note + ")"));
        row++;
    }
}
