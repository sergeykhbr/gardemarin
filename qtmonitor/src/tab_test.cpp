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

#include "tab_test.h"
#include <QVBoxLayout>

TabTest::TabTest(QWidget *parent)
    : QWidget(parent) {

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(6);
    layout->setContentsMargins(0, 0, 0, 0);

    m_console = new Console(this),
    m_panel = new ControlPanel(this);

    layout->addWidget(m_panel);
    layout->addWidget(m_console);
    setLayout(layout);

    m_console->setEnabled(false);

    connect(m_console, &Console::signalSendData, this, &TabTest::slotSendData);
    connect(m_panel, &ControlPanel::signalSendData, this, &TabTest::slotSendData);
}

