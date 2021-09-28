//
//    ConfigDialog.h: Configuration dialog window
//    Copyright (C) 2018 Gonzalo José Carracedo Carballal
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
#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <ui_Config.h>
#include <Suscan/AnalyzerParams.h>
#include <Suscan/Source.h>
#include <ColorConfig.h>
#include <GuiConfig.h>
#include <SaveProfileDialog.h>

namespace SigDigger {
  class ProfileConfigTab;
  class ColorConfigTab;
  class GuiConfigTab;
  class LocationConfigTab;

  class ConfigDialog : public QDialog
  {
    Q_OBJECT

  private:
    // We keep these for the time being
    Suscan::AnalyzerParams analyzerParams;

    // UI elements
    ProfileConfigTab  *profileTab  = nullptr;
    ColorConfigTab    *colorTab    = nullptr;
    GuiConfigTab      *guiTab      = nullptr;
    LocationConfigTab *locationTab = nullptr;

    bool accepted = false;

    Ui_Config *ui = nullptr;

    void connectAll(void);

  public:
    // The public API remains.
    void setProfile(const Suscan::Source::Config &profile);
    void setAnalyzerParams(const Suscan::AnalyzerParams &params);
    void setColors(const ColorConfig &config);
    void setGuiConfig(const GuiConfig &config);
    void setGain(std::string const &name, float value);
    void setFrequency(qint64 freq);
    void notifySingletonChanges(void);
    bool remoteSelected(void) const;

    float getGain(std::string const &name) const;
    Suscan::Source::Config getProfile(void) const;
    ColorConfig getColors(void) const;
    GuiConfig getGuiConfig() const;
    Suscan::AnalyzerParams getAnalyzerParams(void) const;

    bool run(void);

    explicit ConfigDialog(QWidget *parent = nullptr);
    ~ConfigDialog();

  public slots:
    void onAccepted(void);
  };
};


#endif // CONFIGDIALOG_H
