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

#include "tab_lights.h"
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>

TabLights::TabLights(QWidget *parent)
    : QWidget(parent) {

    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    for (int i = 0; i < 4; i++) {
        slider_[i] = new QSlider(this);
        slider_[i]->setMaximum(100);
        slider_[i]->setMinimum(0);
        slider_[i]->setPageStep(10);
        slider_[i]->setTickPosition(QSlider::TicksAbove);
        slider_[i]->setOrientation(Qt::Horizontal);
    }
    
    QGroupBox *dimmingGroup = new QGroupBox(this);
    dimmingGroup->setTitle(tr("LED Dimming"));
    QGridLayout *dimmingLayout = new QGridLayout(dimmingGroup);
    dimmingGroup->setLayout(dimmingLayout);

    dimmingLayout->addWidget(new QLabel(tr("Red:"), this), 0, 0);
    dimmingLayout->addWidget(slider_[0], 0, 1);

    dimmingLayout->addWidget(new QLabel(tr("Blue:"), this), 1, 0);
    dimmingLayout->addWidget(slider_[1], 1, 1);

    dimmingLayout->addWidget(new QLabel(tr("White:"), this), 2, 0);
    dimmingLayout->addWidget(slider_[2], 2, 1);

    dimmingLayout->addWidget(new QLabel(tr("Unused:"), this), 3, 0);
    dimmingLayout->addWidget(slider_[3], 3, 1);


    QGroupBox *liftGroup = new QGroupBox(this);
    QGridLayout *liftLayout = new QGridLayout(liftGroup);
    liftGroup->setTitle(tr("Lift"));
    liftGroup->setLayout(liftLayout);

    QLabel *labelLightsLift = new QLabel(tr("Move, [cm]: "), liftGroup);
    QSpinBox *liftSpinBox = new QSpinBox(liftGroup);
    liftSpinBox->setMinimum(0);
    liftSpinBox->setMaximum(20);
    liftSpinBox->setSingleStep(1);
    liftSpinBox->setValue(2);

    btnUp_ = new QPushButton(liftGroup);
    btnUp_->setText(tr("Up"));
    btnUp_->setCheckable(true);
    btnDown_ = new QPushButton(liftGroup);
    btnDown_->setText(tr("Down"));
    btnDown_->setCheckable(true);

    liftLayout->addWidget(labelLightsLift, 0, 0, 2, 1);
    liftLayout->addWidget(liftSpinBox, 0, 1, 2, 1);
    liftLayout->addWidget(btnUp_, 0, 2);
    liftLayout->addWidget(btnDown_, 1, 2);


    layout->addWidget(dimmingGroup, 0, 0);
    layout->addWidget(liftGroup, 0, 1);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Expanding);
    layout->addItem(verticalSpacer, 1, 0, 1, 2);


    connect(slider_[0], &QAbstractSlider::valueChanged, this,
            &TabLights::slotChangeDim0);

    connect(slider_[1], &QAbstractSlider::valueChanged, this,
            &TabLights::slotChangeDim1);

    connect(slider_[2], &QAbstractSlider::valueChanged, this,
            &TabLights::slotChangeDim2);

    connect(slider_[3], &QAbstractSlider::valueChanged, this,
            &TabLights::slotChangeDim3);

    connect(btnDown_, &QPushButton::toggled, this,
            &TabLights::slotLightsMoveDown);

    connect(btnUp_, &QPushButton::toggled, this,
            &TabLights::slotLightsMoveUp);
}

void TabLights::slotChangeDim0(int idx) {
    emit signalRequestWriteAttribute(tr("ledrbw"), tr("duty0"), static_cast<quint32>(idx));
}

void TabLights::slotChangeDim1(int idx) {
    emit signalRequestWriteAttribute(tr("ledrbw"), tr("duty1"), static_cast<quint32>(idx));
}

void TabLights::slotChangeDim2(int idx) {
    emit signalRequestWriteAttribute(tr("ledrbw"), tr("duty2"), static_cast<quint32>(idx));
}

void TabLights::slotChangeDim3(int idx) {
    emit signalRequestWriteAttribute(tr("ledrbw"), tr("duty3"), static_cast<quint32>(idx));
}

void TabLights::slotLightsMoveUp(bool checked) {
    // direction backward
    emit signalRequestWriteAttribute(tr("hbrg2"), tr("dc1_direction"), static_cast<quint32>(1));
    emit signalRequestWriteAttribute(tr("hbrg2"), tr("dc1_duty"), 100*static_cast<quint32>(checked));
}

void TabLights::slotLightsMoveDown(bool checked) {
    // direction forward
    emit signalRequestWriteAttribute(tr("hbrg2"), tr("dc1_direction"), static_cast<quint32>(0));
    emit signalRequestWriteAttribute(tr("hbrg2"), tr("dc1_duty"), 100*static_cast<quint32>(checked));
}

void TabLights::slotResponseAttribute(const QString &objname, const QString &atrname, quint32 data) {
    if (objname == "ledrbw") {
        if (atrname == "duty0") {
            slider_[0]->setValue(data);
        } else if (atrname == "duty1") {
            slider_[1]->setValue(data);
        } else if (atrname == "duty2") {
            slider_[2]->setValue(data);
        } else if (atrname == "duty3") {
            slider_[3]->setValue(data);
        }
    } else if (objname == "hbrg2") {
        if (atrname == "dc1_duty") {

        }
    }
}

