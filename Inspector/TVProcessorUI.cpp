//
//    TVProcessorUI.cpp: TV Processor UI logic
//    Copyright (C) 2020 Gonzalo José Carracedo Carballal
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

#include "InspectorUI.h"
#include "ui_Inspector.h"
#include <QMessageBox>

using namespace SigDigger;


void
InspectorUI::connectTVProcessorUi(void)
{
  connect(
        this->tvWorker,
        SIGNAL(paramsChanged(struct sigutils_tv_processor_params)),
        this,
        SLOT(onTVProcessorParamsChanged(struct sigutils_tv_processor_params)));

  connect(
        this->tvWorker,
        SIGNAL(error(QString)),
        this,
        SLOT(onTVProcessorError(QString)));

  connect(
        this->tvWorker,
        SIGNAL(frame(struct sigutils_tv_frame_buffer *)),
        this,
        SLOT(onTVProcessorFrame(struct sigutils_tv_frame_buffer *)));

  connect(
        this->ui->enableTvButton,
        SIGNAL(clicked(bool)),
        this,
        SLOT(onToggleTVProcessor(void)));

  connect(
        this,
        SIGNAL(startTVProcessor(void)),
        this->tvWorker,
        SLOT(start(void)));

  connect(
        this,
        SIGNAL(stopTVProcessor(void)),
        this->tvWorker,
        SLOT(stop(void)));

  connect(
        this,
        SIGNAL(tvProcessorData()),
        this->tvWorker,
        SLOT(process()));

  connect(
        this,
        SIGNAL(tvProcessorDisposeFrame(struct sigutils_tv_frame_buffer *)),
        this->tvWorker,
        SLOT(returnFrame(struct sigutils_tv_frame_buffer *)));

  connect(
        this,
        SIGNAL(tvProcessorParams(sigutils_tv_processor_params)),
        this->tvWorker,
        SLOT(setParams(sigutils_tv_processor_params)));

  // UI signals
  connect(
        this->ui->ntscRadio,
        SIGNAL(toggled(bool)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->palRadio,
        SIGNAL(toggled(bool)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->customRadio,
        SIGNAL(toggled(bool)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->agcCheck,
        SIGNAL(toggled(bool)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->enableSyncCheck,
        SIGNAL(toggled(bool)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->invertSyncCheck,
        SIGNAL(toggled(bool)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->invertImageCheck,
        SIGNAL(toggled(bool)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->combFilterCheck,
        SIGNAL(toggled(bool)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->swapCombFilterButton,
        SIGNAL(toggled(bool)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->nonInterlacedRadio,
        SIGNAL(toggled(bool)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->fieldOneRadio,
        SIGNAL(toggled(bool)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->fieldTwoRadio,
        SIGNAL(toggled(bool)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->linesSpin,
        SIGNAL(valueChanged(int)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->vsyncTrainLengthSpin,
        SIGNAL(valueChanged(int)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->agcTauSpin,
        SIGNAL(valueChanged(qreal)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->slowTrackTauSpin,
        SIGNAL(valueChanged(qreal)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->fastTrackTauSpin,
        SIGNAL(valueChanged(qreal)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->lineLenTauSpin,
        SIGNAL(valueChanged(qreal)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->hsyncSpin,
        SIGNAL(changed(qreal, qreal)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->vsyncSpin,
        SIGNAL(changed(qreal, qreal)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->linePeriodSpin,
        SIGNAL(changed(qreal, qreal)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->hugeErrorSlider,
        SIGNAL(valueChanged(int)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->hsyncErrorRange,
        SIGNAL(rangeChanged(int, int)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->timeTolSlider,
        SIGNAL(valueChanged(int)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->levelTolSlider,
        SIGNAL(valueChanged(int)),
        this,
        SLOT(onTVProcessorUiChanged()));

  connect(
        this->ui->geomTolSlider,
        SIGNAL(valueChanged(int)),
        this,
        SLOT(onTVProcessorUiChanged()));
}

void
InspectorUI::emitTVProcessorParameters(void)
{
  if (this->getBaudRate() > 0) {
    struct sigutils_tv_processor_params params;

    this->refreshTVProcessorParametersUiState();
    this->parseTVProcessorParametersUi(params);

    emit tvProcessorParams(params);
    if (this->tvProcessing)
      emit startTVProcessor();
    this->ui->enableTvButton->setEnabled(true);

    this->ui->tvDisplay->setPicGeometry(
          static_cast<int>(params.line_len),
          static_cast<int>(params.frame_lines));
  } else {
    this->ui->enableTvButton->setEnabled(false);
    if (this->tvProcessing) {
      emit stopTVProcessor();
      this->tvProcessing = false;
      this->ui->enableTvButton->setChecked(false);
    }
  }
}

void
InspectorUI::refreshTVProcessorParametersUiState(void)
{
  bool customParamsEnabled = this->ui->customRadio->isChecked();

  if (this->getBaudRate() > 0) {
    this->ui->linePeriodSpin->setSampleRate(this->getBaudRate());
    this->ui->hsyncSpin->setSampleRate(this->getBaudRate());
    this->ui->vsyncSpin->setSampleRate(this->getBaudRate());
  }

  this->ui->geometryGroup->setEnabled(customParamsEnabled);
  this->ui->agcCheck->setEnabled(customParamsEnabled);
  this->ui->syncTab->setEnabled(customParamsEnabled);
  this->ui->invertImageCheck->setEnabled(customParamsEnabled);

  if (!customParamsEnabled)
    this->ui->invertImageCheck->setChecked(
        this->ui->invertSyncCheck->isChecked());

  this->ui->swapCombFilterButton->setEnabled(
        this->ui->combFilterCheck->isChecked());

  this->ui->hsyncSpin->setEnabled(this->ui->enableSyncCheck->isChecked());
  this->ui->vsyncSpin->setEnabled(this->ui->enableSyncCheck->isChecked());

  this->ui->levelTolLabel->setText(
        QString::number(
          100 * std::pow(10., this->ui->levelTolSlider->value() / 100.))
        + "%");

  this->ui->timeTolLabel->setText(
        QString::number(
          100 * std::pow(10., this->ui->timeTolSlider->value() / 100.))
        + "%");

  this->ui->geomTolLabel->setText(
        QString::number(
          100 * std::pow(10., this->ui->geomTolSlider->value() / 100.))
        + "%");

  this->ui->hsyncErrorRangeLabel->setText(
        QString::number(
          100 * std::pow(
            10.,
            this->ui->hsyncErrorRange->minimumPosition() / 100.))
        + "% - "
        + QString::number(
          100 * std::pow(
            10.,
            this->ui->hsyncErrorRange->maximumPosition() / 100.))
        + "%");

  this->ui->hugeErrorLabel->setText(
        QString::number(
          100 * std::pow(10., this->ui->hugeErrorSlider->value() / 100.))
        + "%");

  this->ui->frameRateSpin->setValue(
        1. / (this->ui->linesSpin->value()
              * this->ui->linePeriodSpin->timeValue()));
}

void
InspectorUI::refreshTVProcessorParametersUi(
    struct sigutils_tv_processor_params const &params)
{
  bool oldState = this->editingTVProcessorParams;
  this->editingTVProcessorParams = true;

  this->ui->combFilterCheck->setChecked(params.enable_comb);
  this->ui->agcCheck->setChecked(params.enable_agc);
  this->ui->enableSyncCheck->setChecked(params.enable_sync);
  this->ui->invertImageCheck->setChecked(params.reverse);
  this->ui->swapCombFilterButton->setChecked(params.comb_reverse);

  if (!params.interlace) {
    this->ui->nonInterlacedRadio->setChecked(true);
  } else if (params.dominance) {
    this->ui->fieldOneRadio->setChecked(true);
  } else {
    this->ui->fieldTwoRadio->setChecked(true);
  }

  refreshTVProcessorParametersUiState();

  this->ui->linesSpin->setValue(static_cast<int>(params.frame_lines));
  this->ui->hsyncSpin->setSamplesValue(
        static_cast<qreal>(params.hsync_len));
  this->ui->vsyncSpin->setSamplesValue(
        static_cast<qreal>(params.vsync_len));
  this->ui->linePeriodSpin->setSamplesValue(
        static_cast<qreal>(params.line_len));

  this->ui->hsyncSpin->setBestUnits(true);
  this->ui->vsyncSpin->setBestUnits(true);
  this->ui->linePeriodSpin->setBestUnits(true);

  this->ui->timeTolSlider->setValue(
        static_cast<int>(100 * std::log10(params.t_tol)));
  this->ui->levelTolSlider->setValue(
        static_cast<int>(100 * std::log10(params.l_tol)));
  this->ui->geomTolSlider->setValue(
        static_cast<int>(100 * std::log10(params.g_tol)));

  this->ui->hsyncErrorRange->setMinimumPosition(
        static_cast<int>(100 * std::log10(params.hsync_min_err)));
  this->ui->hsyncErrorRange->setMaximumPosition(
        static_cast<int>(100 * std::log10(params.hsync_max_err)));
  this->ui->hugeErrorSlider->setValue(
        static_cast<int>(100 * std::log10(params.hsync_huge_err)));
  this->ui->vsyncTrainLengthSpin->setValue(
        static_cast<int>(params.vsync_odd_trigger));

  this->ui->agcTauSpin->setValue(
        static_cast<qreal>(params.agc_tau));
  this->ui->lineLenTauSpin->setValue(
        static_cast<qreal>(params.line_len_tau));
  this->ui->fastTrackTauSpin->setValue(
        static_cast<qreal>(params.hsync_fast_track_tau));
  this->ui->slowTrackTauSpin->setValue(
        static_cast<qreal>(params.hsync_slow_track_tau));

  this->editingTVProcessorParams = oldState;
}

bool
InspectorUI::parseTVProcessorParametersUi(
    struct sigutils_tv_processor_params &params)
{
  params.enable_sync  = this->ui->enableSyncCheck->isChecked();
  params.reverse      = this->ui->invertImageCheck->isChecked();
  params.interlace    = !this->ui->nonInterlacedRadio->isChecked();
  params.dominance    = !this->ui->fieldTwoRadio->isChecked();
  params.enable_agc   = this->ui->agcCheck->isChecked();
  params.frame_lines  = static_cast<SUSCOUNT>(this->ui->linesSpin->value());
  params.enable_comb  = this->ui->combFilterCheck->isChecked();
  params.comb_reverse = this->ui->swapCombFilterButton->isChecked();
  params.x_off        = 0;
  params.hsync_len    = static_cast<SUFLOAT>(
        this->ui->hsyncSpin->samplesValue());
  params.vsync_len    = static_cast<SUFLOAT>(
        this->ui->vsyncSpin->samplesValue());
  params.line_len     = static_cast<SUFLOAT>(
        this->ui->linePeriodSpin->samplesValue());
  params.t_tol        = static_cast<SUFLOAT>(
        std::pow(10., this->ui->timeTolSlider->value() / 100.));
  params.l_tol        = static_cast<SUFLOAT>(
        std::pow(10., this->ui->levelTolSlider->value() / 100.));
  params.g_tol        = static_cast<SUFLOAT>(
        std::pow(10., this->ui->geomTolSlider->value() / 100.));

  params.hsync_huge_err = static_cast<SUFLOAT>(
        std::pow(10., this->ui->hugeErrorSlider->value() / 100.));
  params.hsync_min_err  = static_cast<SUFLOAT>(
        std::pow(10., this->ui->hsyncErrorRange->minimumPosition() / 100.));
  params.hsync_max_err  = static_cast<SUFLOAT>(
        std::pow(10., this->ui->hsyncErrorRange->maximumPosition() / 100.));
  params.vsync_odd_trigger  = static_cast<SUSCOUNT>(
        this->ui->vsyncTrainLengthSpin->value());

  params.hsync_len_tau = 9.5;

  params.line_len_tau  = static_cast<SUFLOAT>(
        this->ui->lineLenTauSpin->value());
  params.hsync_fast_track_tau  = static_cast<SUFLOAT>(
        this->ui->fastTrackTauSpin->value());
  params.hsync_slow_track_tau  = static_cast<SUFLOAT>(
        this->ui->slowTrackTauSpin->value());
  params.agc_tau  = static_cast<SUFLOAT>(
        this->ui->agcTauSpin->value());

  return true;
}

void
InspectorUI::onTVProcessorUiChanged(void)
{
  if (!this->editingTVProcessorParams) {
    if (this->ui->ntscRadio->isChecked()) {
      struct sigutils_tv_processor_params params;
      su_tv_processor_params_ntsc(&params, this->getBaudRateFloat());
      refreshTVProcessorParametersUi(params);
    } else if (this->ui->palRadio->isChecked()) {
      struct sigutils_tv_processor_params params;
      su_tv_processor_params_pal(&params, this->getBaudRateFloat());
      refreshTVProcessorParametersUi(params);
    }

    this->emitTVProcessorParameters();
  }

  this->refreshTVProcessorParametersUiState();
}

void
InspectorUI::onToggleTVProcessor(void)
{
  this->tvProcessing = this->ui->enableTvButton->isChecked();

  if (this->tvProcessing)
    emit startTVProcessor();
  else
    emit stopTVProcessor();
}

void
InspectorUI::onTVProcessorFrame(struct sigutils_tv_frame_buffer *frame)
{
  this->tvWorker->acknowledgeFrame();
  this->ui->tvDisplay->putFrame(frame);
  this->ui->tvDisplay->invalidate();
  emit tvProcessorDisposeFrame(frame);
}

void
InspectorUI::onTVProcessorParamsChanged(
    struct sigutils_tv_processor_params params)
{
  this->refreshTVProcessorParametersUi(params);
}

void
InspectorUI::onTVProcessorError(QString error)
{
  QMessageBox::critical(this->ui->tvDisplay, "TV Processor error", error);
}
