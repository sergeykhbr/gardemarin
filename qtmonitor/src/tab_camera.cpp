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

#include "tab_camera.h"
#include "dlg/dlgvideosettings.h"

#include <QAudioDevice>
#include <QAudioInput>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QMediaFormat>
#include <QMediaMetaData>
#include <QMediaRecorder>

#include <QLineEdit>
#include <QMessageBox>

#include <QAction>
#include <QActionGroup>
#include <QImage>
#include <QKeyEvent>
#include <QPalette>
#include <QGridLayout>
#include <QLabel>
#include <QStackedWidget>

#include <QDir>
#include <QTimer>

#if QT_CONFIG(permissions)
  #include <QPermission>
#endif
TabCamera::TabCamera(QWidget *parent)
    : QWidget(parent) {

    QGridLayout *gridLayout_3 = new QGridLayout(this);
    setLayout(gridLayout_3);
    captureWidget = new QTabWidget();

    QWidget *tab_2 = new QWidget();
    QGridLayout *gridLayout = new QGridLayout();
    tab_2->setLayout(gridLayout);

   
    takeImageButton = new QPushButton("Capture Photo", this);
    takeImageButton->setEnabled(false);

    exposureCompensation = new QSlider(this);
    exposureCompensation->setMinimum(-4);
    exposureCompensation->setMaximum(4);
    exposureCompensation->setPageStep(2);
    exposureCompensation->setOrientation(Qt::Horizontal);
    exposureCompensation->setTickPosition(QSlider::TicksAbove);

    QLabel *label = new QLabel(tr("Exposure Compensation:"));

    gridLayout->addWidget(takeImageButton, 0, 0);
    gridLayout->addWidget(exposureCompensation, 5, 0);
    gridLayout->addWidget(label, 4, 0);

    QWidget *tab = new QWidget(this);
    QGridLayout * gridLayout_2 = new QGridLayout(tab);
    tab->setLayout(gridLayout_2);

    recordButton = new QPushButton(tab);
    recordButton->setText(tr("Record"));
    pauseButton = new QPushButton(tab);
    pauseButton->setText(tr("Pause"));
    stopButton = new QPushButton(tab);
    stopButton->setText(tr("Stop"));
    QSpacerItem * verticalSpacer = new QSpacerItem(20, 76, QSizePolicy::Fixed, QSizePolicy::Fixed);
    muteButton = new QPushButton(tab);
    muteButton->setText(tr("Mute"));
    muteButton->setCheckable(true);
    metaDataButton = new QPushButton(tab);
    metaDataButton->setText(tr("metadata"));
    metaDataButton->setCheckable(true);

    gridLayout_2->addWidget(recordButton, 0, 0);
    gridLayout_2->addWidget(pauseButton, 1, 0);
    gridLayout_2->addWidget(stopButton, 2, 0);
    //gridLayout_2->addWidget(verticalSpacer, 3, 0);
    gridLayout_2->addWidget(muteButton, 4, 0);
    gridLayout_2->addWidget(metaDataButton, 5, 0);

    captureWidget->addTab(tab_2, "Image");
    captureWidget->addTab(tab, "Video");

    stackedWidget = new QStackedWidget(this);
    stackedWidget->setSizePolicy(QSizePolicy::Policy::Preferred, QSizePolicy::Policy::Preferred);
    QWidget *viewFinderPage = new QWidget(this);
    QGridLayout *gridLayout_5 = new QGridLayout();
    viewFinderPage->setLayout(gridLayout_5);

    viewfinder = new QVideoWidget();
    gridLayout_5->addWidget(viewfinder, 0, 0);
        
    QWidget *previewPage = new QWidget(this);
    QGridLayout *gridLayout_4 = new QGridLayout();
    previewPage->setLayout(gridLayout_4);

    lastImagePreviewLabel = new QLabel();
    lastImagePreviewLabel->setSizePolicy(QSizePolicy::Policy::Preferred,
                                         QSizePolicy::Policy::MinimumExpanding);
    lastImagePreviewLabel->setText("");
    lastImagePreviewLabel->setFrameShape(QFrame::Box);
    gridLayout_4->addWidget(lastImagePreviewLabel, 0, 0);

    stackedWidget->addWidget(viewFinderPage);
    stackedWidget->addWidget(previewPage);

    gridLayout_3->addWidget(stackedWidget, 0, 0);
    gridLayout_3->addWidget(captureWidget, 0, 1);
    gridLayout_3->setColumnStretch(0, 3);


    actionStartCamera = new QAction(tr("Start Camera"), this);
    actionStopCamera = new QAction(tr("Stop Camera"), this);
    actionSettings = new QAction(tr("Cfg Camera"), this);

    connect(actionStartCamera, &QAction::triggered, this, &TabCamera::startCamera);
    connect(actionStopCamera, &QAction::triggered, this, &TabCamera::stopCamera);
    connect(actionSettings, &QAction::triggered, this, &TabCamera::configureCaptureSettings);

    connect(recordButton, &QPushButton::clicked, this, &TabCamera::record);
    connect(stopButton, &QPushButton::clicked, this, &TabCamera::stop);
    connect(pauseButton, &QPushButton::clicked, this, &TabCamera::pause);
    connect(takeImageButton, &QPushButton::clicked, this, &TabCamera::takeImage);

    // disable all buttons by default
    updateCameraActive(false);
    readyForCapture(false);
    recordButton->setEnabled(false);
    pauseButton->setEnabled(false);
    stopButton->setEnabled(false);
    metaDataButton->setEnabled(false);

    // try to actually initialize camera & mic
    init();
}

