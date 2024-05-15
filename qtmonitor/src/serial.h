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

#include <QSerialPort>
#include <QTimer>

class SerialWidget : public QSerialPort {
    Q_OBJECT

 public:
    explicit SerialWidget(QObject *parent);

 signals:
    void signalRecvSerialPort(const QByteArray &data);
    void signalFailed(const QString &msg);
    void signalRxFrame(quint32 objid, quint32 attrid, quint64 payload);

 public slots:
    void slotSendSerialPort(const QByteArray &data);

 protected slots:
    void slotRecvSerialPort();
    void slotBytesWritten(qint64 bytes);
    void slotSendTimeout();
    void handleError(QSerialPort::SerialPortError error);

 private:
    QTimer timer_;
    qint64 bytesToWrite_;
};
