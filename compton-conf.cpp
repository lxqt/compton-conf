#include "compton-conf.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include "maindialog.h"
#include <libconfig.h>
#include <stdio.h>

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    // find config file
    QString userConfigFile = qgetenv("XDG_CONFIG_HOME");
    if(userConfigFile.isEmpty()) {
      userConfigFile = QDir::homePath();
      userConfigFile += "/.config";
    }
    // QDir configDir = QDir(userConfigFile);
    // if(!configDir.exists())
    userConfigFile += "/compton.conf";

    // load config file
    config_t cfg;
    config_init(&cfg);
    if(config_read_file(&cfg, userConfigFile.toLocal8Bit().constData()) == CONFIG_FALSE) {
      // loading user config file failed
      // try our default example
    }

    MainDialog dlg(&cfg);
    dlg.exec();
    return 0;
}
