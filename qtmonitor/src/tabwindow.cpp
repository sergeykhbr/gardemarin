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
#include <QMenuBar>
#include <QMenu>

TabWindow::TabWindow(QWidget *parent, SerialWidget *serial, AttributeType *cfg)
    : QTabWidget(parent), serial_(serial) {
    tabScales_ = new TabScales(this, &(*cfg)["MonitorConfig"]["TabScales"]);
    tabNPK_ = new TabNPK(this, &(*cfg)["MonitorConfig"]["TabNPK"]);
    tabTemperature_ = new TabTemperature(this, &(*cfg)["MonitorConfig"]["TabTemperature"]);
    tabLights_ = new TabLights(this);
    tabCamera_ = new TabCamera(this);
    tabUsrSettings_ = new TabUserSettings(this);
    tabCalculator_ = new TabCalculator(this, &(*cfg)["MonitorConfig"]["TabCalculator"]);
    tabTest_ = new TabTest(this);

    addTab(tabScales_, tr("Scales"));
    addTab(tabNPK_, tr("NPK"));
    addTab(tabTemperature_, tr("T"));
    addTab(tabLights_, tr("Lights"));
    addTab(tabCamera_, tr("Camera"));
    addTab(tabUsrSettings_, tr("Settings"));
    addTab(tabCalculator_, tr("Calc"));
    addTab(tabTest_, tr("Test"));

    // connect TabScales:
    connect(tabScales_, &TabScales::signalRequestReadAttribute,
            serial_, &SerialWidget::slotRequestReadAttribute);

    connect(serial_, &SerialWidget::signalResponseReadAttribute,
            tabScales_, &TabScales::slotResponseAttribute);

    connect(tabScales_, &TabScales::signalTextToStatusBar,
            this, &TabWindow::slotTextToStatusBar);

    // connect TabNPK:
    connect(tabNPK_, &TabNPK::signalRequestReadAttribute,
            serial_, &SerialWidget::slotRequestReadAttribute);

    connect(serial_, &SerialWidget::signalResponseReadAttribute,
            tabNPK_, &TabNPK::slotResponseAttribute);

    // connect TabTemperature:
    connect(tabTemperature_, &TabTemperature::signalRequestReadAttribute,
            serial_, &SerialWidget::slotRequestReadAttribute);

    connect(serial_, &SerialWidget::signalResponseReadAttribute,
            tabTemperature_, &TabTemperature::slotResponseAttribute);

    // connect TabLights:
    connect(tabLights_, &TabLights::signalRequestReadAttribute,
            serial_, &SerialWidget::slotRequestReadAttribute);

    connect(tabLights_, &TabLights::signalRequestWriteAttribute,
            serial_, &SerialWidget::slotRequestWriteAttribute);

    connect(serial_, &SerialWidget::signalResponseReadAttribute,
            tabLights_, &TabLights::slotResponseAttribute);

    // connect TabCamera:
    connect(tabCamera_, &TabCamera::signalTextToStatusBar,
            this, &TabWindow::slotTextToStatusBar);

    // connect TabUserSettings
    connect(tabUsrSettings_, &TabUserSettings::signalRequestReadAttribute,
            serial_, &SerialWidget::slotRequestReadAttribute);

    connect(tabUsrSettings_, &TabUserSettings::signalRequestWriteAttribute,
            serial_, &SerialWidget::slotRequestWriteAttribute);

    connect(serial_, &SerialWidget::signalResponseReadAttribute,
            tabUsrSettings_, &TabUserSettings::slotResponseAttribute);


    // connect TabTest:
    connect(tabTest_, &TabTest::signalSendData,
            serial_, &SerialWidget::slotSendSerialPort);

    connect(serial_, &SerialWidget::signalRecvSerialPort,
            tabTest_, &TabTest::slotRecvData);
}


