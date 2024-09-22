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
#include <QTimer>
#include "chart/PlotWidget.h"

class TabScales : public QWidget {
    Q_OBJECT

 public:
    explicit TabScales(QWidget *parent, AttributeType *cfg);

 signals:
    void signalRequestReadAttribute(const QString &objname, const QString &atrname);
    void signalTextToStatusBar(qint32 idx, const QString &text);

 public slots:
    void slotResponseAttribute(const QString &objname, const QString &atrname, uint32_t data);

 private slots:
    void slotTimeToRequest();

 private:
    PlotWidget *plotMoisture_;
    PlotWidget *plotScales_;
    QTimer timer_;
    float gram1_;
    float gram2_;
    float gram2flt_;
};


