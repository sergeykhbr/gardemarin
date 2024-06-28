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

static const double PERCENTAGE_TO_MG_PER_L = 1000.0 / 100.0;

TabCalculator::TabCalculator(QWidget *parent, AttributeType *cfg)
    : QWidget(parent) {

    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);
    AttributeType &receipt = (*cfg)["Receipts"][0u];
    AttributeType &components = receipt["Components"];

    weight_ = receipt["Weight"].to_float();

    enum ESaltComponentIndex {
        SaltName,
        Weight,
        N_NH2,
        N_NO3,
        N_NH4,
        P2O5,
        K2O,
        Ca,
        MgO,
        Mg,
        SO3,
        S,
        ElemTotal
    };
    enum ERowIndex {
        RowHeader,
        Carbomid,
        NitratPhosfatAmonio,
        NitratCa,
        NitratK,
        MonoK,
        SulfatMg,
        TextNPK,
        ValueNPKChesnokov,
        ValueNPKKnop,
        ValueNPK,
        RowTotal
    };
    
    layout->addWidget(new QLabel(tr("N-NH2    ")), RowHeader, N_NH2);
    layout->addWidget(new QLabel(tr("N-NO3    ")), RowHeader, N_NO3);
    layout->addWidget(new QLabel(tr("N-NH4    ")), RowHeader, N_NH4);
    layout->addWidget(new QLabel(tr("P2O5    ")), RowHeader, P2O5);
    layout->addWidget(new QLabel(tr("K2O    ")), RowHeader, K2O);
    layout->addWidget(new QLabel(tr("Ca    ")), RowHeader, Ca);
    layout->addWidget(new QLabel(tr("MgO    ")), RowHeader, MgO);
    layout->addWidget(new QLabel(tr("Mg    ")), RowHeader, Mg);
    layout->addWidget(new QLabel(tr("SO3    ")), RowHeader, SO3);
    layout->addWidget(new QLabel(tr("S    ")), RowHeader, S);

    boxNitratK_ = new QDoubleSpinBox(this);
    boxNitratK_->setDecimals(2);
    boxNitratK_->setMinimum(0.0);
    boxNitratK_->setValue(components["Potassium nitrate"].to_float());
    layout->addWidget(new QLabel(tr("Potassium nitrate 13-0-46:")), NitratK, SaltName);
    layout->addWidget(boxNitratK_, NitratK, Weight);
    layout->addWidget(new QLabel(tr("13.5")), NitratK, N_NO3);
    layout->addWidget(new QLabel(tr("46.2")), NitratK, K2O);
    
    boxNitratCa_ = new QDoubleSpinBox(this);
    boxNitratCa_->setDecimals(2);
    boxNitratCa_->setMinimum(0.0);
    boxNitratCa_->setValue(components["Calcium nitrate"].to_float());
    layout->addWidget(new QLabel(tr("Calcium nitrate 15-0-0+19:")), NitratCa, SaltName);
    layout->addWidget(boxNitratCa_, NitratCa, Weight);
    layout->addWidget(new QLabel(tr("14.4")), NitratCa, N_NO3);
    layout->addWidget(new QLabel(tr("1.1")), NitratCa, N_NH4);
    layout->addWidget(new QLabel(tr("19.0")), NitratCa, Ca);

    boxMonophosphatK_ = new QDoubleSpinBox(this);
    boxMonophosphatK_->setDecimals(2);
    boxMonophosphatK_->setMinimum(0.0);
    boxMonophosphatK_->setValue(components["Monopotassium phosphate"].to_float());
    layout->addWidget(new QLabel(tr("Monopotassium phosphate 0-52-34:")), MonoK, SaltName);
    layout->addWidget(boxMonophosphatK_, MonoK, Weight);
    layout->addWidget(new QLabel(tr("52.0")), MonoK, P2O5);
    layout->addWidget(new QLabel(tr("34.0")), MonoK, K2O);

    boxSulfatMg_ = new QDoubleSpinBox(this);
    boxSulfatMg_->setDecimals(2);
    boxSulfatMg_->setMinimum(0.0);
    boxSulfatMg_->setValue(components["Magnesium sulfate"].to_float());
    layout->addWidget(new QLabel(tr("Magnesium sulfate 0-0-0+16+32.5:")), SulfatMg, SaltName);
    layout->addWidget(boxSulfatMg_, SulfatMg, Weight);
    layout->addWidget(new QLabel(tr("16.0")), SulfatMg, MgO);
    layout->addWidget(new QLabel(tr("9.6")), SulfatMg, Mg);
    layout->addWidget(new QLabel(tr("32.5")), SulfatMg, SO3);
    layout->addWidget(new QLabel(tr("13.0")), SulfatMg, S);

    boxNitratPosphatAmmoni_ = new QDoubleSpinBox(this);
    boxNitratPosphatAmmoni_->setDecimals(2);
    boxNitratPosphatAmmoni_->setMinimum(0.0);
    boxNitratPosphatAmmoni_->setValue(components["Ammonium nitrate phosphate"].to_float());
    layout->addWidget(new QLabel(tr("Ammonium nitrate phosphate 26-0-0+14.8:")), NitratPhosfatAmonio, SaltName);
    layout->addWidget(boxNitratPosphatAmmoni_, NitratPhosfatAmonio, Weight);
    layout->addWidget(new QLabel(tr("6.5")), NitratPhosfatAmonio, N_NO3);
    layout->addWidget(new QLabel(tr("19.5")), NitratPhosfatAmonio, N_NH4);
    layout->addWidget(new QLabel(tr("14.8")), NitratPhosfatAmonio, S);

    boxCarbomid_ = new QDoubleSpinBox(this);
    boxCarbomid_->setDecimals(2);
    boxCarbomid_->setMinimum(0.0);
    boxCarbomid_->setValue(components["Carbomid"].to_float());
    layout->addWidget(new QLabel(tr("Carbomid 46-0-0:")), Carbomid, SaltName);
    layout->addWidget(boxCarbomid_, Carbomid, Weight);
    layout->addWidget(new QLabel(tr("46.5")), Carbomid, N_NH2);


    layout->addWidget(new QLabel(tr("N")), TextNPK, N_NH2, 1, 3);
    layout->addWidget(new QLabel(tr("P")), TextNPK, P2O5);
    layout->addWidget(new QLabel(tr("K")), TextNPK, K2O);
    layout->addWidget(new QLabel(tr("Ca")), TextNPK, Ca);
    layout->addWidget(new QLabel(tr("Mg")), TextNPK, MgO, 1, 2);
    layout->addWidget(new QLabel(tr("S")), TextNPK, SO3, 1, 2);

    layout->addWidget(new QLabel(tr("Chesnokov:")), ValueNPKChesnokov, 0);
    layout->addWidget(new QLabel(tr("105+35")), ValueNPKChesnokov, N_NH2, 1, 3);
    layout->addWidget(new QLabel(tr("38.5")), ValueNPKChesnokov, P2O5);
    layout->addWidget(new QLabel(tr("190")), ValueNPKChesnokov, K2O);
    layout->addWidget(new QLabel(tr("164")), ValueNPKChesnokov, Ca);
    layout->addWidget(new QLabel(tr("30")), ValueNPKChesnokov, MgO, 1, 2);

    layout->addWidget(new QLabel(tr("Knop:")), ValueNPKKnop, 0);
    layout->addWidget(new QLabel(tr("154")), ValueNPKKnop, N_NH2, 1, 3);
    layout->addWidget(new QLabel(tr("56")), ValueNPKKnop, P2O5);
    layout->addWidget(new QLabel(tr("167")), ValueNPKKnop, K2O);
    layout->addWidget(new QLabel(tr("170")), ValueNPKKnop, Ca);
    layout->addWidget(new QLabel(tr("24")), ValueNPKKnop, MgO, 1, 2);

    boxWeight_ = new QDoubleSpinBox(this);
    boxWeight_->setDecimals(0);
    boxWeight_->setMinimum(1000);
    boxWeight_->setMaximum(10000);
    boxWeight_->setValue(weight_);
    layout->addWidget(new QLabel(tr("Weight, g:")), ValueNPK, 0);
    layout->addWidget(boxWeight_, ValueNPK, 1);

    editN_ = new QLabel(this);
    editN_->setText(tr("0"));
    layout->addWidget(editN_, ValueNPK, N_NH2, 1, 3);

    editP_ = new QLabel(this);
    editP_->setText(tr("0"));
    layout->addWidget(editP_, ValueNPK, P2O5);

    editK_ = new QLabel(this);
    editK_->setText(tr("0"));
    layout->addWidget(editK_, ValueNPK, K2O);

    editCa_ = new QLabel(this);
    editCa_->setText(tr("0"));
    layout->addWidget(editCa_, ValueNPK, Ca);

    editMg_ = new QLabel(this);
    editMg_->setText(tr("0"));
    layout->addWidget(editMg_, ValueNPK, MgO, 1, 2);

    editS_ = new QLabel(this);
    editS_->setText(tr("0"));
    layout->addWidget(editS_, ValueNPK, SO3, 1, 2);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Expanding);
    QSpacerItem *horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addItem(verticalSpacer, RowTotal, 0, 1, ElemTotal-1);
    layout->addItem(horizontalSpacer, RowTotal, ElemTotal, RowTotal, 1);

    //layout->setColumnStretch(0, 5);
    //layout->setColumnStretch(1, 2);
    //layout->setColumnStretch(2, 5);

    connect(boxNitratK_, &QDoubleSpinBox::valueChanged,
            this, &TabCalculator::slotSaltConcentrationChanged);
    connect(boxNitratCa_, &QDoubleSpinBox::valueChanged,
            this, &TabCalculator::slotSaltConcentrationChanged);
    connect(boxMonophosphatK_, &QDoubleSpinBox::valueChanged,
            this, &TabCalculator::slotSaltConcentrationChanged);
    connect(boxSulfatMg_, &QDoubleSpinBox::valueChanged,
            this, &TabCalculator::slotSaltConcentrationChanged);
    connect(boxNitratPosphatAmmoni_, &QDoubleSpinBox::valueChanged,
            this, &TabCalculator::slotSaltConcentrationChanged);
    connect(boxCarbomid_, &QDoubleSpinBox::valueChanged,
            this, &TabCalculator::slotSaltConcentrationChanged);
    connect(boxWeight_, &QDoubleSpinBox::valueChanged,
            this, &TabCalculator::slotWeightChanged);

    slotSaltConcentrationChanged(0);
}

