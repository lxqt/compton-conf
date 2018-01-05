#include "compton-conf.h"
#include <QApplication>
#include <QLocale>
#include <QLibraryInfo>
#include <QTranslator>
#ifndef USE_QT4
#include <QCommandLineParser>
#endif
#include "maindialog.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
#ifndef USE_QT4
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Compton Conf"));
    const QString VERINFO = QStringLiteral(COMPTONCONF_VERSION
                                           "\nQt        " QT_VERSION_STR);
    app.setApplicationVersion(VERINFO);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.process(app);
#endif

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
