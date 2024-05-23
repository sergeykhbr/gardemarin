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

SerialWidget::SerialWidget(QObject *parent, AttributeType *cfg) :
    QSerialPort(parent),
    timer_(this) {

    ObjectsList_ = (*cfg)["TargetConfig"]["ObjectsList"];
    bytesToWrite_ = 0;
    eframestate_ = State_PRM1;

    timer_.setSingleShot(true);

    connect(&timer_, &QTimer::timeout, this, &SerialWidget::slotSendTimeout);

    connect(this, &SerialWidget::errorOccurred, this, &SerialWidget::handleError);
    connect(this, &QSerialPort::readyRead, this, &SerialWidget::slotRecvSerialPort);
    connect(this, &QSerialPort::bytesWritten, this, &SerialWidget::slotBytesWritten);
}

bool SerialWidget::open(OpenMode mode) {
    setPortName(settings_.name);
    setBaudRate(settings_.baudRate);
    setDataBits(settings_.dataBits);
    setParity(settings_.parity);
    setStopBits(settings_.stopBits);
    setFlowControl(settings_.flowControl);

    bool ret = QSerialPort::open(mode);
    if (ret) {
        const QString text = QString::asprintf("Connected to %s : %d",
                settings_.name.toUtf8().constBegin(), settings_.baudRate);
        emit signalSerialPortOpened();
        emit signalTextToStatusBar(0, text);
    } else {
        const QString error = QString::asprintf("Open error %s",
                settings_.name.toUtf8().constBegin());
        emit signalFailed(error);
        emit signalTextToStatusBar(0, error);
    }
    return ret;
}

void SerialWidget::close() {
    if (isOpen()) {
        emit signalSerialPortClosed();
    }
    QSerialPort::close();
}

quint32 SerialWidget::str2hex32(char *buf, int sz) {
    quint32 ret = 0;
    for (int i = 0; i < sz; i++) {
        ret <<= 4;
        if (buf[i] >= '0' && buf[i] <= '9') {
            ret |= buf[i] - '0';
        } else if (buf[i] >= 'a' && buf[i] <= 'f') {
            ret |= buf[i] - 'a' + 10;
        } else if (buf[i] >= 'A' && buf[i] <= 'F') {
            ret |= buf[i] - 'A' + 10;
        }
    }
    return ret;
}

void SerialWidget::idx2names(quint32 canid, QString &objname,
                             quint32 atrid, QString &atrname,
                             QString &type) {
    for (unsigned i = 0; i < ObjectsList_.size(); i++) {
        const AttributeType &obj = ObjectsList_[i];
        objname = QString(obj["Name"].to_string());
        if (obj["Index"].to_int() != (canid & 0xFF)) {
            continue;
        }
        for (unsigned n = 0; n < obj["Attributes"].size(); n++) {
            const AttributeType &atr = obj["Attributes"][n];
            if (atr["Index"].to_int() != atrid) {
                continue;
            }
            atrname = QString(atr["Name"].to_string());
            type = QString(atr["Type"].to_string());
            break;
        }
        break;
    }
}

QString SerialWidget::names2request(const QString &objname, const QString &atrname) {
    QString ret = "";
    for (unsigned i = 0; i < ObjectsList_.size(); i++) {
        const AttributeType &obj = ObjectsList_[i];
        if (objname != QString(obj["Name"].to_string())) {
            continue;
        }
        for (unsigned n = 0; n < obj["Attributes"].size(); n++) {
            const AttributeType &atr = obj["Attributes"][n];
            if (atrname != QString(atr["Name"].to_string())) {
                continue;
            }
            char buf[32];
            sprintf(buf, ">!%08x,1,%02x\r\n",
                        obj["Index"].to_uint32(),
                        atr["Index"].to_uint32());
    
            return QString(buf);
        }
        break;
    }
    return ret;
}

QString SerialWidget::names2request(const QString &objname, const QString &atrname, quint32 data) {
    QString ret = "";
    for (unsigned i = 0; i < ObjectsList_.size(); i++) {
        AttributeType &obj = ObjectsList_[i];
        if (objname != QString(obj["Name"].to_string())) {
            continue;
        }
        for (unsigned n = 0; n < obj["Attributes"].size(); n++) {
            AttributeType &atr = obj["Attributes"][n];
            if (atrname != QString(atr["Name"].to_string())) {
                continue;
            }
            char buf[32];
            AttributeType &type = atr["Type"];
            if (type.is_equal("uint8") || type.is_equal("int8")) {
                sprintf(buf, ">!%08x,2,%02x%02x\r\n",
                            obj["Index"].to_uint32(),
                            0x80 | atr["Index"].to_uint32(),
                            data & 0xFF);
            } else if (type.is_equal("uint16") || type.is_equal("int16")) {
                sprintf(buf, ">!%08x,3,%02x%04x\r\n",
                            obj["Index"].to_uint32(),
                            0x80 | atr["Index"].to_uint32(),
                            data & 0xFFFF);
            } else {
                sprintf(buf, ">!%08x,5,%02x%08x\r\n",
                            obj["Index"].to_uint32(),
                            0x80 | atr["Index"].to_uint32(),
                            data);
            }
    
            return QString(buf);
        }
        break;
    }
    return ret;
}

