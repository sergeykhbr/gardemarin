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

#pragma once

#include <QTabWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QShowEvent>

class TabUserSettings : public QWidget {
    Q_OBJECT

 public:
    explicit TabUserSettings(QWidget *parent);

 signals:
    void signalRequestReadAttribute(const QString &objname, const QString &atrname);
    void signalRequestWriteAttribute(const QString &objname, const QString &atrname, quint32 data);
    void signalResponseAttribute(const QString &objname, const QString &atrname, quint32 data);

 public slots:
    void slotResponseAttribute(const QString &objname, const QString &atrname, quint32 data);

 protected:
    virtual void showEvent(QShowEvent *ev) override;

 private slots:
    void slotUpdateUserSettings();
    void slotSettingsWasChanged(const QString &);

 private:
    QLineEdit *editLastServiceDate_;
    QLineEdit *editLastServiceTime_;
    QLineEdit *editRequestToService_;
    QLineEdit *editWateringPerDrain_;
    QLineEdit *editWateringInterval_;
    QLineEdit *editWateringDuration_;
    QLineEdit *editLastWatering_;
    QLineEdit *editOxygenSaturationInterval_;
    QLineEdit *editDayStart_;
    QLineEdit *editDayEnd_;
    QPushButton *btnUpdateSettings_;
    bool wasChanged_;
};


