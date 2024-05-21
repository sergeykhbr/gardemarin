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

#include "mainwindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QToolBar>
#include <chrono>

MainWindow::MainWindow(AttributeType *cfg) :
    QMainWindow(nullptr),
    serial_(new SerialWidget(this, cfg)),
    tabWindow_(new TabWindow(this, serial_)),
    labelStatus_(new QLabel)
{
    Config_.clone(cfg);

    QSize appsz = qApp->screens()[0]->size();
    if (appsz.width() <= 1200 && appsz.height() <= 600) {
        showFullScreen();
    } else {
        appsz.setWidth(1200);
        appsz.setHeight(600);
        resize(appsz);
    }
    setWindowIcon(QIcon(":/images/connect.png"));
    setWindowTitle(tr("qtmonitor"));

    setCentralWidget(tabWindow_);


    dialogSerialSettings_ = new ComPortSettings(this);

    QStatusBar *statusBar_ = new QStatusBar(this);
    setStatusBar(statusBar_);
    statusBar_->addWidget(labelStatus_);


    connect(this, &MainWindow::signalSerialPortOpened,
            tabWindow_, &TabWindow::slotSerialPortOpened);
    connect(this, &MainWindow::signalSerialPortClosed,
            tabWindow_, &TabWindow::slotSerialPortClosed);
    connect(serial_, &SerialWidget::signalFailed,
            this, &MainWindow::slotSerialError);


    openSerialPort();
}

MainWindow::~MainWindow() {
    delete dialogSerialSettings_;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // TODO: save state
    closeSerialPort();
    event->accept();
}

void MainWindow::openSerialPort() {
    dialogSerialSettings_->exec();
    const ComPortSettings::Settings p = dialogSerialSettings_->settings();

    serial_->setPortName(p.name);
    serial_->setBaudRate(p.baudRate);
    serial_->setDataBits(p.dataBits);
    serial_->setParity(p.parity);
    serial_->setStopBits(p.stopBits);
    serial_->setFlowControl(p.flowControl);
    if (serial_->open(QIODevice::ReadWrite)) {
        emit signalSerialPortOpened(p.localEchoEnabled);
        showStatusMessage(tr("Connected to %1 : %2")
                          .arg(p.name, p.stringBaudRate));
    } else {
        QMessageBox::critical(this, tr("Error"), serial_->errorString());

        showStatusMessage(tr("Open error"));
    }
}



void MainWindow::closeSerialPort() {
    if (serial_->isOpen()) {
        serial_->close();
    }
    emit signalSerialPortClosed();
}

void MainWindow::showStatusMessage(const QString &message) {
    labelStatus_->setText(message);
}

void MainWindow::slotSerialError(const QString &message) {
    QMessageBox::warning(this, tr("Warning"), message);
}

