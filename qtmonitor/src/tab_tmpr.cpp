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

TabTemperature::TabTemperature(QWidget *parent)
    : QWidget(parent),
    timer_(this) {

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(6);
    layout->setContentsMargins(0, 0, 0, 0);

    AttributeType cfgTemp;
    cfgTemp.from_config("{"
        "'GroupName':'Temperature',"
        "'GroupUnits':'C',"
        "'Lines':["
          "{"
            "'Name':'fog',"
            "'Format':'%.1f',"
            "'RingLength':256,"
            "'Color':'#007ACC',"
            "'FixedMinY':true,"
            "'FixedMinYVal':0.0,"
            "'FixedMaxY':true,"
            "'FixedMaxYVal':60.0,"
            "'NormalMaxY':false,"
            "'NormalMaxYVal':0.0,"
            "'NormalMinY':false,"
            "'NormalMinYVal':0.0"
          "},"
          "{"
            "'Name':'cpu',"
            "'Format':'%.1f',"
            "'RingLength':256,"
            "'Color':'#CA5100',"
            "'FixedMinY':true,"
            "'FixedMinYVal':0.0,"
            "'FixedMaxY':true,"
            "'FixedMaxYVal':60.0,"
            "'NormalMaxY':false,"
            "'NormalMaxYVal':0.0,"
            "'NormalMinY':false,"
            "'NormalMinYVal':0.0"
          "}"
        "]"
        "}");
    plotTemperature_ = new PlotWidget(this, &cfgTemp),

    layout->addWidget(plotTemperature_);
    setLayout(layout);

    timer_.setSingleShot(false);

    connect(&timer_, &QTimer::timeout, this, &TabTemperature::slotTimeToRequest);
    timer_.start(std::chrono::seconds{1});

}

void TabTemperature::slotTimeToRequest() {
    emit signalRequestScaleAttribute(tr("soil0"), tr("T"));
    emit signalRequestScaleAttribute(tr("adc1"), tr("temperature"));
}

void TabTemperature::slotResponseScaleAttribute(const QString &objname, const QString &atrname, quint32 data) {
    if (objname == "soil0") {
        if (atrname == "T") {
            plotTemperature_->writeData(0, static_cast<double>(data) / 100.0);
        }
    } else if (objname == "adc1") {
        if (atrname == "temperature") {
            plotTemperature_->writeData(1, static_cast<double>(data) / 10.0);
        }
    }
}
