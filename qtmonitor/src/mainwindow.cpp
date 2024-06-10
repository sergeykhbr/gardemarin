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
#include <QApplication>

MainWindow::MainWindow(AttributeType *cfg) :
    QMainWindow(nullptr),
    serial_(new SerialWidget(this, cfg)),
    tabWindow_(new TabWindow(this, serial_, cfg))   
{
    Config_.clone(cfg);

    QSize appsz = qApp->screens()[0]->size();
    if (appsz.width() <= 1280 && appsz.height() <= 600) {
        showFullScreen();
    } else {
        appsz.setWidth(1280);
        appsz.setHeight(600);
        setFixedSize(appsz);
    }
    setWindowIcon(QIcon(":/images/connect.png"));
    setWindowTitle(tr("qtmonitor"));

    setCentralWidget(tabWindow_);


    dialogSerialSettings_ = new DialogSerialSettings(this, serial_->getpPortSettings());

    QStatusBar *statusBar_ = new QStatusBar(this);
    labelStatus_[0] = new QLabel();
    labelStatus_[1] = new QLabel();
    labelStatus_[0]->setFixedWidth(appsz.width() - 100);
    labelStatus_[1]->setFixedWidth(50);
    setStatusBar(statusBar_);
    statusBar_->addWidget(labelStatus_[0]);
    statusBar_->addWidget(labelStatus_[1]);


    connect(serial_, &SerialWidget::signalSerialPortOpened,
            tabWindow_, &TabWindow::slotSerialPortOpened);
    connect(serial_, &SerialWidget::signalSerialPortClosed,
            tabWindow_, &TabWindow::slotSerialPortClosed);
    connect(serial_, &SerialWidget::signalFailed,
            this, &MainWindow::slotSerialError);

    // Output to status bar
    connect(serial_, &SerialWidget::signalTextToStatusBar,
            this, &MainWindow::slotTextToStatusBar);
    connect(tabWindow_, &TabWindow::signalTextToStatusBar,
            this, &MainWindow::slotTextToStatusBar);

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
    serial_->open(QIODevice::ReadWrite);
}



void MainWindow::closeSerialPort() {
    serial_->close();
}

void MainWindow::slotTextToStatusBar(qint32 idx, const QString &message) {
    labelStatus_[idx]->setText(message);
}

void MainWindow::slotSerialError(const QString &message) {
    QMessageBox::warning(this, tr("Warning"), message);
}

