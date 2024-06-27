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

#include "tab_calc.h"
#include <QGridLayout>
#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>

TabCalculator::TabCalculator(QWidget *parent)
    : QWidget(parent) {

    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);

    
    boxNitratK_ = new QSpinBox(this);
    layout->addWidget(new QLabel(tr("NitratK 13-0-46:")), 0, 0);
    layout->addWidget(boxNitratK_, 0, 1);
    layout->addWidget(new QLabel(tr("N-NO3 13.5%")), 0, 2);
    layout->addWidget(new QLabel(tr("K2O3 46.2%")), 0, 3);
    
    boxNitratCa_ = new QSpinBox(this);
    layout->addWidget(new QLabel(tr("NitratCa 15-0-0 + 19:")), 1, 0);
    layout->addWidget(boxNitratCa_, 1, 1);
    layout->addWidget(new QLabel(tr("N-NO3 14.4%")), 1, 2);
    layout->addWidget(new QLabel(tr("N-NH4 1.1%")), 1, 3);
    layout->addWidget(new QLabel(tr("Ca 19.0%")), 1, 4);

    boxMonophosphatK_ = new QSpinBox(this);
    layout->addWidget(new QLabel(tr("Phosfat MonoK 0-52-34:")), 2, 0);
    layout->addWidget(boxMonophosphatK_, 2, 1);
    layout->addWidget(new QLabel(tr("P2O3 52.0%")), 2, 2);
    layout->addWidget(new QLabel(tr("K2O 34.0%")), 2, 3);

    boxSulfatMg_ = new QSpinBox(this);
    layout->addWidget(new QLabel(tr("Sulfat Mg 0-0-0 + 16+32.5:")), 3, 0);
    layout->addWidget(boxSulfatMg_, 3, 1);
    layout->addWidget(new QLabel(tr("MgO 16.0%")), 3, 2);
    layout->addWidget(new QLabel(tr("Mg 9.6%")), 3, 3);
    layout->addWidget(new QLabel(tr("SO3 32.5%")), 3, 4);
    layout->addWidget(new QLabel(tr("S 13.0%")), 3, 5);

    boxNitratPosphatAmmoni_ = new QSpinBox(this);
    layout->addWidget(new QLabel(tr("Nitrat-phosfat Ammonio 26-0-0+14.8:")), 4, 0);
    layout->addWidget(boxNitratPosphatAmmoni_, 4, 1);
    layout->addWidget(new QLabel(tr("NO3-N 6.5%")), 4, 2);
    layout->addWidget(new QLabel(tr("NH4-N 19.5%")), 4, 3);
    layout->addWidget(new QLabel(tr("S 14.8%")), 4, 5);


    QSpacerItem *verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Expanding);
    QSpacerItem *horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addItem(verticalSpacer, 10, 0, 1, 2);
    layout->addItem(horizontalSpacer, 0, 2, 10, 1);

    //layout->setColumnStretch(0, 1);
    //layout->setColumnStretch(1, 2);
    //layout->setColumnStretch(2, 5);
}

void TabCalculator::showEvent(QShowEvent *ev) {
    QWidget::showEvent(ev);
//    emit signalRequestReadAttribute(tr("usrset"), tr("LastServiceDate"));
}


void TabCalculator::slotResponseAttribute(const QString &objname,
                                          const QString &atrname,
                                          quint32 data) {
    if (objname == "scales0") {
        
        if (atrname == "gram") {
        }
    }
}


