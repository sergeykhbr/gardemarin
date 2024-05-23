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

#include "dlgvideosettings.h"
#include <QAudioDevice>
#include <QAudioInput>
#include <QCamera>
#include <QCameraDevice>
#include <QMediaCaptureSession>
#include <QMediaFormat>
#include <QMediaRecorder>
#include <QDebug>
#include <QTextStream>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <qevent.h>

static QString toFormattedString(const QCameraFormat &cameraFormat)
{
    QString string;
    QTextStream str(&string);
    str << QVideoFrameFormat::pixelFormatToString(cameraFormat.pixelFormat())
        << ' ' << cameraFormat.resolution().width()
        << 'x' << cameraFormat.resolution().height()
        << ' ' << cameraFormat.minFrameRate()
        << '-' << cameraFormat.maxFrameRate() << "FPS";
    return string;
}

DialogVideoSettings::DialogVideoSettings(QMediaRecorder *mediaRecorder)
    : QDialog(nullptr),
    mediaRecorder(mediaRecorder)
{
    QGridLayout *gridLayout_3 = new QGridLayout();
    setLayout(gridLayout_3);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

    QGroupBox *groupBox_2 = new QGroupBox(this);
    QGridLayout *gridLayout_2 = new QGridLayout();
    groupBox_2->setLayout(gridLayout_2);
    groupBox_2->setTitle(tr("Video"));

    QLabel *label_8 = new QLabel();
    label_8->setText(tr("Camera Format:"));

    videoCodecBox = new QComboBox();

    QLabel *label_9 = new QLabel();
    label_9->setText(tr("Framerate:"));

    QLabel *label_6 = new QLabel();
    label_6->setText(tr("Video Codec:"));

    videoFormatBox = new QComboBox();

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    fpsSpinBox = new QSpinBox(this);
    fpsSlider = new QSlider(this);
    horizontalLayout->addWidget(fpsSpinBox);
    horizontalLayout->addWidget(fpsSlider);

    gridLayout_2->addWidget(label_8, 0, 0, 1, 2);
    gridLayout_2->addWidget(videoFormatBox, 1, 0, 1, 2);
    gridLayout_2->addWidget(label_9, 2, 0, 1, 2);
    gridLayout_2->addLayout(horizontalLayout, 3, 0, 1, 2);
    gridLayout_2->addWidget(label_6, 4, 0, 1, 2);
    gridLayout_2->addWidget(videoCodecBox, 5, 0, 1, 2);

    QWidget *widget = new QWidget(this);
    widget->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
    QVBoxLayout *verticalLayout_3 = new QVBoxLayout(this);
    verticalLayout_3->setContentsMargins(QMargins(0, 0, 0, 0));
    widget->setLayout(verticalLayout_3);

    QGroupBox *groupBox_3 = new QGroupBox(this);
    groupBox_3->setTitle(tr("Audio"));
    QVBoxLayout *verticalLayout_2 = new QVBoxLayout(groupBox_3);
    groupBox_3->setLayout(verticalLayout_2);

    QLabel *label_2 = new QLabel();
    label_2->setText(tr("Audio Codec:"));

    audioCodecBox = new QComboBox();

    QLabel *label_5 = new QLabel();
    label_5->setText(tr("Sample Rate:"));

    audioSampleRateBox = new QSpinBox();

    verticalLayout_2->addWidget(label_2);
    verticalLayout_2->addWidget(audioCodecBox);
    verticalLayout_2->addWidget(label_5);
    verticalLayout_2->addWidget(audioSampleRateBox);

    QGroupBox *groupBox = new QGroupBox(this);
    QVBoxLayout *verticalLayout = new QVBoxLayout(groupBox);
    groupBox->setLayout(verticalLayout);

    QLabel *label_3 = new QLabel();
    label_3->setText(tr("Quality:"));

    qualitySlider = new QSlider();
    qualitySlider->setMaximum(4);
    qualitySlider->setOrientation(Qt::Horizontal);

    QLabel *label_4 = new QLabel();
    label_4->setText(tr("File Format::"));

    containerFormatBox = new QComboBox();

    verticalLayout->addWidget(label_3);
    verticalLayout->addWidget(qualitySlider);
    verticalLayout->addWidget(label_4);
    verticalLayout->addWidget(containerFormatBox);


    verticalLayout_3->addWidget(groupBox_3);
    verticalLayout_3->addWidget(groupBox);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40);

    gridLayout_3->addWidget(groupBox_2, 0, 0, 1, 2);
    gridLayout_3->addWidget(widget, 2, 0);
    gridLayout_3->addItem(verticalSpacer, 3, 0);
    gridLayout_3->addWidget(buttonBox, 4, 1);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &DialogVideoSettings::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DialogVideoSettings::reject);


    // sample rate:
    auto audioDevice = mediaRecorder->captureSession()->audioInput()->device();
    audioSampleRateBox->setRange(audioDevice.minimumSampleRate(),
                                     audioDevice.maximumSampleRate());

    // camera format
    videoFormatBox->addItem(tr("Default camera format"));

    auto *camera = mediaRecorder->captureSession()->camera();
    const QList<QCameraFormat> videoFormats = camera->cameraDevice().videoFormats();

    for (const QCameraFormat &format : videoFormats)
        videoFormatBox->addItem(toFormattedString(format), QVariant::fromValue(format));

    connect(videoFormatBox, &QComboBox::currentIndexChanged, [this](int /*index*/) {
        this->setFpsRange(boxValue(videoFormatBox).value<QCameraFormat>());
    });

    setFpsRange(camera->cameraFormat());

    connect(fpsSlider, &QSlider::valueChanged, fpsSpinBox, &QSpinBox::setValue);
    connect(fpsSpinBox, &QSpinBox::valueChanged, fpsSlider, &QSlider::setValue);

    updateFormatsAndCodecs();
    connect(audioCodecBox, &QComboBox::currentIndexChanged, this,
            &DialogVideoSettings::updateFormatsAndCodecs);
    connect(videoCodecBox, &QComboBox::currentIndexChanged, this,
            &DialogVideoSettings::updateFormatsAndCodecs);
    connect(containerFormatBox, &QComboBox::currentIndexChanged, this,
            &DialogVideoSettings::updateFormatsAndCodecs);

    qualitySlider->setRange(0, int(QMediaRecorder::VeryHighQuality));

    QMediaFormat format = mediaRecorder->mediaFormat();
    selectComboBoxItem(containerFormatBox, QVariant::fromValue(format.fileFormat()));
    selectComboBoxItem(audioCodecBox, QVariant::fromValue(format.audioCodec()));
    selectComboBoxItem(videoCodecBox, QVariant::fromValue(format.videoCodec()));

    qualitySlider->setValue(mediaRecorder->quality());
    audioSampleRateBox->setValue(mediaRecorder->audioSampleRate());
    selectComboBoxItem(videoFormatBox,
                       QVariant::fromValue(camera->cameraFormat()));

    fpsSlider->setValue(mediaRecorder->videoFrameRate());
    fpsSpinBox->setValue(mediaRecorder->videoFrameRate());
}

