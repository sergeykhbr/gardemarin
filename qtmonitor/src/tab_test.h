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
#include "console.h"
#include "ctrlpanel.h"

class TabTest : public QWidget {
    Q_OBJECT

 public:
    explicit TabTest(QWidget *parent = nullptr);

    void clearSerialConsole() {
        m_console->clear();
    }

 public slots:
    void slotSerialPortOpened(bool localEchoEnabled) {
        m_console->setEnabled(true);
        m_console->setLocalEchoEnabled(localEchoEnabled);
    }

    void slotSerialPortClosed() {
        m_console->setEnabled(false);
    }

    void writeConsoleData(const QByteArray &data);
    void readConsoleData();

 private:
    Console *m_console;
    ControlPanel *m_panel;
};


