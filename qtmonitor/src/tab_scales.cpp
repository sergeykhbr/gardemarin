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

TabScales::TabScales(QWidget *parent, AttributeType *cfg)
    : QWidget(parent),
    timer_(this),
    gram1_(0),
    gram2_(0),
    gram2flt_(0) {

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(6);
    layout->setContentsMargins(0, 0, 0, 0);

    plotMoisture_ = new PlotWidget(this, &(*cfg)["Groups"][0u]),
    plotScales_ = new PlotWidget(this, &(*cfg)["Groups"][1]);

    layout->addWidget(plotMoisture_, 1);
    layout->addWidget(plotScales_, 2);
    setLayout(layout);

    timer_.setSingleShot(false);

    connect(&timer_, &QTimer::timeout, this, &TabScales::slotTimeToRequest);
    timer_.start(std::chrono::seconds{(*cfg)["UpdateTime"].to_int()});

}

void TabScales::slotTimeToRequest() {
    emit signalRequestReadAttribute(tr("scales"), tr("gram1"));
    emit signalRequestReadAttribute(tr("scales"), tr("gram2"));
    emit signalRequestReadAttribute(tr("scales"), tr("gram2flt"));
    emit signalRequestReadAttribute(tr("soil0"), tr("moisture"));
}

void TabScales::slotResponseAttribute(const QString &objname, const QString &atrname, quint32 data) {
    if (objname == "scales") {
        if (atrname == "gram1") {
            *reinterpret_cast<quint32 *>(&gram1_) = data;
            plotScales_->writeData(2, gram1_);
        } else if (atrname == "gram2") {
            float prev = gram2_;
            *reinterpret_cast<quint32 *>(&gram2_) = data;
            plotScales_->writeData(0, gram2_);

            // Show delta mix grams in the status bar
            QString text = QString::asprintf("%.1f", gram1_ - prev);
            emit signalTextToStatusBar(1, text);
        } else if (atrname == "gram2flt") {
            *reinterpret_cast<quint32 *>(&gram2flt_) = data;
            plotScales_->writeData(1, gram2flt_);
        }
    } else if (objname == "soil0") {
        if (atrname == "moisture") {
            plotMoisture_->writeData(0, static_cast<double>(data) / 10.0);
        }
    }
}

