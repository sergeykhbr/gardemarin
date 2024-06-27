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
#include "serial.h"
#include "tab_scales.h"
#include "tab_npk.h"
#include "tab_tmpr.h"
#include "tab_test.h"
#include "tab_camera.h"
#include "tab_lights.h"
#include "tab_usrset.h"
#include "tab_calc.h"

class TabWindow : public QTabWidget {
    Q_OBJECT

 public:
    TabWindow(QWidget *parent, SerialWidget *serial, AttributeType *cfg);

 signals:
    void signalSendData(const QByteArray &data);
    void signalTextToStatusBar(qint32 idx, const QString &text);

 public slots:
    void slotSerialPortOpened() {
        tabTest_->openSerialPort();
    }

    void slotSerialPortClosed() {
        tabTest_->closeSerialPort();
    }

 private slots:
    // signal from tabs:
    void slotTextToStatusBar(qint32 idx, const QString &text) {
        emit signalTextToStatusBar(idx, text);
    }
   
 private:
    SerialWidget *serial_;
    TabScales *tabScales_;
    TabNPK *tabNPK_;
    TabTemperature *tabTemperature_;
    TabCamera *tabCamera_;
    TabLights *tabLights_;
    TabUserSettings *tabUsrSettings_;
    TabCalculator *tabCalculator_;
    TabTest *tabTest_;
};


