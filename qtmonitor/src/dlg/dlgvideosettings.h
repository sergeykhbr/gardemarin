/*
 *  Copyright 2024 Sergey Khabarov, sergeykhbr@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#pragma once

#include <QDialog>
#include <QCameraFormat>
#include <QComboBox>
#include <QMediaRecorder>
#include <QComboBox>
#include <QSpinBox>
#include <QSlider>

class DialogVideoSettings : public QDialog
{
    Q_OBJECT

 public:
    explicit DialogVideoSettings(QMediaRecorder *mediaRecorder);
    ~DialogVideoSettings();

    void applySettings();
    void updateFormatsAndCodecs();

 protected:
    void changeEvent(QEvent *e) override;

 private:
    void setFpsRange(const QCameraFormat &format);
    QVariant boxValue(const QComboBox *) const;
    void selectComboBoxItem(QComboBox *box, const QVariant &value);

    QMediaRecorder *mediaRecorder;
    bool m_updatingFormats = false;

    QComboBox *videoFormatBox;
    QComboBox *containerFormatBox;
    QComboBox *videoCodecBox;
    QComboBox *audioCodecBox;
    QSpinBox *fpsSpinBox;
    QSpinBox *audioSampleRateBox;
    QSlider *fpsSlider;
    QSlider *qualitySlider;
};
