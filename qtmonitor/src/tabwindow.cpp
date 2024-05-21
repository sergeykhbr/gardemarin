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

TabWindow::TabWindow(QWidget *parent, SerialWidget *serial)
    : QTabWidget(parent), serial_(serial) {
    tabScales_ = new TabScales(this);
    tabNPK_ = new TabNPK(this);
    tabTemperature_ = new TabTemperature(this);
    tabTest_ = new TabTest(this);

    addTab(tabScales_, tr("Scales"));
    addTab(tabNPK_, tr("NPK"));
    addTab(tabTemperature_, tr("T"));
    addTab(tabTest_, tr("Test"));

    // connect TabScales:
    connect(tabScales_, &TabScales::signalRequestScaleAttribute,
            serial_, &SerialWidget::slotRequestReadAttribute);

    connect(serial_, &SerialWidget::signalResponseReadAttribute,
            tabScales_, &TabScales::slotResponseScaleAttribute);

    // connect TabNPK:
    connect(tabNPK_, &TabNPK::signalRequestScaleAttribute,
            serial_, &SerialWidget::slotRequestReadAttribute);

    connect(serial_, &SerialWidget::signalResponseReadAttribute,
            tabNPK_, &TabNPK::slotResponseScaleAttribute);

    // connect TabTemperature:
    connect(tabTemperature_, &TabTemperature::signalRequestScaleAttribute,
            serial_, &SerialWidget::slotRequestReadAttribute);

    connect(serial_, &SerialWidget::signalResponseReadAttribute,
            tabTemperature_, &TabTemperature::slotResponseScaleAttribute);

    // connect TabTest:
    connect(tabTest_, &TabTest::signalSendData,
            serial_, &SerialWidget::slotSendSerialPort);

    connect(serial_, &SerialWidget::signalRecvSerialPort,
            tabTest_, &TabTest::slotRecvData);

}