void TabCamera::init() {
#if QT_CONFIG(permissions)
    // camera
    QCameraPermission cameraPermission;
    switch (qApp->checkPermission(cameraPermission)) {
    case Qt::PermissionStatus::Undetermined:
        qApp->requestPermission(cameraPermission, this, &TabCamera::init);
        return;
    case Qt::PermissionStatus::Denied:
        qWarning("Camera permission is not granted!");
        return;
    case Qt::PermissionStatus::Granted:
        break;
    }
    // microphone
    QMicrophonePermission microphonePermission;
    switch (qApp->checkPermission(microphonePermission)) {
    case Qt::PermissionStatus::Undetermined:
        qApp->requestPermission(microphonePermission, this, &TabCamera::init);
        return;
    case Qt::PermissionStatus::Denied:
        qWarning("Microphone permission is not granted!");
        return;
    case Qt::PermissionStatus::Granted:
        break;
    }
#endif

    m_audioInput.reset(new QAudioInput);
    m_captureSession.setAudioInput(m_audioInput.get());

    // Camera devices:

    videoDevicesGroup = new QActionGroup(this);
    videoDevicesGroup->setExclusive(true);
    updateCameras();
    connect(&m_devices, &QMediaDevices::videoInputsChanged, this, &TabCamera::updateCameras);

    connect(videoDevicesGroup, &QActionGroup::triggered, this, &TabCamera::updateCameraDevice);
    connect(captureWidget, &QTabWidget::currentChanged, this, &TabCamera::updateCaptureMode);

    connect(metaDataButton, &QPushButton::clicked, this, &TabCamera::showMetaDataDialog);
    connect(exposureCompensation, &QAbstractSlider::valueChanged, this,
            &TabCamera::setExposureCompensation);

    setCamera(QMediaDevices::defaultVideoInput());
}