void SerialWidget::slotRecvSerialPort() {
    const QByteArray data = readAll();
    emit signalRecvSerialPort(data);

    // search CAN frames over Serial interface
    for (auto &s : data) {
        switch (eframestate_) {
        case State_PRM1:
            if (s == '<') {
                eframestate_ = State_PRM2;
            }
            break;
        case State_PRM2:
            if (s == '!') {
                eframestate_ = State_CanId;
                rawcnt_ = 0;
            } else {
                eframestate_ = State_PRM1;
            }
            break;
        case State_CanId:
            rawid_[rawcnt_++] = s;
            if (rawcnt_ == 8) {
                eframestate_ = State_Comma1;
            }
            break;
        case State_Comma1:
            if (s == ',') {
                eframestate_ = State_DLC;
            } else {
                eframestate_ = State_PRM1;
            }
            break;
        case State_DLC:
            if (s >= '1' && s <= '8') {
                eframestate_ = State_Comma2;
                rawcnt_ = 0;
                rawdlc_ = s - '0';
            } else {
                eframestate_ = State_PRM1;
            }
            break;
        case State_Comma2:
            if (s == ',') {
                eframestate_ = State_Payload;
            } else {
                eframestate_ = State_PRM1;
            }
            break;
        case State_Payload:
            rawpayload_[rawcnt_] = s;
            rawpayload_[rawcnt_ + 1] = '\0';
            if (++rawcnt_ == 2*rawdlc_) {
                eframestate_ = State_PRM1;
                can_frame_type frame;
                frame.busid = 0;
                frame.id = str2hex32(rawid_, 8);
                frame.dlc = static_cast<quint8>(rawdlc_);
                for (quint8 n = 0; n < frame.dlc; n++) {
                    frame.data.u8[n] = 
                        static_cast<quint8>(str2hex32(&rawpayload_[2*n], 2));
                }
                
                processRxCanFrame(&frame);
            }
            break;
        default:;
        }
    }
}

void SerialWidget::processRxCanFrame(can_frame_type *frame) {
    QString objname = tr("none");
    QString atrname = tr("none");
    QString type = tr("");
    quint32 data = static_cast<quint32>(frame->data.u64 >> 8);

    idx2names(frame->id, objname, frame->data.u8[0] & 0x7F, atrname, type);
    if (type == "uint8") {
        data = frame->data.u8[1];
    } else if (type == "int8") {
        data = frame->data.s8[1];
    } else if (type == "uint16") {
        data = frame->data.u8[2];
        data = (data << 8) | frame->data.u8[1];
    } else if (type == "int16") {
        data = frame->data.u8[2];
        data = (data << 8) | frame->data.u8[1];
        if (data & 0x8000) {
            data |= 0xFFFF0000;
        }
    } else if (type == "uint32" || type == "int32") {
        data = frame->data.u8[4];
        data = (data << 8) | frame->data.u8[3];
        data = (data << 8) | frame->data.u8[2];
        data = (data << 8) | frame->data.u8[1];
    }

    emit signalResponseReadAttribute(objname, atrname, data);
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

void SerialWidget::slotRequestReadAttribute(const QString &objname,
                                            const QString &atrname) {
    if (!isOpen()) {
        return;
    }

    QString request = names2request(objname, atrname);
    slotSendSerialPort(request.toUtf8());
}

void SerialWidget::slotRequestWriteAttribute(const QString &objname,
                                             const QString &atrname,
                                             quint32 data) {
    if (!isOpen()) {
        return;
    }

    QString request = names2request(objname, atrname, data);
    slotSendSerialPort(request.toUtf8());
}

void SerialWidget::handleError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        emit signalFailed(errorString());
    }
}


void SerialWidget::slotSendTimeout() {
    const QString error = tr("Write operation timed out for port %1.\n"
                                "Error: %2").arg(portName(),
                                                errorString());
    emit signalFailed(error);
}



