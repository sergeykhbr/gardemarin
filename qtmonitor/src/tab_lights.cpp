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
#include <QHBoxLayout>

TabLights::TabLights(QWidget *parent)
    : QWidget(parent) {

    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    AttributeType slidersCfg;
    slidersCfg.from_config("["
          "{"
            "'Name':'Blue',"
            "'ObjName':'ledrbw',"
            "'AttrName':'duty0',"
            "'Active':true,"
            "'ColorStart':'#040B68',"
            "'ColorEnd':'#BDF1FD'"
          "},"
          "{"
            "'Name':'Unused',"
            "'ObjName':'ledrbw',"
            "'AttrName':'duty1',"
            "'Active':false,"
            "'ColorStart':'#101010',"
            "'ColorEnd':'#101010'"
          "},"
          "{"
            "'ObjName':'ledrbw',"
            "'AttrName':'duty2',"
            "'Name':'White',"
            "'Active':true,"
            "'ColorStart':'#404040',"
            "'ColorEnd':'#FFFFFF'"
          "},"
          "{"
            "'Name':'Red/Blue',"
            "'ObjName':'ledrbw',"
            "'AttrName':'duty3',"
            "'Active':true,"
            "'ColorStart':'#1A000F',"
            "'ColorEnd':'#F59BE7'"
          "}"
        "]");


    for (int i = 0; i < 4; i++) {
        slider_[i] = new PwmSlider(this, &slidersCfg[i]);

        connect(slider_[i], &PwmSlider::signalRequestReadAttribute, this,
                &TabLights::slotRequestReadAttribute);
        connect(slider_[i], &PwmSlider::signalRequestWriteAttribute, this,
                &TabLights::slotRequestWriteAttribute);
    }
    
    QGroupBox *dimmingGroup = new QGroupBox(this);
    dimmingGroup->setTitle(tr("LED Dimming"));
    QGridLayout *dimmingLayout = new QGridLayout(dimmingGroup);
    dimmingGroup->setLayout(dimmingLayout);

    dimmingLayout->addWidget(new QLabel(tr("Blue:"), this), 0, 0);
    dimmingLayout->addWidget(slider_[0], 0, 1);

    dimmingLayout->addWidget(new QLabel(tr("Unused:"), this), 1, 0);
    dimmingLayout->addWidget(slider_[1], 1, 1);

    dimmingLayout->addWidget(new QLabel(tr("White:"), this), 2, 0);
    dimmingLayout->addWidget(slider_[2], 2, 1);

    dimmingLayout->addWidget(new QLabel(tr("Red/Blue:"), this), 3, 0);
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

    liftLayout->addWidget(btnUp_, 0, 0, 1, 2);
    liftLayout->addWidget(labelLightsLift, 1, 0);
    liftLayout->addWidget(liftSpinBox, 1, 1);
    liftLayout->addWidget(btnDown_, 2, 0, 1, 2);


    layout->addWidget(dimmingGroup, 0, 0);
    layout->addWidget(liftGroup, 0, 1);

    QGroupBox *timeGroup = new QGroupBox(this);
    QHBoxLayout *timeLayout = new QHBoxLayout(timeGroup);
    timeGroup->setTitle(tr("Time"));
    timeGroup->setLayout(timeLayout);

    hours_ = new QSpinBox(timeGroup);
    hours_->setMinimum(0);
    hours_->setMaximum(23);
    hours_->setSingleStep(1);
    hours_->setValue(10);
    minutes_ = new QSpinBox(timeGroup);
    minutes_->setMinimum(0);
    minutes_->setMaximum(59);
    minutes_->setSingleStep(1);
    minutes_->setValue(30);
    btnSetTime_ = new QPushButton(timeGroup);
    btnSetTime_->setText(tr("Set"));
    QSpacerItem *timeSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);

    timeLayout->addWidget(hours_);
    timeLayout->addWidget(minutes_);
    timeLayout->addWidget(btnSetTime_);
    timeLayout->addItem(timeSpacer);

    layout->addWidget(timeGroup, 1, 0, 1, 2);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Expanding);
    layout->addItem(verticalSpacer, 2, 0, 1, 2);


    connect(btnDown_, &QPushButton::toggled, this,
            &TabLights::slotLightsMoveDown);

    connect(btnUp_, &QPushButton::toggled, this,
            &TabLights::slotLightsMoveUp);

    connect(btnSetTime_, &QPushButton::clicked, this,
            &TabLights::slotChangeTime);
}

void TabLights::showEvent(QShowEvent *ev) {
    QWidget::showEvent(ev);
    emit signalRequestReadAttribute(tr("rtc"), tr("Time"));
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

void TabLights::slotChangeTime() {
    int tm = 0;
    tm = (hours_->value() / 10) << 20;
    tm |= (hours_->value() % 10) << 16;
    tm |= (minutes_->value() / 10) << 12;
    tm |= (minutes_->value() % 10) << 8;

    emit signalRequestWriteAttribute(tr("rtc"), tr("Time"),
                    static_cast<quint32>(tm));
}

void TabLights::slotResponseAttribute(const QString &objname,
                                      const QString &atrname,
                                      quint32 data) {
    if (objname == "hbrg2") {
        if (atrname == "dc1_duty") {

        }
    } else if (objname == "rtc") {
        if (atrname == "Time") {
            quint32 h = 10 * ((data >> 20) & 0x3);
            h += ((data >> 16) & 0xf);

            quint32 m = 10 * ((data >> 12) & 0x7);
            m += ((data >> 8) & 0xf);

            hours_->setValue(static_cast<int>(h));
            minutes_->setValue(static_cast<int>(m));
        }
    }
}


