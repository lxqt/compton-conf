#include "compton-conf.h"
#include <QApplication>
#include <QLocale>
#include <QLibraryInfo>
#include <QTranslator>
#include "maindialog.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // load translations
    QTranslator qtTranslator, translator;
    // install the translations built-into Qt itself
    qtTranslator.load("qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);
    // install our own tranlations
    translator.load("compton-conf_" + QLocale::system().name(), COMPTON_CONF_DATA_DIR "/translations");
    app.installTranslator(&translator);

    MainDialog dlg;
    dlg.exec();
    return 0;
}
