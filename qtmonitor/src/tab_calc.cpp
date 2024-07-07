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

    Sources_ = (*cfg)["Sources"];       // not included SO3 and MgO
    Salts_ = (*cfg)["Salts"];
    AttributeType &receipt0 = (*cfg)["Receipts"][0u];
    AttributeType &receipt0Components = receipt0["Components"];
    AllSourceForms_.make_list(0);
    weight_ = receipt0["Weight"].to_float();

    boxSaltConcentration_ = new QDoubleSpinBox *[Salts_.size()];
    labelResult_ = new QLabel *[Sources_.size()];

    /* Header rows:
        row 0:      N           P    K   Ca   Mg   S
        row 1: <see at the end>
        row 2:      NH2 NO3 NH4 P2O5 ..
    */
    for (unsigned i = 0; i < Sources_.size(); i++) {
       
        layout->addWidget(
            new QLabel(QString::asprintf("%s", Sources_.dict_key(i)->to_string())),
            0, 2 + AllSourceForms_.size(),
            1, Sources_[i].size());

        for (unsigned n = 0; n < Sources_[i].size(); n++) {
            const char *srcName = Sources_[i][n].to_string();
            layout->addWidget(new QLabel(
                QString::asprintf("%s    ", srcName)), 2, 2 + AllSourceForms_.size());
            AllSourceForms_.add_to_list(&Sources_[i][n]);
        }
    }


    /* Computed constration NPK+

        row 1:  Weight, g:   190.0  34.1  ...
    */
    boxWeight_ = new QDoubleSpinBox(this);
    boxWeight_->setDecimals(0);
    boxWeight_->setMinimum(1000);
    boxWeight_->setMaximum(10000);
    boxWeight_->setValue(weight_);
    layout->addWidget(new QLabel(tr("Weight, g:")), 1, 0);
    layout->addWidget(boxWeight_, 1, 1);

    unsigned totalElements = 0;
    for (unsigned i = 0; i < Sources_.size(); i++) {
        labelResult_[i] = new QLabel(this);
        labelResult_[i]->setText(tr("0"));

        layout->addWidget(labelResult_[i],
            1, 2 + totalElements,
            1, Sources_[i].size());

        totalElements += Sources_[i].size();
    }
    connect(boxWeight_, &QDoubleSpinBox::valueChanged,
            this, &TabCalculator::slotWeightChanged);


    /*
       Salts concentration by component
    */
    for (unsigned saltidx = 0; saltidx < Salts_.size(); saltidx++) {
        AttributeType &saltName = *Salts_.dict_key(saltidx);
        AttributeType &saltComps = *Salts_.dict_value(saltidx);

        layout->addWidget(new QLabel(tr(saltName.to_string())), 3 + saltidx, 0);

        if (receipt0Components.has_key(saltName.to_string())) {
            boxSaltConcentration_[saltidx] = new QDoubleSpinBox(this);
            boxSaltConcentration_[saltidx]->setDecimals(2);
            boxSaltConcentration_[saltidx]->setValue(
                receipt0Components[saltName.to_string()].to_float());

            connect(boxSaltConcentration_[saltidx], &QDoubleSpinBox::valueChanged,
                    this, &TabCalculator::slotSaltConcentrationChanged);
            layout->addWidget(boxSaltConcentration_[saltidx], 3 + saltidx, 1);
        } else {
            boxSaltConcentration_[saltidx] = 0;
        }

        for (unsigned srcidx = 0; srcidx < AllSourceForms_.size(); srcidx++) {
            const char *srcFormName = AllSourceForms_[srcidx].to_string();
            if (saltComps.has_key(srcFormName)) {
                layout->addWidget(new QLabel(QString::asprintf("%.1f",
                        saltComps[srcFormName].to_float())), 3 + saltidx, 2 + srcidx);
            }
        }
    }



    QSpacerItem *verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Expanding);
    QSpacerItem *horizontalSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
    int RowTotal = Salts_.size() + 3;
    int ColTotal = AllSourceForms_.size() + 2;
    layout->addItem(verticalSpacer, RowTotal, 0, 1, ColTotal - 1);
    layout->addItem(horizontalSpacer, RowTotal, ColTotal, RowTotal, 1);

    slotSaltConcentrationChanged(0);
}

double TabCalculator::calculateResult(double weight, const char *src) {
    double ret = 0;
    double liter = weight / 1000.0;
    for (unsigned saltidx = 0; saltidx < Salts_.size(); saltidx++) {
        if (!boxSaltConcentration_[saltidx]) {
            continue;
        }

        for (unsigned formidx = 0; formidx < Sources_[src].size(); formidx++) {
            const char *srcFormName = Sources_[src][formidx].to_string();
            if (!Salts_.dict_value(saltidx)->has_key(srcFormName)) {
                continue;
            }
            ret += boxSaltConcentration_[saltidx]->value()
                    * (*Salts_.dict_value(saltidx))[srcFormName].to_float()
                    * PERCENTAGE_TO_MG_PER_L / liter;
        }
    }
    return ret;
}

void TabCalculator::slotSaltConcentrationChanged(double v) {
    for (unsigned i = 0; i < Sources_.size(); i++) {
        labelResult_[i]->setText(QString::asprintf("%.1f",
            calculateResult(weight_, Sources_.dict_key(i)->to_string())));
    }
}

void TabCalculator::slotWeightChanged(double v) {
    double rate = v / weight_;
    weight_ = v;
    for (unsigned saltidx = 0; saltidx < Salts_.size(); saltidx++) {
        if (!boxSaltConcentration_[saltidx]) {
            continue;
        }
        boxSaltConcentration_[saltidx]->setValue(
                rate * boxSaltConcentration_[saltidx]->value());
    }

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


