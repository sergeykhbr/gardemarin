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

#include <attribute.h>
#include <QSerialPort>
#include <QTimer>

typedef union can_payload_type {
    quint64 u64;
    quint32 u32[2];
    quint8 u8[8];
    char s8[8];
} can_payload_type;

typedef struct can_frame_type {
    quint32 timestamp;
    quint32 id;
    quint8 dlc;
    quint8 busid;
    can_payload_type data;
} can_frame_type;

class SerialWidget : public QSerialPort {
    Q_OBJECT

 public:
    SerialWidget(QObject *parent, AttributeType *cfg);

 signals:
    void signalRecvSerialPort(const QByteArray &data);
    void signalResponseReadAttribute(const QString &objname, const QString &atrname, quint32 val);
    void signalFailed(const QString &msg);
    void signalRxFrame(quint32 objid, quint32 attrid, quint64 payload);

 public slots:
    void slotSendSerialPort(const QByteArray &data);
    void slotRequestReadAttribute(const QString &objname, const QString &atrname);

 protected slots:
    void slotRecvSerialPort();
    void slotBytesWritten(qint64 bytes);
    void slotSendTimeout();
    void handleError(QSerialPort::SerialPortError error);

 private:
    quint32 str2hex32(char *buf, int sz);
    void idx2names(quint32 id, QString &objname, quint32 atrid, QString &atrname, QString &type);
    QString names2request(const QString &objname, const QString &atrname);

    void processRxCanFrame(can_frame_type *frame);

 private:
    AttributeType ObjectsList_;
    QTimer timer_;
    qint64 bytesToWrite_;

    enum EFrameDecoderState {
        State_PRM1,     // 1 B = ">"
        State_PRM2,     // 1 B = "!"
        State_CanId,    // 8 B = "12345678" hex in string format
        State_Comma1,   // 1 B = ","
        State_DLC,      // 1 B = "1"..."8"
        State_Comma2,   // 1 B = ","
        State_Payload   // 16 B
    } eframestate_;
    char rawid_[11];
    char rawdlc_;
    char rawpayload_[17];
    int rawcnt_;

};
