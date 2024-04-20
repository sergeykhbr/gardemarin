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

#include <QMainWindow>
#include <QSerialPort>
#include <QLabel>
#include "console.h"
#include "ctrlpanel.h"
#include "comsettings.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

 public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

 private slots:
    void openSerialPort();
    void closeSerialPort();
    void about();
    void writeConsoleData(const QByteArray &data);
    void readConsoleData();

    void handleError(QSerialPort::SerialPortError error);
    void handleBytesWritten(qint64 bytes);
    void handleWriteTimeout();

 private:
    void showStatusMessage(const QString &message);
    void showWriteError(const QString &message);

    QAction *actionConnect_;
    QAction *actionDisconnect_;
    QAction *actionClear_;
    QAction *actionConfigure_;
    QAction *actionAbout_;
    QAction *actionQuit_;

    MainWindow *m_ui = nullptr;
    QLabel *m_status = nullptr;
    Console *m_console = nullptr;
    ComPortSettings *m_settings = nullptr;
    qint64 m_bytesToWrite = 0;
    QTimer *m_timer = nullptr;
    QSerialPort *m_serial = nullptr;
    ControlPanel *m_panel;
};
