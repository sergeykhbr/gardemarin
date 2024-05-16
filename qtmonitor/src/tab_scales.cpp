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

    plotMix_ = new PlotWidget(this),
    plotPlant_ = new PlotWidget(this);

    layout->addWidget(plotMix_);
    layout->addWidget(plotPlant_);
    setLayout(layout);

    timer_.setSingleShot(false);

    connect(&timer_, &QTimer::timeout, this, &TabScales::slotTimeToRequest);
    timer_.start(std::chrono::seconds{1});

}

void TabScales::slotTimeToRequest() {
    emit signalRequestScaleAttribute(tr("scales"), tr("gram0"));
    emit signalRequestScaleAttribute(tr("scales"), tr("gram1"));
    emit signalRequestScaleAttribute(tr("scales"), tr("gram2"));
}

void TabScales::slotResponseScaleAttribute(const QString &objname, const QString &atrname, quint32 data) {
    if (objname != "scales") {
        return;
    }
    if (atrname == "gram0") {
        float t1;
        *reinterpret_cast<quint32 *>(&t1) = data;
        plotPlant_->writeData(0, t1);
    } else if (atrname == "gram1") {
        float t1;
        *reinterpret_cast<quint32 *>(&t1) = data;
        plotPlant_->writeData(1, t1);
    } else if (atrname == "gram2") {
        float t1;
        *reinterpret_cast<quint32 *>(&t1) = data;
        plotPlant_->writeData(2, t1);
    }
}

