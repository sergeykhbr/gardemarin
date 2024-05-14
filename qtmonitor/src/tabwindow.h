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

#include <QTabWidget>
#include "tab_scales.h"
#include "tab_test.h"

class TabWindow : public QTabWidget {
    Q_OBJECT

 public:
    explicit TabWindow(QWidget *parent = nullptr);


 signals:
    void signalSendData(const QByteArray &data);

 public slots:
    void slotSerialPortOpened(bool localEchoEnabled) {
        tabTest_->openSerialPort(localEchoEnabled);
    }

    void slotSerialPortClosed() {
        tabTest_->closeSerialPort();
    }

    void slotSendData(const QByteArray &data) {
        emit signalSendData(data);
    }

    void slotRecvData(const QByteArray &data) {
        tabTest_->writeData(data);
    }

    void slotClearSerialConsole() {
        tabTest_->clearSerialConsole();
    }

 private:
    TabScales *tabScales_;
    TabTest *tabTest_;
};


