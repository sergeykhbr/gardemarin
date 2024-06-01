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

#include "dlgimagesettings.h"
#include <QCamera>
#include <QMediaCaptureSession>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDialogButtonBox>

#include <QDebug>

DialogImageSettings::DialogImageSettings(QImageCapture *imageCapture)
    : QDialog(nullptr), imagecapture(imageCapture)
{
    setWindowTitle(tr("Image Settings"));

    QGridLayout *gridLayout = new QGridLayout(this);
    setLayout(gridLayout);

    QGroupBox *groupBox_2 = new QGroupBox(this);
    groupBox_2->setTitle(tr("Image"));
    QGridLayout *gridLayout_2 = new QGridLayout(groupBox_2);
    groupBox_2->setLayout(gridLayout_2);

    QLabel *label_8 = new QLabel(groupBox_2);
    label_8->setText(tr("Resolution:"));
    imageResolutionBox = new QComboBox(groupBox_2);

    QLabel *label_6 = new QLabel(groupBox_2);
    label_6->setText(tr("Image Format:"));
    imageCodecBox = new QComboBox(groupBox_2);

    QLabel *label_7 = new QLabel(groupBox_2);
    label_7->setText(tr("Image Quality:"));
    imageQualitySlider = new QSlider(Qt::Horizontal, groupBox_2);
    imageQualitySlider->setMaximum(4);
    
    gridLayout_2->addWidget(label_8, 0, 0, 1, 2);
    gridLayout_2->addWidget(imageResolutionBox, 1, 0, 1, 2);
    gridLayout_2->addWidget(label_6, 2, 0, 1, 2);
    gridLayout_2->addWidget(imageCodecBox, 3, 0, 1, 2);
    gridLayout_2->addWidget(label_7, 4, 0);
    gridLayout_2->addWidget(imageQualitySlider, 4, 1);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 14, QSizePolicy::Preferred, QSizePolicy::Fixed);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

    gridLayout->addWidget(groupBox_2, 0, 0);
    gridLayout->addItem(verticalSpacer, 1, 0);
    gridLayout->addWidget(buttonBox, 2, 0);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &DialogImageSettings::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DialogImageSettings::reject);

    // image codecs
    imageCodecBox->addItem(tr("Default image format"), QVariant(QString()));
    const auto supportedImageFormats = QImageCapture::supportedFormats();
    for (const auto &f : supportedImageFormats) {
        QString description = QImageCapture::fileFormatDescription(f);
        imageCodecBox->addItem(QImageCapture::fileFormatName(f) + ": " + description,
                                   QVariant::fromValue(f));
    }

    imageQualitySlider->setRange(0, int(QImageCapture::VeryHighQuality));

    imageResolutionBox->addItem(tr("Default Resolution"));
    const QList<QSize> supportedResolutions =
            imagecapture->captureSession()->camera()->cameraDevice().photoResolutions();
    for (const QSize &resolution : supportedResolutions) {
        imageResolutionBox->addItem(
                QString::asprintf("%dx%d", resolution.width(), resolution.height()));
    }

    selectComboBoxItem(imageCodecBox, QVariant::fromValue(imagecapture->fileFormat()));
    selectComboBoxItem(imageResolutionBox, QVariant(imagecapture->resolution()));
    imageQualitySlider->setValue(imagecapture->quality());
}

void DialogImageSettings::applyImageSettings() const
{
    imagecapture->setFileFormat(boxValue(imageCodecBox).value<QImageCapture::FileFormat>());
    imagecapture->setQuality(QImageCapture::Quality(imageQualitySlider->value()));
    imagecapture->setResolution(boxValue(imageResolutionBox).toSize());
}

QVariant DialogImageSettings::boxValue(const QComboBox *box) const
{
    const int idx = box->currentIndex();
    return idx != -1 ? box->itemData(idx) : QVariant{};
}

void DialogImageSettings::selectComboBoxItem(QComboBox *box, const QVariant &value)
{
    const int idx = box->findData(value);
    if (idx != -1) {
        box->setCurrentIndex(idx);
    }
}