void TabCamera::setCamera(const QCameraDevice &cameraDevice) {
    m_camera.reset(new QCamera(cameraDevice));
    m_captureSession.setCamera(m_camera.data());

    connect(m_camera.data(), &QCamera::activeChanged, this, &TabCamera::updateCameraActive);
    connect(m_camera.data(), &QCamera::errorOccurred, this, &TabCamera::displayCameraError);

    if (!m_mediaRecorder) {
        m_mediaRecorder.reset(new QMediaRecorder);
        m_captureSession.setRecorder(m_mediaRecorder.data());
        connect(m_mediaRecorder.data(), &QMediaRecorder::recorderStateChanged, this,
                &TabCamera::updateRecorderState);
        connect(m_mediaRecorder.data(), &QMediaRecorder::durationChanged, this,
                &TabCamera::updateRecordTime);
        connect(m_mediaRecorder.data(), &QMediaRecorder::errorChanged, this,
                &TabCamera::displayRecorderError);
    }

    if (!m_imageCapture) {
        m_imageCapture.reset(new QImageCapture);
        m_captureSession.setImageCapture(m_imageCapture.get());
        connect(m_imageCapture.get(), &QImageCapture::readyForCaptureChanged, this,
                &TabCamera::readyForCapture);
        connect(m_imageCapture.get(), &QImageCapture::imageCaptured, this,
                &TabCamera::processCapturedImage);
        connect(m_imageCapture.get(), &QImageCapture::imageSaved, this, &TabCamera::imageSaved);
        connect(m_imageCapture.get(), &QImageCapture::errorOccurred, this,
                &TabCamera::displayCaptureError);
    }

    m_captureSession.setVideoOutput(viewfinder);

    updateCameraActive(m_camera->isActive());
    updateRecorderState(m_mediaRecorder->recorderState());
    readyForCapture(m_imageCapture->isReadyForCapture());

    updateCaptureMode();

    m_camera->start();
}

void TabCamera::keyPressEvent(QKeyEvent *event) {
    if (event->isAutoRepeat())
        return;

    switch (event->key()) {
    case Qt::Key_CameraFocus:
        displayViewfinder();
        event->accept();
        break;
    case Qt::Key_Camera:
        if (m_doImageCapture) {
            takeImage();
        } else {
            if (m_mediaRecorder->recorderState() == QMediaRecorder::RecordingState)
                stop();
            else
                record();
        }
        event->accept();
        break;
    default:
        QWidget::keyPressEvent(event);
    }
}

void TabCamera::updateRecordTime() {
    QString str = tr("Recorded %1 sec").arg(m_mediaRecorder->duration() / 1000);
    emit signalTextToStatusBar(0, str);
}

