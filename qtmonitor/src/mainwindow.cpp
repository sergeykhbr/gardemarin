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

#include "mainwindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QToolBar>
#include <chrono>

MainWindow::MainWindow(AttributeType *cfg) :
    QMainWindow(nullptr),
    serial_(new SerialWidget(this, cfg)),
    tabWindow_(new TabWindow(this, serial_)),
    labelStatus_(new QLabel)
{
    Config_.clone(cfg);

    setWindowIcon(QIcon(":/images/connect.png"));
    setWindowTitle(tr("qtmonitor"));

    resize(QSize(400, 300));

    setCentralWidget(tabWindow_);


    dialogSerialSettings_ = new ComPortSettings(this);

    /*actionAbout_ = new QAction(QIcon(tr(":/images/logo.png")),
                              tr("&About"), this);
    actionAbout_->setToolTip(tr("About program"));
    actionAbout_->setShortcut(QKeySequence(tr("Alt+A")));
    connect(actionAbout_, &QAction::triggered, this, &MainWindow::about);


    actionConnect_ = new QAction(QIcon(tr(":/images/connect.png")),
                              tr("C&onnect"), this);
    actionConnect_->setToolTip(tr("Connect to serial port"));
    actionConnect_->setShortcut(QKeySequence(tr("Ctrl+O")));
    connect(actionConnect_, &QAction::triggered, this, &MainWindow::openSerialPort);


    actionDisconnect_ = new QAction(QIcon(tr(":/images/disconnect.png")),
                              tr("&Disconnect"), this);
    actionDisconnect_->setToolTip(tr("Disconnect from serial port"));
    actionDisconnect_->setShortcut(QKeySequence(tr("Ctrl+D")));
    connect(actionDisconnect_, &QAction::triggered, this, &MainWindow::closeSerialPort);


    actionConfigure_ = new QAction(QIcon(tr(":/images/settings.png")),
                              tr("&Configure"), this);
    actionConfigure_->setToolTip(tr("Configure serial port"));
    actionConfigure_->setShortcut(QKeySequence(tr("Alt+C")));
    connect(actionConfigure_, &QAction::triggered, dialogSerialSettings_, &ComPortSettings::show);


    actionClear_ = new QAction(QIcon(tr(":/images/clear.png")),
                              tr("&Clear"), this);
    actionClear_->setToolTip(tr("Clear data"));
    actionClear_->setShortcut(QKeySequence(tr("Alt+L")));
    connect(actionClear_, &QAction::triggered, tabWindow_, &TabWindow::slotClearSerialConsole);


    actionQuit_ = new QAction(QIcon(tr(":/images/appexit.png")),
                              tr("&Quit"), this);
    actionQuit_->setShortcut(QKeySequence(tr("Ctrl+Q")));
    connect(actionQuit_, &QAction::triggered, this, &MainWindow::close);


    QRect menuGeometry(0, 0, 400, 19);
    menuBar()->setGeometry(menuGeometry);

    QMenu *menu = menuBar()->addMenu(tr("&Calls"));
    menu->addAction(actionConnect_);
    menu->addAction(actionDisconnect_);
    menu->addSeparator();
    menu->addAction(actionQuit_);

    menu = menuBar()->addMenu(tr("&Tools"));
    menu->addAction(actionConfigure_);
    menu->addAction(actionClear_);

    menu = menuBar()->addMenu(tr("&Info"));
    menu->addAction(actionAbout_);


    QToolBar *mainToolBar = new QToolBar(this);
    mainToolBar->addAction(actionConnect_);
    mainToolBar->addAction(actionDisconnect_);
    mainToolBar->addAction(actionConfigure_);
    mainToolBar->addAction(actionClear_);
    addToolBar(mainToolBar);

    actionConnect_->setEnabled(true);
    actionDisconnect_->setEnabled(false);
    actionQuit_->setEnabled(true);
    actionConfigure_->setEnabled(true);
    */

    QStatusBar *statusBar_ = new QStatusBar(this);
    setStatusBar(statusBar_);
    statusBar_->addWidget(labelStatus_);


    connect(this, &MainWindow::signalSerialPortOpened,
            tabWindow_, &TabWindow::slotSerialPortOpened);
    connect(this, &MainWindow::signalSerialPortClosed,
            tabWindow_, &TabWindow::slotSerialPortClosed);
    connect(serial_, &SerialWidget::signalFailed,
            this, &MainWindow::slotSerialError);


    openSerialPort();
}

MainWindow::~MainWindow() {
    delete dialogSerialSettings_;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    // TODO: save state
    closeSerialPort();
    event->accept();
}

void MainWindow::openSerialPort() {
    dialogSerialSettings_->exec();
    const ComPortSettings::Settings p = dialogSerialSettings_->settings();

    serial_->setPortName(p.name);
    serial_->setBaudRate(p.baudRate);
    serial_->setDataBits(p.dataBits);
    serial_->setParity(p.parity);
    serial_->setStopBits(p.stopBits);
    serial_->setFlowControl(p.flowControl);
    if (serial_->open(QIODevice::ReadWrite)) {
        emit signalSerialPortOpened(p.localEchoEnabled);
        showStatusMessage(tr("Connected to %1 : %2")
                          .arg(p.name, p.stringBaudRate));
    } else {
        QMessageBox::critical(this, tr("Error"), serial_->errorString());

        showStatusMessage(tr("Open error"));
    }
}



void MainWindow::closeSerialPort() {
    if (serial_->isOpen()) {
        serial_->close();
    }
    emit signalSerialPortClosed();
}

void MainWindow::showStatusMessage(const QString &message) {
    labelStatus_->setText(message);
}

void MainWindow::slotSerialError(const QString &message) {
    QMessageBox::warning(this, tr("Warning"), message);
}

