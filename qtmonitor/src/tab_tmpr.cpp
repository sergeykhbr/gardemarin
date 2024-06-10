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

#include "tab_tmpr.h"
#include <QVBoxLayout>

TabTemperature::TabTemperature(QWidget *parent, AttributeType *cfg)
    : QWidget(parent),
    timer_(this) {

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(6);
    layout->setContentsMargins(0, 0, 0, 0);

    plotTemperature_ = new PlotWidget(this, &(*cfg)["Groups"][0u]),

    layout->addWidget(plotTemperature_);
    setLayout(layout);

    timer_.setSingleShot(false);

    connect(&timer_, &QTimer::timeout, this, &TabTemperature::slotTimeToRequest);
    timer_.start(std::chrono::seconds{(*cfg)["UpdateTime"].to_int()});

}

void TabTemperature::slotTimeToRequest() {
    emit signalRequestReadAttribute(tr("soil0"), tr("T"));
    emit signalRequestReadAttribute(tr("adc1"), tr("temperature"));
    emit signalRequestReadAttribute(tr("temp0"), tr("T0"));
    emit signalRequestReadAttribute(tr("temp0"), tr("T1"));
}

void TabTemperature::slotResponseAttribute(const QString &objname, const QString &atrname, quint32 data) {
    if (objname == "soil0") {
        if (atrname == "T") {
            plotTemperature_->writeData(0, static_cast<double>(data) / 100.0);
        }
    } else if (objname == "temp0") {
        if (atrname == "T0") {
            plotTemperature_->writeData(1, static_cast<double>(data) / 100.0);
        } else if (atrname == "T1") {
            plotTemperature_->writeData(2, static_cast<double>(data) / 100.0);
        }
    } else if (objname == "adc1") {
        if (atrname == "temperature") {
            plotTemperature_->writeData(3, static_cast<double>(data) / 10.0);
        }
    }
}

