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

#include "serial.h"

SerialWidget::SerialWidget(QObject *parent) :
    QSerialPort(parent),
    timer_(this) {
    bytesToWrite_ = 0;

    timer_.setSingleShot(true);

    connect(&timer_, &QTimer::timeout, this, &SerialWidget::slotSendTimeout);

    connect(this, &SerialWidget::errorOccurred, this, &SerialWidget::handleError);
    connect(this, &QSerialPort::readyRead, this, &SerialWidget::slotRecvSerialPort);
    connect(this, &QSerialPort::bytesWritten, this, &SerialWidget::slotBytesWritten);
}

void SerialWidget::slotRecvSerialPort() {
    const QByteArray data = readAll();
    emit signalRecvSerialPort(data);

    // search CAN frames over Serial interface
    for (auto &s : data) {
        if (s == '<') {
        }
        if (s == '!') {
        }
    }
}


void SerialWidget::slotBytesWritten(qint64 bytes) {
    bytesToWrite_ -= bytes;
    if (bytesToWrite_ == 0) {
        timer_.stop();
    }
}

void SerialWidget::slotSendSerialPort(const QByteArray &data) {
    const qint64 written = write(data);
    if (written == data.size()) {
        bytesToWrite_ += written;
        timer_.start(std::chrono::seconds{5});
    } else {
        const QString error = tr("Send operation failed for port %1.\n"
                                 "Error: %2").arg(portName(),
                                                  errorString());
        emit signalFailed(error);
    }
}

void SerialWidget::handleError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        //QMessageBox::critical(this, tr("Critical Error"), errorString());
        //closeSerialPort();
        emit signalFailed(errorString());
    }
}


void SerialWidget::slotSendTimeout() {
    const QString error = tr("Write operation timed out for port %1.\n"
                                "Error: %2").arg(portName(),
                                                errorString());
    emit signalFailed(error);
}