DialogVideoSettings::~DialogVideoSettings()
{
}

void DialogVideoSettings::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        //retranslateUi(this);
        break;
    default:
        break;
    }
}

void DialogVideoSettings::setFpsRange(const QCameraFormat &format)
{
    fpsSlider->setRange(format.minFrameRate(), format.maxFrameRate());
    fpsSpinBox->setRange(format.minFrameRate(), format.maxFrameRate());
}

void DialogVideoSettings::applySettings()
{
    QMediaFormat format;
    format.setFileFormat(boxValue(containerFormatBox).value<QMediaFormat::FileFormat>());
    format.setAudioCodec(boxValue(audioCodecBox).value<QMediaFormat::AudioCodec>());
    format.setVideoCodec(boxValue(videoCodecBox).value<QMediaFormat::VideoCodec>());

    mediaRecorder->setMediaFormat(format);
    mediaRecorder->setQuality(QMediaRecorder::Quality(qualitySlider->value()));
    mediaRecorder->setAudioSampleRate(audioSampleRateBox->value());

    const auto &cameraFormat = boxValue(videoFormatBox).value<QCameraFormat>();
    mediaRecorder->setVideoResolution(cameraFormat.resolution());
    mediaRecorder->setVideoFrameRate(fpsSlider->value());

    mediaRecorder->captureSession()->camera()->setCameraFormat(cameraFormat);
}

void DialogVideoSettings::updateFormatsAndCodecs()
{
    if (m_updatingFormats)
        return;
    m_updatingFormats = true;

    QMediaFormat format;
    if (containerFormatBox->count())
        format.setFileFormat(boxValue(containerFormatBox).value<QMediaFormat::FileFormat>());
    if (audioCodecBox->count())
        format.setAudioCodec(boxValue(audioCodecBox).value<QMediaFormat::AudioCodec>());
    if (videoCodecBox->count())
        format.setVideoCodec(boxValue(videoCodecBox).value<QMediaFormat::VideoCodec>());

    int currentIndex = 0;
    audioCodecBox->clear();
    audioCodecBox->addItem(tr("Default audio codec"),
                               QVariant::fromValue(QMediaFormat::AudioCodec::Unspecified));
    for (auto codec : format.supportedAudioCodecs(QMediaFormat::Encode)) {
        if (codec == format.audioCodec())
            currentIndex = audioCodecBox->count();
        audioCodecBox->addItem(QMediaFormat::audioCodecDescription(codec),
                                   QVariant::fromValue(codec));
    }
    audioCodecBox->setCurrentIndex(currentIndex);

    currentIndex = 0;
    videoCodecBox->clear();
    videoCodecBox->addItem(tr("Default video codec"),
                               QVariant::fromValue(QMediaFormat::VideoCodec::Unspecified));
    for (auto codec : format.supportedVideoCodecs(QMediaFormat::Encode)) {
        if (codec == format.videoCodec())
            currentIndex = videoCodecBox->count();
        videoCodecBox->addItem(QMediaFormat::videoCodecDescription(codec),
                                   QVariant::fromValue(codec));
    }
    videoCodecBox->setCurrentIndex(currentIndex);

    currentIndex = 0;
    containerFormatBox->clear();
    containerFormatBox->addItem(tr("Default file format"),
                                    QVariant::fromValue(QMediaFormat::UnspecifiedFormat));
    for (auto container : format.supportedFileFormats(QMediaFormat::Encode)) {
        if (container == format.fileFormat())
            currentIndex = containerFormatBox->count();
        containerFormatBox->addItem(QMediaFormat::fileFormatDescription(container),
                                        QVariant::fromValue(container));
    }
    containerFormatBox->setCurrentIndex(currentIndex);

    m_updatingFormats = false;
}

QVariant DialogVideoSettings::boxValue(const QComboBox *box) const
{
    const int idx = box->currentIndex();
    return idx != -1 ? box->itemData(idx) : QVariant{};
}

void DialogVideoSettings::selectComboBoxItem(QComboBox *box, const QVariant &value)
{
    const int idx = box->findData(value);
    if (idx != -1)
        box->setCurrentIndex(idx);
}
