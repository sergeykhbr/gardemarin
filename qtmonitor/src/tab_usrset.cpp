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

    editLastServiceDate_ = new QLineEdit(this);
    editLastServiceDate_->setReadOnly(true);
    layout->addWidget(new QLabel(tr("LastServiceDate")), 0, 0);
    layout->addWidget(editLastServiceDate_, 0, 1);
    
    editLastServiceTime_ = new QLineEdit(this);
    editLastServiceTime_->setReadOnly(true);
    layout->addWidget(new QLabel(tr("LastServiceTime")), 1, 0);
    layout->addWidget(editLastServiceTime_, 1, 1);

    editWateringPerDrain_ = new QLineEdit(this);
    layout->addWidget(new QLabel(tr("WateringPerDrain")), 2, 0);
    layout->addWidget(editWateringPerDrain_, 2, 1);

    editWateringInterval_ = new QLineEdit(this);
    layout->addWidget(new QLabel(tr("WateringInterval")), 3, 0);
    layout->addWidget(editWateringInterval_, 3, 1);

    editWateringDuration_ = new QLineEdit(this);
    layout->addWidget(new QLabel(tr("WateringDuration")), 4, 0);
    layout->addWidget(editWateringDuration_, 4, 1);

    editLastWatering_ = new QLineEdit(this);
    editLastWatering_->setReadOnly(true);
    layout->addWidget(new QLabel(tr("LastWatering")), 5 ,0);
    layout->addWidget(editLastWatering_, 5, 1);

    editOxygenSaturationInterval_ = new QLineEdit(this);
    layout->addWidget(new QLabel(tr("OxygenSaturationInterval")), 6, 0);
    layout->addWidget(editOxygenSaturationInterval_, 6, 1);

    editDayStart_ = new QLineEdit(this);
    layout->addWidget(new QLabel(tr("DayStart")), 7, 0);
    layout->addWidget(editDayStart_, 7, 1);

    editDayEnd_ = new QLineEdit(this);
    layout->addWidget(new QLabel(tr("DayEnd")), 8, 0);
    layout->addWidget(editDayEnd_, 8, 1);


    QSpacerItem *btnSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);
    btnUpdateSettings_ = new QPushButton(this);
    btnUpdateSettings_->setText(tr("Update"));
    layout->addWidget(btnUpdateSettings_, 9, 0);

    layout->addItem(btnSpacer, 9, 1);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Expanding);
    QSpacerItem *horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addItem(verticalSpacer, 10, 0, 1, 2);
    layout->addItem(horizontalSpacer, 0, 2, 10, 1);

    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 2);
    layout->setColumnStretch(2, 5);


    wasChanged_ = false;
    connect(editWateringPerDrain_, &QLineEdit::textChanged, this,
            &TabUserSettings::slotSettingsWasChanged);
    connect(editWateringInterval_, &QLineEdit::textChanged, this,
            &TabUserSettings::slotSettingsWasChanged);
    connect(editWateringDuration_, &QLineEdit::textChanged, this,
            &TabUserSettings::slotSettingsWasChanged);
    connect(editLastWatering_, &QLineEdit::textChanged, this,
            &TabUserSettings::slotSettingsWasChanged);
    connect(editOxygenSaturationInterval_, &QLineEdit::textChanged, this,
            &TabUserSettings::slotSettingsWasChanged);
    connect(editDayStart_, &QLineEdit::textChanged, this,
            &TabUserSettings::slotSettingsWasChanged);
    connect(editDayEnd_, &QLineEdit::textChanged, this,
            &TabUserSettings::slotSettingsWasChanged);

    connect(btnUpdateSettings_, &QPushButton::clicked, this,
            &TabUserSettings::slotUpdateUserSettings);
}

void TabUserSettings::showEvent(QShowEvent *ev) {
    QWidget::showEvent(ev);
    emit signalRequestReadAttribute(tr("usrset"), tr("LastServiceDate"));
    emit signalRequestReadAttribute(tr("usrset"), tr("LastServiceTime"));
    emit signalRequestReadAttribute(tr("usrset"), tr("WateringPerDrain"));
    emit signalRequestReadAttribute(tr("usrset"), tr("WateringInterval"));
    emit signalRequestReadAttribute(tr("usrset"), tr("WateringDuration"));
    emit signalRequestReadAttribute(tr("usrset"), tr("LastWatering"));
    emit signalRequestReadAttribute(tr("usrset"), tr("OxygenSaturationInterval"));
    emit signalRequestReadAttribute(tr("usrset"), tr("DayStart"));
    emit signalRequestReadAttribute(tr("usrset"), tr("DayEnd"));
    emit signalRequestReadAttribute(tr("usrset"), tr("State"));
}

void TabUserSettings::slotSettingsWasChanged(const QString &) {
    wasChanged_ = true;
}

void TabUserSettings::slotUpdateUserSettings() {
    if (!wasChanged_) {
        return;
    }
    quint32 t1;
    t1 = editWateringPerDrain_->text().toInt();
    emit signalRequestWriteAttribute(tr("usrset"), tr("WateringPerDrain"), t1);
    t1 = editWateringInterval_->text().toInt();
    emit signalRequestWriteAttribute(tr("usrset"), tr("WateringInterval"), t1);
    t1 = editWateringDuration_->text().toInt();
    emit signalRequestWriteAttribute(tr("usrset"), tr("WateringDuration"), t1);
    t1 = editLastWatering_->text().toInt();
    emit signalRequestWriteAttribute(tr("usrset"), tr("OxygenSaturationInterval"), t1);
    t1 = editDayStart_->text().toInt();
    emit signalRequestWriteAttribute(tr("usrset"), tr("DayStart"), t1);
    t1 = editDayEnd_->text().toInt();
    emit signalRequestWriteAttribute(tr("usrset"), tr("DayEnd"), t1);
}

quint32 toBCD8(quint32 v) {
    quint32 ret = 10 * ((v >> 4) & 0xf);
    ret += (v & 0xf);
    return ret;
}

void TabUserSettings::slotResponseAttribute(const QString &objname,
                                      const QString &atrname,
                                      quint32 data) {
    if (objname == "usrset") {
        wasChanged_ = false;

        if (atrname == "LastServiceDate") {
            editLastServiceDate_->setText(QString::asprintf("%02d/%02d/%02d",
                toBCD8(data >> 16), toBCD8(data >> 8), toBCD8(data)));
        } else if (atrname == "LastServiceTime") {
            editLastServiceTime_->setText(QString::asprintf("%02d:%02d:%02d",
                toBCD8(data >> 16), toBCD8(data >> 8), toBCD8(data)));
        } else if (atrname == "WateringPerDrain") {
            editWateringPerDrain_->setText(QString::asprintf("%d", data));
        } else if (atrname == "WateringInterval") {
            editWateringInterval_->setText(QString::asprintf("%d", data));
        } else if (atrname == "WateringDuration") {
            editWateringDuration_->setText(QString::asprintf("%d", data));
        } else if (atrname == "LastWatering") {
            editLastWatering_->setText(QString::asprintf("%02d:%02d:%02d",
                toBCD8(data >> 16), toBCD8(data >> 8), toBCD8(data)));
        } else if (atrname == "OxygenSaturationInterval") {
            editOxygenSaturationInterval_->setText(QString::asprintf("%d", data));
        } else if (atrname == "DayStart") {
            editDayStart_->setText(QString::asprintf("%d", data));
        } else if (atrname == "DayEnd") {
            editDayEnd_->setText(QString::asprintf("%d", data));
        }
    }
}