void TabCalculator::slotSaltConcentrationChanged(double v) {
    double valN = 0;
    double valK = 0;
    double valP = 0;
    double valCa = 0;
    double valMg = 0;
    double valS = 0;
    double liter = boxWeight_->value() / 1000.0;

    valN += boxNitratK_->value() * 13.5 * PERCENTAGE_TO_MG_PER_L / liter;
    valK += boxNitratK_->value() * 46.2 * PERCENTAGE_TO_MG_PER_L / liter;

    valN += boxNitratCa_->value() * 14.4 * PERCENTAGE_TO_MG_PER_L / liter;
    valN += boxNitratCa_->value() * 1.1 * PERCENTAGE_TO_MG_PER_L / liter;
    valCa += boxNitratCa_->value() * 19.0 * PERCENTAGE_TO_MG_PER_L / liter;

    valP += boxMonophosphatK_->value() * 52.0 * PERCENTAGE_TO_MG_PER_L / liter;
    valK += boxMonophosphatK_->value() * 34.0 * PERCENTAGE_TO_MG_PER_L / liter;

    valMg += boxSulfatMg_->value() * 9.6 * PERCENTAGE_TO_MG_PER_L / liter;  // MgO 16.0
    valS += boxSulfatMg_->value() * 13.0 * PERCENTAGE_TO_MG_PER_L / liter;  // SO3 32.5

    valN += boxNitratPosphatAmmoni_->value() * 6.5 * PERCENTAGE_TO_MG_PER_L / liter;
    valN += boxNitratPosphatAmmoni_->value() * 19.5 * PERCENTAGE_TO_MG_PER_L / liter;
    valS += boxNitratPosphatAmmoni_->value() * 14.8 * PERCENTAGE_TO_MG_PER_L / liter;

    valN += boxCarbomid_->value() * 46.5 * PERCENTAGE_TO_MG_PER_L / liter;

    editN_->setText(QString::asprintf("%.2f", valN));
    editP_->setText(QString::asprintf("%.2f", valP));
    editK_->setText(QString::asprintf("%.2f", valK));
    editCa_->setText(QString::asprintf("%.2f", valCa));
    editMg_->setText(QString::asprintf("%.2f", valMg));
    editS_->setText(QString::asprintf("%.2f", valS));
}

void TabCalculator::slotWeightChanged(double v) {
    double rate = v / weight_;
    weight_ = v;

    boxNitratK_->setValue(rate * boxNitratK_->value());
    boxNitratCa_->setValue(rate * boxNitratCa_->value());
    boxMonophosphatK_->setValue(rate * boxMonophosphatK_->value());
    boxSulfatMg_->setValue(rate * boxSulfatMg_->value());
    boxNitratPosphatAmmoni_->setValue(rate * boxNitratPosphatAmmoni_->value());
    boxCarbomid_->setValue(rate * boxCarbomid_->value());

    slotSaltConcentrationChanged(0);
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


