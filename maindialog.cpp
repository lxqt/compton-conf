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

MainDialog::MainDialog(config_t* cfg) : config_(cfg) {
  ui = new Ui::MainDialog;
  ui->setupUi(this);

    // set up signal handlers
  Q_FOREACH(QWidget* child, findChildren<QWidget*>()) {
    if(!child->isWidgetType() || child->objectName().isEmpty())
      continue;
    QString keyName = child->objectName().replace('_', '-');
    if(child->inherits("QCheckBox")) {
      int val;
      if(config_lookup_bool(config_, keyName.toLatin1().constData(), &val) == CONFIG_TRUE)
	static_cast<QCheckBox*>(child)->setChecked((bool)val);
      connect(child, SIGNAL(toggled(bool)), SLOT(onButtonToggled(bool)));
    }
    else if(child->inherits("QDoubleSpinBox")) {
      double val;
      if(config_lookup_float(config_, keyName.toLatin1().constData(), &val) == CONFIG_TRUE)
	static_cast<QDoubleSpinBox*>(child)->setValue(val);
      connect(child, SIGNAL(valueChanged(double)), SLOT(onSpinValueChanged(double)));
    }
    else if(child->inherits("QSpinBox")) {
      int val;
      if(config_lookup_int(config_, keyName.toLatin1().constData(), &val) == CONFIG_TRUE)
	static_cast<QSpinBox*>(child)->setValue(val);
      connect(child, SIGNAL(valueChanged(int)), SLOT(onSpinValueChanged(int)));
    }
  }
}

MainDialog::~MainDialog() {
    delete ui;
}

void MainDialog::onButtonToggled(bool checked) {
  qDebug() << "toggled: " << sender()->objectName();
}

void MainDialog::onSpinValueChanged(double d) {
  qDebug() << "changed: " << sender()->objectName() << ": " << d;
}

void MainDialog::onSpinValueChanged(int i) {
  qDebug() << "changed: " << sender()->objectName() << ": " << i;
}
