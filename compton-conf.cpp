#include "compton-conf.h"
#include <QApplication>
#include "maindialog.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    MainDialog dlg;
    dlg.exec();
    return 0;
}
