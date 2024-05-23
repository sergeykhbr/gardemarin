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

#include "dlgserialsettings.h"
#include <QIntValidator>
#include <QLineEdit>
#include <QSerialPortInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QComboBox>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

DialogSerialSettings::DialogSerialSettings(QWidget *parent, SerialPortSettings *settings) :
    QDialog(parent),
    settings_(settings),
    m_intValidator(new QIntValidator(0, 4000000, this))
{
    //setGeometry(QRect(0, 0, 281, 262));
    setWindowTitle("Settings");
    setWindowIcon(QIcon(":/images/settings.png"));
    QGridLayout *grigLayout_3 = new QGridLayout(this);
    grigLayout_3->setSpacing(4);
    grigLayout_3->setContentsMargins(4, 4, 4, 4);
    setLayout(grigLayout_3);

    QGroupBox *parametersBox = new QGroupBox();
    parametersBox->setTitle("Select Parameters");

    QGridLayout *gridLayout_2 = new QGridLayout(parametersBox);
    parametersBox->setLayout(gridLayout_2);

    QLabel *baudRateLabel = new QLabel();
    baudRateLabel->setText("Baud rate:");
    baudRateBox = new QComboBox();
    baudRateBox->setInsertPolicy(QComboBox::NoInsert);

    QLabel *dataBitsLabel = new QLabel();
    dataBitsLabel->setText("Data bits:");
    dataBitsBox = new QComboBox();

    QLabel *parityLabel = new QLabel();
    parityLabel->setText("Parity bits:");
    parityBox = new QComboBox();

    QLabel *stopBitsLabel = new QLabel();
    stopBitsLabel->setText("Stop bits:");
    stopBitsBox = new QComboBox();

    QLabel *flowControlLabel = new QLabel();
    flowControlLabel->setText("Flow control:");
    flowControlBox = new QComboBox();

    localEchoCheckBox = new QCheckBox();
    localEchoCheckBox->setText("Local echo");
    localEchoCheckBox->setChecked(true);


    gridLayout_2->addWidget(baudRateLabel, 0, 0);
    gridLayout_2->addWidget(baudRateBox, 0, 1);
    gridLayout_2->addWidget(dataBitsLabel, 1, 0);
    gridLayout_2->addWidget(dataBitsBox, 1, 1);
    gridLayout_2->addWidget(parityLabel, 2, 0);
    gridLayout_2->addWidget(parityBox, 2, 1);
    gridLayout_2->addWidget(stopBitsLabel, 3, 0);
    gridLayout_2->addWidget(stopBitsBox, 3, 1);
    gridLayout_2->addWidget(flowControlLabel, 4, 0);
    gridLayout_2->addWidget(flowControlBox, 4, 1);
    gridLayout_2->addWidget(localEchoCheckBox, 5, 1);

    QGroupBox *selectBox = new QGroupBox();
    selectBox->setTitle("Select Serial Port");

    QGridLayout *gridLayout_1 = new QGridLayout(selectBox);
    selectBox->setLayout(gridLayout_1);

    serialPortInfoListBox = new QComboBox();
    
    descriptionLabel = new QLabel();
    descriptionLabel->setText("Description:");

    manufacturerLabel = new QLabel();
    manufacturerLabel->setText("Manufacturer:");

    serialNumberLabel = new QLabel();
    serialNumberLabel->setText("Serial Number:");

    locationLabel = new QLabel();
    locationLabel->setText("Location:");

    vidLabel = new QLabel();
    vidLabel->setText("Vendor ID:");

    pidLabel = new QLabel();
    pidLabel->setText("Product ID:");

    gridLayout_1->addWidget(serialPortInfoListBox, 0, 0);
    gridLayout_1->addWidget(descriptionLabel, 1, 0);
    gridLayout_1->addWidget(manufacturerLabel, 2, 0);
    gridLayout_1->addWidget(serialNumberLabel, 3, 0);
    gridLayout_1->addWidget(locationLabel, 4, 0);
    gridLayout_1->addWidget(vidLabel, 5, 0);
    gridLayout_1->addWidget(pidLabel, 6, 0);

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    QWidget *horizontalSpacer = new QWidget();
    horizontalSpacer->setLayout(horizontalLayout);
    horizontalSpacer->setMinimumWidth(96);
    horizontalSpacer->setMinimumHeight(20);

    QPushButton *applyButton = new QPushButton();
    applyButton->setText("Apply");
    horizontalLayout->addWidget(applyButton);
    horizontalLayout->addStretch();
    applyButton->setMinimumSize(96, 20);


    grigLayout_3->addWidget(parametersBox, 0, 1);
    grigLayout_3->addWidget(selectBox, 0, 0);
    grigLayout_3->addWidget(horizontalSpacer, 2, 0, 1, 2);


    connect(applyButton, &QPushButton::clicked,
            this, &DialogSerialSettings::apply);
    connect(serialPortInfoListBox, &QComboBox::currentIndexChanged,
            this, &DialogSerialSettings::showPortInfo);
    connect(baudRateBox,  &QComboBox::currentIndexChanged,
            this, &DialogSerialSettings::checkCustomBaudRatePolicy);
    connect(serialPortInfoListBox, &QComboBox::currentIndexChanged,
            this, &DialogSerialSettings::checkCustomDevicePathPolicy);

    fillPortsParameters();
    fillPortsInfo();

    updateSettings();
}

