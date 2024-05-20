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
    explicit TabScales(QWidget *parent = nullptr);

 signals:
    void signalSendData(const QByteArray &data);
    void signalRequestScaleAttribute(const QString &objname, const QString &atrname);

 public slots:
    void slotSendData(const QByteArray &data) {
        emit signalSendData(data);
    }
    void slotResponseScaleAttribute(const QString &objname, const QString &atrname, uint32_t data);

 private slots:
    void slotTimeToRequest();

 private:
    PlotWidget *plotMoisture_;
    PlotWidget *plotScales_;
    QTimer timer_;
};


