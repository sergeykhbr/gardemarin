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
    timer_(this),
    gram0_(0),
    gram1_(0),
    gram2_(0) {

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
            "'FixedMinYVal':0.0,"
            "'FixedMaxY':true,"
            "'FixedMaxYVal':100.0,"
            "'NormalMaxY':true,"
            "'NormalMaxYVal':100.0,"
            "'NormalMinY':true,"
            "'NormalMinYVal':80.0"
          "}"
        "]"
        "}");

    scalesCfg.from_config("{"
        "'GroupName':'Scales',"
        "'GroupUnits':'gram',"
        "'Lines':["
          "{"
            "'Name':'sewer',"
            "'Format':'%.1f',"
            "'RingLength':256,"
            "'Color':'#BD63C5',"
            "'FixedMinY':true,"
            "'FixedMinYVal':0.0,"
            "'FixedMaxY':true,"
            "'FixedMaxYVal':6000.0,"
            "'NormalMaxY':false,"
            "'NormalMaxYVal':0.0,"
            "'NormalMinY':false,"
            "'NormalMinYVal':0.0"
          "},"
          "{"
            "'Name':'plant',"
            "'Format':'%.1f',"
            "'RingLength':256,"
            "'Color':'#40C977',"
            "'FixedMinY':true,"
            "'FixedMinYVal':0.0,"
            "'FixedMaxY':true,"
            "'FixedMaxYVal':6000.0,"
            "'NormalMaxY':false,"
            "'NormalMaxYVal':0.0,"
            "'NormalMinY':false,"
            "'NormalMinYVal':0.0"
          "},"
          "{"
            "'Name':'mix',"
            "'Format':'%.1f',"
            "'RingLength':256,"
            "'Color':'#007ACC',"
            "'FixedMinY':true,"
            "'FixedMinYVal':0.0,"
            "'FixedMaxY':true,"
            "'FixedMaxYVal':6000.0,"
            "'NormalMaxY':false,"
            "'NormalMaxYVal':0.0,"
            "'NormalMinY':false,"
            "'NormalMinYVal':0.0"
          "},"
          "{"
            "'Name':'total',"
            "'Format':'%.1f',"
            "'RingLength':256,"
            "'Color':'#FFFFFF',"
            "'FixedMinY':true,"
            "'FixedMinYVal':0.0,"
            "'FixedMaxY':true,"
            "'FixedMaxYVal':6000.0,"
            "'NormalMaxY':true,"
            "'NormalMaxYVal':6000.0,"
            "'NormalMinY':true,"
            "'NormalMinYVal':4000.0"
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
    emit signalRequestReadAttribute(tr("scales"), tr("gram0"));
    emit signalRequestReadAttribute(tr("scales"), tr("gram1"));
    emit signalRequestReadAttribute(tr("scales"), tr("gram2"));
    emit signalRequestReadAttribute(tr("soil0"), tr("moisture"));
}

void TabScales::slotResponseAttribute(const QString &objname, const QString &atrname, quint32 data) {
    if (objname == "scales") {
        if (atrname == "gram0") {
            *reinterpret_cast<quint32 *>(&gram0_) = data;
            plotScales_->writeData(0, gram0_);
        } else if (atrname == "gram1") {
            float prev = gram1_;
            *reinterpret_cast<quint32 *>(&gram1_) = data;
            plotScales_->writeData(1, gram1_);

            // Show delta mix grams in the status bar
            QString text = QString::asprintf("%.1f", gram1_ - prev);
            emit signalTextToStatusBar(1, text);
        } else if (atrname == "gram2") {
            *reinterpret_cast<quint32 *>(&gram2_) = data;
            plotScales_->writeData(2, gram2_);
            plotScales_->writeData(3, gram0_ + gram1_ + gram2_);
        }
    } else if (objname == "soil0") {
        if (atrname == "moisture") {
            plotMoisture_->writeData(0, static_cast<double>(data) / 100.0);
        }
    }
}

