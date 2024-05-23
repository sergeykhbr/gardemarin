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

#include <QAudioInput>
#include <QCamera>
#include <QImageCapture>
#include <QMediaCaptureSession>
#include <QMediaDevices>
#include <QMediaMetaData>
#include <QMediaRecorder>
#include <QVideoWidget>
#include <QScopedPointer>
#include <QPushButton>
#include <QStackedWidget>
#include <QSlider>
#include <QTabWidget>
#include <QLabel>
#include "tab_camera_metadata.h"

class TabCamera : public QWidget {
    Q_OBJECT

 public:
    explicit TabCamera(QWidget *parent = nullptr);

 signals:
    void signalTextToStatusBar(qint32 idx, const QString &text);

 public slots:
    void saveMetaData();

 private slots:
    void init();

    void setCamera(const QCameraDevice &cameraDevice);

    void startCamera();
    void stopCamera();

    void record();
    void pause();
    void stop();
    void setMuted(bool);

    void takeImage();
    void displayCaptureError(int, QImageCapture::Error, const QString &errorString);

    void configureCaptureSettings();
    void configureVideoSettings();
    void configureImageSettings();

    void displayRecorderError();
    void displayCameraError();

    void updateCameraDevice(QAction *action);

    void updateCameraActive(bool active);
    void updateCaptureMode();
    void updateRecorderState(QMediaRecorder::RecorderState state);
    void setExposureCompensation(int index);

    void updateRecordTime();

    void processCapturedImage(int requestId, const QImage &img);

    void displayViewfinder();
    void displayCapturedImage();

    void readyForCapture(bool ready);
    void imageSaved(int id, const QString &fileName);

    void updateCameras();

    void showMetaDataDialog();

 protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

 private:
    QActionGroup *videoDevicesGroup = nullptr;

    QMediaDevices m_devices;
    QScopedPointer<QImageCapture> m_imageCapture;
    QMediaCaptureSession m_captureSession;
    QScopedPointer<QCamera> m_camera;
    QScopedPointer<QAudioInput> m_audioInput;
    QScopedPointer<QMediaRecorder> m_mediaRecorder;

    QPushButton *takeImageButton;
    QPushButton *recordButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QPushButton *muteButton;
    QPushButton *metaDataButton;
    QTabWidget *captureWidget;
    QSlider *exposureCompensation;
    QLabel *lastImagePreviewLabel;
    QVideoWidget *viewfinder;
    QStackedWidget *stackedWidget;

    QAction *actionStartCamera;
    QAction *actionStopCamera;
    QAction *actionSettings;


    bool m_isCapturingImage = false;
    bool m_applicationExiting = false;
    bool m_doImageCapture = true;

    MetaDataDialog *m_metaDataDialog = nullptr;
};