void DialogSerialSettings::showPortInfo(int idx)
{
    if (idx == -1) {
        return;
    }

    const QString blankString = tr(::blankString);

    const QStringList list = serialPortInfoListBox->itemData(idx).toStringList();
    descriptionLabel->setText(tr("Description: %1").arg(list.value(1, blankString)));
    manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.value(2, blankString)));
    serialNumberLabel->setText(tr("Serial number: %1").arg(list.value(3, blankString)));
    locationLabel->setText(tr("Location: %1").arg(list.value(4, blankString)));
    vidLabel->setText(tr("Vendor Identifier: %1").arg(list.value(5, blankString)));
    pidLabel->setText(tr("Product Identifier: %1").arg(list.value(6, blankString)));
}

void DialogSerialSettings::apply()
{
    updateSettings();
    hide();
}

void DialogSerialSettings::checkCustomBaudRatePolicy(int idx)
{
    const bool isCustomBaudRate = !baudRateBox->itemData(idx).isValid();
    baudRateBox->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        baudRateBox->clearEditText();
        QLineEdit *edit = baudRateBox->lineEdit();
        edit->setValidator(m_intValidator);
    }
}

void DialogSerialSettings::checkCustomDevicePathPolicy(int idx)
{
    const bool isCustomPath = !serialPortInfoListBox->itemData(idx).isValid();
    serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath) {
        serialPortInfoListBox->clearEditText();
    }
}

void DialogSerialSettings::fillPortsParameters()
{
    baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    baudRateBox->addItem(tr("Custom"));
    baudRateBox->setCurrentIndex(3);

    dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    dataBitsBox->setCurrentIndex(3);

    parityBox->addItem(tr("None"), QSerialPort::NoParity);
    parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}

void DialogSerialSettings::fillPortsInfo()
{
    serialPortInfoListBox->clear();
    const QString blankString = tr(::blankString);
    const auto infos = QSerialPortInfo::availablePorts();

    int total = 0;
    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        const QString description = info.description();
        const QString manufacturer = info.manufacturer();
        const QString serialNumber = info.serialNumber();
        const auto vendorId = info.vendorIdentifier();
        const auto productId = info.productIdentifier();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (vendorId ? QString::number(vendorId, 16) : blankString)
             << (productId ? QString::number(productId, 16) : blankString);

        serialPortInfoListBox->addItem(list.constFirst(), list);
        if (info.systemLocation().contains(tr("COM3"))
            || info.systemLocation().contains(tr("COM4"))) {
            serialPortInfoListBox->setCurrentIndex(total);
        }
        total++;
    }

    serialPortInfoListBox->addItem(tr("Custom"));
}

void DialogSerialSettings::updateSettings()
{
    settings_->name = serialPortInfoListBox->currentText();

    if (baudRateBox->currentIndex() == 4) {
        settings_->baudRate = baudRateBox->currentText().toInt();
    } else {
        const auto baudRateData = baudRateBox->currentData();
        settings_->baudRate = baudRateData.value<QSerialPort::BaudRate>();
    }
    settings_->stringBaudRate = QString::number(settings_->baudRate);

    const auto dataBitsData = dataBitsBox->currentData();
    settings_->dataBits = dataBitsData.value<QSerialPort::DataBits>();
    settings_->stringDataBits = dataBitsBox->currentText();

    const auto parityData = parityBox->currentData();
    settings_->parity = parityData.value<QSerialPort::Parity>();
    settings_->stringParity = parityBox->currentText();

    const auto stopBitsData = stopBitsBox->currentData();
    settings_->stopBits = stopBitsData.value<QSerialPort::StopBits>();
    settings_->stringStopBits = stopBitsBox->currentText();

    const auto flowControlData = flowControlBox->currentData();
    settings_->flowControl = flowControlData.value<QSerialPort::FlowControl>();
    settings_->stringFlowControl = flowControlBox->currentText();

    settings_->localEchoEnabled = localEchoCheckBox->isChecked();
}
