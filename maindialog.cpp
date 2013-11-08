/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2013  <copyright holder> <email>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */

#include "maindialog.h"
#include "ui_maindialog.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDBusInterface>
#include <QDBusConnection>

// dbus interface of compton
#define COMPTON_SERVICE    "com.github.chjj.compton._0"
#define COMPTON_PATH       "/"
#define COMPTON_INTERFACE  "com.github.chjj.compton"


MainDialog::MainDialog(QString userConfigFile) {
  ui = new Ui::MainDialog;
  ui->setupUi(this);

  if(userConfigFile.isEmpty()) {
    userConfigFile_ = qgetenv("XDG_CONFIG_HOME");
    if(userConfigFile_.isEmpty()) {
      userConfigFile_ = QDir::homePath();
      userConfigFile_ += "/.config";
    }
    // QDir configDir = QDir(userConfigFile);
    // if(!configDir.exists())
    userConfigFile_ += "/compton.conf";
  }
  else
    userConfigFile_ = userConfigFile;

  config_init(&config_);
  if(config_read_file(&config_, userConfigFile_.toLocal8Bit().constData()) == CONFIG_FALSE) {
    // loading user config file failed
    // try our default example
    qDebug() << "load fail, try " << COMPTON_CONF_DATA_DIR << "/compton.conf.example";
    config_read_file(&config_, COMPTON_CONF_DATA_DIR "/compton.conf.example");
  }

  // set up signal handlers and initial values of the controls
  // objectNames are kept the same as config file key names.
  Q_FOREACH(QWidget* child, findChildren<QWidget*>()) {
    if(!child->isWidgetType() || child->objectName().isEmpty())
      continue;
    // objectName uses _ while config file keys uses - as separator.
    QByteArray keyName = child->objectName().replace('_', '-').toLatin1(); // generate config key from objectName.
    if(child->inherits("QCheckBox")) {
      int val = -1;
      if(config_lookup_bool(&config_, keyName.constData(), &val) == CONFIG_TRUE)
	static_cast<QCheckBox*>(child)->setChecked((bool)val);
      connect(child, SIGNAL(toggled(bool)), SLOT(onButtonToggled(bool)));
    }
    else if(child->inherits("QDoubleSpinBox")) {
      double val;
      if(config_lookup_float(&config_, keyName.constData(), &val) == CONFIG_TRUE)
	static_cast<QDoubleSpinBox*>(child)->setValue(val);
      connect(child, SIGNAL(valueChanged(double)), SLOT(onSpinValueChanged(double)));
    }
    else if(child->inherits("QSpinBox")) {
      int val;
      if(config_lookup_int(&config_, keyName.constData(), &val) == CONFIG_TRUE)
	static_cast<QSpinBox*>(child)->setValue(val);
      connect(child, SIGNAL(valueChanged(int)), SLOT(onSpinValueChanged(int)));
    }
  }
}

MainDialog::~MainDialog() {
  config_destroy(&config_);
    delete ui;
}

void MainDialog::onButtonToggled(bool checked) {
  qDebug() << "toggled: " << sender()->objectName();
  // generate config key from objectName.
  QByteArray keyName = sender()->objectName().replace('_', '-').toLatin1();
  config_setting_t* setting = config_lookup(&config_, keyName.constData());
  if(!setting) { // setting not found
    // add a new setting for it
    config_setting_t* root = config_root_setting(&config_);
    setting = config_setting_add(root, keyName.constData(), CONFIG_TYPE_BOOL);
  }
  // set the value
  config_setting_set_bool(setting, checked);
  saveConfig();
}

void MainDialog::onSpinValueChanged(double d) {
  qDebug() << "changed: " << sender()->objectName() << ": " << d;
  // generate config key from objectName.
  QByteArray keyName = sender()->objectName().replace('_', '-').toLatin1();
  config_setting_t* setting = config_lookup(&config_, keyName.constData());
  if(!setting) { // setting not found
    // add a new setting for it
    config_setting_t* root = config_root_setting(&config_);
    setting = config_setting_add(root, keyName.constData(), CONFIG_TYPE_FLOAT);
  }
  // set the value
  config_setting_set_float(setting, d);
  saveConfig();
}

void MainDialog::onSpinValueChanged(int i) {
  qDebug() << "changed: " << sender()->objectName() << ": " << i;
  // generate config key from objectName.
  QByteArray keyName = sender()->objectName().replace('_', '-').toLatin1();
  config_setting_t* setting = config_lookup(&config_, keyName.constData());
  if(!setting) { // setting not found
    // add a new setting for it
    config_setting_t* root = config_root_setting(&config_);
    setting = config_setting_add(root, keyName.constData(), CONFIG_TYPE_INT);
  }
  // set the value
  config_setting_set_int(setting, i);
  saveConfig();
}

void MainDialog::saveConfig() {
  // ensure the existance of user config dir
  QString configDir = QFileInfo(userConfigFile_).dir().path();
  QDir().mkpath(configDir);
  qDebug() << userConfigFile_;
  // save the config file
  config_write_file(&config_, userConfigFile_.toLocal8Bit().constData());
  
  // ask compton to reload the config
  QDBusInterface iface(COMPTON_SERVICE, COMPTON_PATH, COMPTON_INTERFACE);
  if(iface.isValid())
    iface.call("reset");
  // FIXME: dbus interface of compton is not always available and reset() creates
  // much flickers. Maybe we should use internal dbus method set_opts().
  // Or, we can patch compton to do what we want.
}

void MainDialog::done(int res) {
  QDialog::done(res);
}