void TabCamera::processCapturedImage(int requestId, const QImage &img) {
    Q_UNUSED(requestId);
    QImage scaledImage =
            img.scaled(viewfinder->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

    lastImagePreviewLabel->setPixmap(QPixmap::fromImage(scaledImage));

    // Display captured image for 4 seconds.
    displayCapturedImage();
    QTimer::singleShot(4000, this, &TabCamera::displayViewfinder);
}

void TabCamera::configureCaptureSettings() {
    if (m_doImageCapture) {
        configureImageSettings();
    } else {
        configureVideoSettings();
    }
}

void TabCamera::configureVideoSettings() {
    DialogVideoSettings settingsDialog(m_mediaRecorder.data());

    if (settingsDialog.exec()) {
        settingsDialog.applySettings();
    }
}

void TabCamera::configureImageSettings() {
#if 0
    ImageSettings settingsDialog(m_imageCapture.get());

    if (settingsDialog.exec() == QDialog::Accepted)
        settingsDialog.applyImageSettings();
#endif
}

void TabCamera::record() {
    m_mediaRecorder->record();
    updateRecordTime();
}

void TabCamera::pause() {
    m_mediaRecorder->pause();
}

void TabCamera::stop() {
    m_mediaRecorder->stop();
}

void TabCamera::setMuted(bool muted) {
    m_captureSession.audioInput()->setMuted(muted);
}

void TabCamera::takeImage() {
    m_isCapturingImage = true;
    m_imageCapture->captureToFile();
}

void TabCamera::displayCaptureError(int id, const QImageCapture::Error error,
                                    const QString &errorString) {
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::warning(this, tr("Image Capture Error"), errorString);
    m_isCapturingImage = false;
}

void TabCamera::startCamera() {
    m_camera->start();
}

void TabCamera::stopCamera() {
    m_camera->stop();
}

void TabCamera::updateCaptureMode() {
    int tabIndex = captureWidget->currentIndex();
    m_doImageCapture = (tabIndex == 0);
}

void TabCamera::updateCameraActive(bool active) {
    if (active) {
        actionStartCamera->setEnabled(false);
        actionStopCamera->setEnabled(true);
        captureWidget->setEnabled(true);
        actionSettings->setEnabled(true);
    } else {
        actionStartCamera->setEnabled(true);
        actionStopCamera->setEnabled(false);
        captureWidget->setEnabled(false);
        actionSettings->setEnabled(false);
    }
}

void TabCamera::updateRecorderState(QMediaRecorder::RecorderState state) {
    switch (state) {
    case QMediaRecorder::StoppedState:
        recordButton->setEnabled(true);
        pauseButton->setEnabled(true);
        stopButton->setEnabled(false);
        metaDataButton->setEnabled(true);
        break;
    case QMediaRecorder::PausedState:
        recordButton->setEnabled(true);
        pauseButton->setEnabled(false);
        stopButton->setEnabled(true);
        metaDataButton->setEnabled(false);
        break;
    case QMediaRecorder::RecordingState:
        recordButton->setEnabled(false);
        pauseButton->setEnabled(true);
        stopButton->setEnabled(true);
        metaDataButton->setEnabled(false);
        break;
    }
}

void TabCamera::setExposureCompensation(int index) {
    m_camera->setExposureCompensation(index * 0.5);
}

void TabCamera::displayRecorderError() {
    if (m_mediaRecorder->error() != QMediaRecorder::NoError)
        QMessageBox::warning(this, tr("Capture Error"), m_mediaRecorder->errorString());
}

void TabCamera::displayCameraError() {
    if (m_camera->error() != QCamera::NoError)
        QMessageBox::warning(this, tr("Camera Error"), m_camera->errorString());
}

void TabCamera::updateCameraDevice(QAction *action) {
    setCamera(qvariant_cast<QCameraDevice>(action->data()));
}

void TabCamera::displayViewfinder() {
    stackedWidget->setCurrentIndex(0);
}

void TabCamera::displayCapturedImage() {
    stackedWidget->setCurrentIndex(1);
}

void TabCamera::readyForCapture(bool ready) {
    takeImageButton->setEnabled(ready);
}

void TabCamera::imageSaved(int id, const QString &fileName) {
    Q_UNUSED(id);

    QString text = tr("Captured \"%1\"").arg(QDir::toNativeSeparators(fileName));
    emit signalTextToStatusBar(0, text);

    m_isCapturingImage = false;
    if (m_applicationExiting) {
        close();
    }
}

void TabCamera::closeEvent(QCloseEvent *event) {
    if (m_isCapturingImage) {
        setEnabled(false);
        m_applicationExiting = true;
        event->ignore();
    } else {
        event->accept();
    }
}

void TabCamera::updateCameras()
{
//    ui->menuDevices->clear();
    const QList<QCameraDevice> availableCameras = QMediaDevices::videoInputs();
    for (const QCameraDevice &cameraDevice : availableCameras) {
        QAction *videoDeviceAction = new QAction(cameraDevice.description(), videoDevicesGroup);
        videoDeviceAction->setCheckable(true);
        videoDeviceAction->setData(QVariant::fromValue(cameraDevice));
        if (cameraDevice == QMediaDevices::defaultVideoInput())
            videoDeviceAction->setChecked(true);

//        ui->menuDevices->addAction(videoDeviceAction);
    }
}

void TabCamera::showMetaDataDialog() {
    if (!m_metaDataDialog)
        m_metaDataDialog = new MetaDataDialog(this);
    m_metaDataDialog->setAttribute(Qt::WA_DeleteOnClose, false);
    if (m_metaDataDialog->exec() == QDialog::Accepted)
        saveMetaData();
}

void TabCamera::saveMetaData() {
    QMediaMetaData data;
    for (int i = 0; i < QMediaMetaData::NumMetaData; i++) {
        QString val = m_metaDataDialog->m_metaDataFields[i]->text();
        if (!val.isEmpty()) {
            auto key = static_cast<QMediaMetaData::Key>(i);
            if (i == QMediaMetaData::CoverArtImage) {
                QImage coverArt(val);
                data.insert(key, coverArt);
            } else if (i == QMediaMetaData::ThumbnailImage) {
                QImage thumbnail(val);
                data.insert(key, thumbnail);
            } else if (i == QMediaMetaData::Date) {
                QDateTime date = QDateTime::fromString(val);
                data.insert(key, date);
            } else {
                data.insert(key, val);
            }
        }
    }
    m_mediaRecorder->setMetaData(data);
}
