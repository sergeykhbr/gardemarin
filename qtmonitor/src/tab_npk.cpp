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

TabNPK::TabNPK(QWidget *parent, AttributeType *cfg)
    : QWidget(parent),
    timer_(this) {

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(6);
    layout->setContentsMargins(0, 0, 0, 0);

    plotEC_ = new PlotWidget(this, &(*cfg)["Groups"][0u]),
    plotPH_ = new PlotWidget(this, &(*cfg)["Groups"][1]);
    plotNPK_ = new PlotWidget(this, &(*cfg)["Groups"][2]);

    layout->addWidget(plotEC_, 1);
    layout->addWidget(plotPH_, 1);
    layout->addWidget(plotNPK_, 2);
    setLayout(layout);

    timer_.setSingleShot(false);

    connect(&timer_, &QTimer::timeout, this, &TabNPK::slotTimeToRequest);
    timer_.start(std::chrono::seconds{(*cfg)["UpdateTime"].to_int()});

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

