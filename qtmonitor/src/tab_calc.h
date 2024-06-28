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

#include <attribute.h>
#include <QTabWidget>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>
#include <QShowEvent>

class TabCalculator : public QWidget {
    Q_OBJECT

 public:
    explicit TabCalculator(QWidget *parent, AttributeType *cfg);

 signals:
    void signalRequestReadAttribute(const QString &objname, const QString &atrname);

 public slots:
    void slotResponseAttribute(const QString &objname, const QString &atrname, quint32 data);

 private slots:
    void slotSaltConcentrationChanged(double v);
    void slotWeightChanged(double v);

 protected:
    virtual void showEvent(QShowEvent *ev) override;

 private:
    QDoubleSpinBox *boxNitratK_;
    QDoubleSpinBox *boxNitratCa_;
    QDoubleSpinBox *boxMonophosphatK_;
    QDoubleSpinBox *boxSulfatMg_;
    QDoubleSpinBox *boxNitratPosphatAmmoni_;
    QDoubleSpinBox *boxCarbomid_;
    QDoubleSpinBox *boxWeight_;
    QLabel *editN_;
    QLabel *editP_;
    QLabel *editK_;
    QLabel *editCa_;
    QLabel *editMg_;
    QLabel *editS_;
    double weight_;
};


