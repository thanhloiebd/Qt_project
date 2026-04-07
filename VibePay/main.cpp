#include "MainWindow.h"
#include "Database.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    
    // Khởi tạo Database
    if (!Database::instance().init()) {
        return -1; // Lỗi khởi tạo DB
    }
    
    MainWindow w;
    w.show();
    
    return a.exec();
}
