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

#include "tab_usrset.h"
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>

TabUserSettings::TabUserSettings(QWidget *parent)
    : QWidget(parent) {

    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);
    
    layout->addWidget(new QLabel(tr("LastServiceTime")), 0, 0);
    layout->addWidget(new QLabel(tr("RequestToService")), 1, 0);
    layout->addWidget(new QLabel(tr("WateringPerDrain")), 2, 0);
    layout->addWidget(new QLabel(tr("WateringInterval")), 3, 0);
    layout->addWidget(new QLabel(tr("WateringDuration")), 4, 0);
    layout->addWidget(new QLabel(tr("LastWatering")), 5 ,0);
    layout->addWidget(new QLabel(tr("OxygenSaturationInterval")), 6, 0);
    layout->addWidget(new QLabel(tr("DayStart")), 7, 0);
    layout->addWidget(new QLabel(tr("DayEnd")), 8, 0);

    //dimmingLayout->setColumnStretch(1, 11);




    //QSpinBox *liftSpinBox = new QSpinBox(liftGroup);
    //liftSpinBox->setMinimum(0);
    //liftSpinBox->setMaximum(20);
    //liftSpinBox->setSingleStep(1);
    //liftSpinBox->setValue(2);

    QSpacerItem *btnSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);
    btnUpdateSettings_ = new QPushButton(this);
    btnUpdateSettings_->setText(tr("Update"));

    layout->addItem(btnSpacer, 9, 0);
    layout->addWidget(btnUpdateSettings_, 9, 1);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Expanding);
    layout->addItem(verticalSpacer, 10, 0, 1, 2);


    connect(btnUpdateSettings_, &QPushButton::clicked, this,
            &TabUserSettings::slotUpdateUserSettings);
}

void TabUserSettings::showEvent(QShowEvent *ev) {
    QWidget::showEvent(ev);
    emit signalRequestReadAttribute(tr("rtc"), tr("Time"));
}

void TabUserSettings::slotUpdateUserSettings() {
    //quint32 duty;
    //emit signalRequestWriteAttribute(tr("usrset"), tr("WateringPerDrain"), duty);
}

void TabUserSettings::slotResponseAttribute(const QString &objname,
                                      const QString &atrname,
                                      quint32 data) {
    if (objname == "usrset") {
        if (atrname == "WateringPerDrain") {
            /*quint32 h = 10 * ((data >> 20) & 0x3);
            h += ((data >> 16) & 0xf);

            quint32 m = 10 * ((data >> 12) & 0x7);
            m += ((data >> 8) & 0xf);
            */
        }
    }
}


