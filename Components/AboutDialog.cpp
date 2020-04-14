//
//    AboutDialog.cpp: About dialog
//    Copyright (C) 2019 Gonzalo José Carracedo Carballal
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as
//    published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public
//    License along with this program.  If not, see
//    <http://www.gnu.org/licenses/>
//

#include "AboutDialog.h"
#include "ui_AboutDialog.h"
#include <Suscan/Library.h>

using namespace SigDigger;

AboutDialog::AboutDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::AboutDialog)
{
  QString sigutilsVer;
  QString suscanVer;

  ui->setupUi(this);

  sigutilsVer = "sigutils "
      + QString::fromStdString(Suscan::Singleton::sigutilsVersion());
  suscanVer = "suscan "
      + QString::fromStdString(Suscan::Singleton::suscanVersion());

  this->ui->sigutilsVerLabel->setText(sigutilsVer);
  this->ui->suscanVerLabel->setText(suscanVer);
}

AboutDialog::~AboutDialog()
{
  delete ui;
}
