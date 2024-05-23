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

#include <QDialog>
#include <QSerialPort>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QIntValidator>
#include <QComboBox>

struct SerialPortSettings {
    QString name;
    qint32 baudRate;
    QString stringBaudRate;
    QSerialPort::DataBits dataBits;
    QString stringDataBits;
    QSerialPort::Parity parity;
    QString stringParity;
    QSerialPort::StopBits stopBits;
    QString stringStopBits;
    QSerialPort::FlowControl flowControl;
    QString stringFlowControl;
    bool localEchoEnabled;
};


class DialogSerialSettings : public QDialog {
    Q_OBJECT

 public:
    explicit DialogSerialSettings(QWidget *parent, SerialPortSettings *settings);

 private slots:
    void showPortInfo(int idx);
    void apply();
    void checkCustomBaudRatePolicy(int idx);
    void checkCustomDevicePathPolicy(int idx);

 private:
    void fillPortsParameters();
    void fillPortsInfo();
    void updateSettings();

 private:
    SerialPortSettings *settings_;
    QIntValidator *m_intValidator = nullptr;

    QComboBox *serialPortInfoListBox;
    QLabel *descriptionLabel;
    QLabel *manufacturerLabel;
    QLabel *serialNumberLabel;
    QLabel *locationLabel;
    QLabel *vidLabel;
    QLabel *pidLabel;

    QComboBox *baudRateBox;
    QComboBox *dataBitsBox;
    QComboBox *parityBox;
    QComboBox *stopBitsBox;
    QComboBox *flowControlBox;

    QCheckBox *localEchoCheckBox;
};


