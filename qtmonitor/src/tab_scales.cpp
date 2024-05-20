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

#include "tab_scales.h"
#include <QVBoxLayout>

TabScales::TabScales(QWidget *parent)
    : QWidget(parent),
    timer_(this) {

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(6);
    layout->setContentsMargins(0, 0, 0, 0);

    AttributeType scalesCfg;
    AttributeType moistureCfg;
    moistureCfg.from_config("{"
        "'GroupName':'Moisture',"
        "'GroupUnits':'%',"
        "'Lines':["
          "{"
            "'Name':'moisture',"
            "'Format':'%.1f',"
            "'RingLength':256,"
            "'Color':'#007ACC',"
            "'FixedMinY':true,"
            "'FixedMinYVal':100.0,"
            "'FixedMaxY':true,"
            "'FixedMaxYVal':0.0"
          "}"
        "]"
        "}");

    scalesCfg.from_config("{"
        "'GroupName':'Scales',"
        "'GroupUnits':'gram',"
        "'Lines':["
          "{"
            "'Name':'gram1',"
            "'Format':'%.1f',"
            "'RingLength':256,"
            "'Color':'#FFFFFF',"
            "'FixedMinY':true,"
            "'FixedMinYVal':0.0,"
            "'FixedMaxY':true,"
            "'FixedMaxYVal':4000.0"
          "},"
          "{"
            "'Name':'gram2',"
            "'Format':'%.1f',"
            "'RingLength':256,"
            "'Color':'#40C977',"
            "'FixedMinY':true,"
            "'FixedMinYVal':0.0,"
            "'FixedMaxY':true,"
            "'FixedMaxYVal':4000.0"
          "},"
          "{"
            "'Name':'gram3',"
            "'Format':'%.1f',"
            "'RingLength':256,"
            "'Color':'#007ACC',"
            "'FixedMinY':true,"
            "'FixedMinYVal':0.0,"
            "'FixedMaxY':true,"
            "'FixedMaxYVal':4000.0"
          "}"
        "]"
        "}");

    plotMoisture_ = new PlotWidget(this, &moistureCfg),
    plotScales_ = new PlotWidget(this, &scalesCfg);

    layout->addWidget(plotMoisture_, 1);
    layout->addWidget(plotScales_, 2);
    setLayout(layout);

    timer_.setSingleShot(false);

    connect(&timer_, &QTimer::timeout, this, &TabScales::slotTimeToRequest);
    timer_.start(std::chrono::seconds{1});

}

void TabScales::slotTimeToRequest() {
    emit signalRequestScaleAttribute(tr("scales"), tr("gram0"));
    emit signalRequestScaleAttribute(tr("scales"), tr("gram1"));
    emit signalRequestScaleAttribute(tr("scales"), tr("gram2"));
    emit signalRequestScaleAttribute(tr("soil0"), tr("moisture"));
}

void TabScales::slotResponseScaleAttribute(const QString &objname, const QString &atrname, quint32 data) {
    if (objname == "scales") {
        if (atrname == "gram0") {
            float t1;
            *reinterpret_cast<quint32 *>(&t1) = data;
            plotScales_->writeData(0, t1);
        } else if (atrname == "gram1") {
            float t1;
            *reinterpret_cast<quint32 *>(&t1) = data;
            plotScales_->writeData(1, t1);
        } else if (atrname == "gram2") {
            float t1;
            *reinterpret_cast<quint32 *>(&t1) = data;
            plotScales_->writeData(2, t1);
        }
    } else if (objname == "soil0") {
        if (atrname == "moisture") {
            uint32_t t1 = data >> 24;
            t1 |= (data >> 8) & 0xFF00;

            plotMoisture_->writeData(0, static_cast<double>(t1) / 100.0);
        }
    }
}

