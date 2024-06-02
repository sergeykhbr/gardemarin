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

#include "tab_npk.h"
#include <QVBoxLayout>

TabNPK::TabNPK(QWidget *parent)
    : QWidget(parent),
    timer_(this) {

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(6);
    layout->setContentsMargins(0, 0, 0, 0);

    AttributeType cfgEC;
    AttributeType cfgPH;
    AttributeType cfgNPK;
    cfgEC.from_config("{"
        "'GroupName':'Salinity',"
        "'GroupUnits':'mg/kg',"
        "'Lines':["
          "{"
            "'Name':'EC',"
            "'Format':'%.1f',"
            "'RingLength':256,"
            "'Color':'#FFFFFF',"
            "'FixedMinY':true,"
            "'FixedMinYVal':0.0,"
            "'FixedMaxY':true,"
            "'FixedMaxYVal':600.0,"
            "'NormalMaxY':true,"
            "'NormalMaxYVal':250.0,"
            "'NormalMinY':true,"
            "'NormalMinYVal':110.0"
          "}"
        "]"
        "}");

    cfgPH.from_config("{"
        "'GroupName':'pH',"
        "'GroupUnits':'',"
        "'Lines':["
          "{"
            "'Name':'pH',"
            "'Format':'%.1f',"
            "'RingLength':256,"
            "'Color':'#FFFFFF',"
            "'FixedMinY':true,"
            "'FixedMinYVal':0.0,"
            "'FixedMaxY':true,"
            "'FixedMaxYVal':10.0,"
            "'NormalMaxY':true,"
            "'NormalMaxYVal':7.0,"
            "'NormalMinY':true,"
            "'NormalMinYVal':5.5"
          "}"
        "]"
        "}");
    cfgNPK.from_config("{"
        "'GroupName':'NPK',"
        "'GroupUnits':'mg/kg',"
        "'Lines':["
          "{"
            "'Name':'N',"
            "'Format':'%.1f',"
            "'RingLength':256,"
            "'Color':'#BD63C5',"
            "'FixedMinY':true,"
            "'FixedMinYVal':0.0,"
            "'FixedMaxY':true,"
            "'FixedMaxYVal':500.0,"
            "'NormalMaxY':false,"
            "'NormalMaxYVal':0.0,"
            "'NormalMinY':false,"
            "'NormalMinYVal':0.0"
          "},"
          "{"
            "'Name':'P',"
            "'Format':'%.1f',"
            "'RingLength':256,"
            "'Color':'#40C977',"
            "'FixedMinY':true,"
            "'FixedMinYVal':0.0,"
            "'FixedMaxY':true,"
            "'FixedMaxYVal':500.0,"
            "'NormalMaxY':false,"
            "'NormalMaxYVal':0.0,"
            "'NormalMinY':false,"
            "'NormalMinYVal':0.0"
          "},"
          "{"
            "'Name':'K',"
            "'Format':'%.1f',"
            "'RingLength':256,"
            "'Color':'#007ACC',"
            "'FixedMinY':true,"
            "'FixedMinYVal':0.0,"
            "'FixedMaxY':true,"
            "'FixedMaxYVal':500.0,"
            "'NormalMaxY':false,"
            "'NormalMaxYVal':0.0,"
            "'NormalMinY':false,"
            "'NormalMinYVal':0.0"
          "}"
        "]"
        "}");

    plotEC_ = new PlotWidget(this, &cfgEC),
    plotPH_ = new PlotWidget(this, &cfgPH);
    plotNPK_ = new PlotWidget(this, &cfgNPK);

    layout->addWidget(plotEC_, 1);
    layout->addWidget(plotPH_, 1);
    layout->addWidget(plotNPK_, 2);
    setLayout(layout);

    timer_.setSingleShot(false);

    connect(&timer_, &QTimer::timeout, this, &TabNPK::slotTimeToRequest);
    timer_.start(std::chrono::seconds{10});

}

void TabNPK::slotTimeToRequest() {
    emit signalRequestReadAttribute(tr("soil0"), tr("EC"));
    emit signalRequestReadAttribute(tr("soil0"), tr("pH"));
    emit signalRequestReadAttribute(tr("soil0"), tr("N"));
    emit signalRequestReadAttribute(tr("soil0"), tr("P"));
    emit signalRequestReadAttribute(tr("soil0"), tr("K"));
}

void TabNPK::slotResponseAttribute(const QString &objname, const QString &atrname, quint32 data) {
    if (objname == "soil0") {
        if (atrname == "EC") {
            plotEC_->writeData(0, static_cast<double>(data));
        } else if (atrname == "pH") {
            plotPH_->writeData(0, static_cast<double>(data) / 100.0);
        } else if (atrname == "N") {
            plotNPK_->writeData(0, static_cast<double>(data));
        } else if (atrname == "P") {
            plotNPK_->writeData(1, static_cast<double>(data));
        } else if (atrname == "K") {
            plotNPK_->writeData(2, static_cast<double>(data));
        }
    }
}

