//
//    Misc/ExportSamplesTask.cpp: Description
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
#include <ExportSamplesTask.h>
#include <QCoreApplication>

using namespace SigDigger;

#define SIGDIGGER_EXPORT_SAMPLES_BREATHE_INTERVAL_MS 100
#define SIGDIGGER_EXPORT_SAMPLES_BREATHE_BLOCK_SIZE  0x10000

void
ExportSamplesTask::breathe(quint64 i)
{
  size_t size = this->data.size();

  if (this->timer.elapsed() > SIGDIGGER_EXPORT_SAMPLES_BREATHE_INTERVAL_MS) {
    this->timer.restart();
    emit progress(
        static_cast<qreal>(i) / static_cast<qreal>(size - 1),
        "Saving data");
    QCoreApplication::processEvents();
  }
}

bool
ExportSamplesTask::exportToMatlab(void)
{
  size_t size = this->data.size();

  of << "%\n";
  of << "% Time domain capture file generated by SigDigger\n";
  of << "%\n\n";

  of << "sampleRate = " << this->fs << ";\n";
  of << "deltaT = " << 1 / this->fs << ";\n";
  of << "X = [ ";

  of << std::setprecision(std::numeric_limits<float>::digits10);


  for (size_t i = 0; !this->cancelFlag && i < size; ++i) {
    of
       << SU_C_REAL(this->data[i]) << " + "
       << SU_C_IMAG(this->data[i]) << "i, ";

    if (i % SIGDIGGER_EXPORT_SAMPLES_BREATHE_BLOCK_SIZE == 0)
      this->breathe(i);
  }

  of << "];\n";

  return true;
}

bool
ExportSamplesTask::exportToMat5(void)
{
  size_t size = this->data.size();
  bool ok = false;

  for (size_t i = 0; !this->cancelFlag && i < size; ++i) {
    SU_TRYCATCH(
          su_mat_file_stream_col(
            this->mf,
            SU_C_REAL(this->data[i]),
            SU_C_IMAG(this->data[i])),
          goto done);

    if (i % SIGDIGGER_EXPORT_SAMPLES_BREATHE_BLOCK_SIZE == 0) {
      SU_TRYCATCH(su_mat_file_flush(mf), goto done);
      this->breathe(i);
    }
  }

  SU_TRYCATCH(su_mat_file_flush(mf), goto done);

  ok = true;

done:
  if (!ok)
    emit error(
        "Cannot save data to Mat5 file "
        + this->path
        + ". See error log for details.");

  return ok;
}

bool
ExportSamplesTask::exportToWav(void)
{
  size_t size = this->data.size();
  bool ok = false;
  size_t i = 0;
  size_t amount;

  for (
       i = 0;
       !this->cancelFlag
         && i < size - SIGDIGGER_EXPORT_SAMPLES_BREATHE_BLOCK_SIZE;
       i += SIGDIGGER_EXPORT_SAMPLES_BREATHE_BLOCK_SIZE) {

    amount = size - i;
    if (amount > SIGDIGGER_EXPORT_SAMPLES_BREATHE_BLOCK_SIZE)
      amount = SIGDIGGER_EXPORT_SAMPLES_BREATHE_BLOCK_SIZE;

    if (sf_write_float(
          this->sfp,
          reinterpret_cast<const SUFLOAT *>(this->data.data() + i),
          2 * amount)
        != 2 * static_cast<sf_count_t>(amount))
        goto done;

    this->breathe(i);
  }

  if (i < size)
    if (sf_write_float(
        this->sfp,
        reinterpret_cast<const SUFLOAT *>(this->data.data() + i),
        2 * static_cast<sf_count_t>(size - i)) !=
        2 * static_cast<sf_count_t>(size - i))
      goto done;

  ok = true;

done:
  if (!ok)
    emit error(
        "Cannot save data to WAV file "
        + this->path
        + ": "
        + QString(sf_strerror(this->sfp)));

  return ok;
}

bool
ExportSamplesTask::work(void)
{
  bool ok = false;

  this->timer.start();

  if (this->format == "mat")
    ok = this->exportToMat5();
  else if (this->format == "m")
    ok = this->exportToMatlab();
  else if (this->format == "wav")
    ok = this->exportToWav();
  else
    emit error("Unsupported data format " + this->format);

  if (ok) {
    if (this->cancelFlag)
      emit cancelled();
    else
      emit done();
  }

  return false;
}

void
ExportSamplesTask::cancel(void)
{
  this->cancelFlag = true;
}

bool
ExportSamplesTask::openMat5(void)
{
  su_mat_matrix_t *mtx = nullptr;
  bool ok = false;

  SU_TRYCATCH(this->mf = su_mat_file_new(), goto done);

  SU_TRYCATCH(
        mtx = su_mat_file_make_matrix(this->mf, "sampleRate", 1, 1),
        goto done);
  SU_TRYCATCH(su_mat_matrix_write_col(mtx, fs), goto done);

  SU_TRYCATCH(
        mtx = su_mat_file_make_matrix(this->mf, "deltaT", 1, 1),
        goto done);
  SU_TRYCATCH(su_mat_matrix_write_col(mtx, 1 / fs), goto done);

  SU_TRYCATCH(
        mtx = su_mat_file_make_streaming_matrix(this->mf, "X", 2, 0),
        goto done);
  SU_TRYCATCH(su_mat_file_dump(mf, path.toStdString().c_str()), goto done);

  ok = true;

done:
  if (!ok)
    this->lastError =
          "Cannot create Mat5 file "
          + this->path
          + ". See error log for details.";

  return ok;
}

QString
ExportSamplesTask::getLastError(void) const
{
  return this->lastError;
}

bool
ExportSamplesTask::openMatlab(void)
{
  this->of = std::ofstream(path.toStdString().c_str(), std::ofstream::binary);

  if (!of.is_open()) {
    this->lastError =
        "Cannot open "
        + this->path
        + ": "
        + QString(strerror(errno));
    return false;
  }

  return true;
}

bool
ExportSamplesTask::openWav(void)
{
  SF_INFO sfinfo;

  sfinfo.channels = 2;
  sfinfo.samplerate = static_cast<int>(fs);
  sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;

  if ((this->sfp = sf_open(
         path.toStdString().c_str(),
         SFM_WRITE,
         &sfinfo)) == nullptr) {
    this->lastError =
        "Cannot open "
        + this->path
        + ": "
        + QString(strerror(errno));
    return false;
  }

  return true;
}

bool
ExportSamplesTask::attemptOpen(void)
{
  if (this->format == "mat")
    return this->openMat5();
  else if (this->format == "m")
    return this->openMatlab();
  else if (this->format == "wav")
    return this->openWav();

  this->lastError = "Unsupported format \"" + this->lastError + "\"";

  return false;
}

ExportSamplesTask::~ExportSamplesTask(void)
{
  if (this->sfp != nullptr)
    sf_close(this->sfp);

  if (this->mf != nullptr)
    su_mat_file_destroy(this->mf);
}

ExportSamplesTask::ExportSamplesTask(
    QString const &path,
    QString const &format,
    const SUCOMPLEX *data,
    size_t length,
    qreal fs,
    int start,
    int end)
{
  if (start < 0)
    start = 0;
  if (end > static_cast<int>(length))
    end = static_cast<int>(length);

  this->start  = start;
  this->end    = end;
  this->fs     = fs;
  this->path   = path;
  this->format = format;

  this->data.resize(static_cast<unsigned>(end - start));
  this->data.assign(data + start, data + end);
  this->setDataSize(this->data.size());
}
