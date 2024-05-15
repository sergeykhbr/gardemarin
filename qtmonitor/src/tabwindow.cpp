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

#include "tabwindow.h"

TabWindow::TabWindow(QWidget *parent)
    : QTabWidget(parent) {
    tabScales_ = new TabScales(this);
    tabTest_ = new TabTest(this);

    addTab(tabScales_, tr("Scales"));
    addTab(tabTest_, tr("Test"));

    connect(tabTest_, &TabTest::signalSendData,
            this, &TabWindow::slotSendData);
}

void TabWindow::slotRxFrame(quint32 objid, quint32 attrid, quint64 payload)
{
}

