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

static constexpr std::chrono::seconds kWriteTimeout = std::chrono::seconds{5};


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_status(new QLabel),
    m_settings(new ComPortSettings(this)),
    m_timer(new QTimer(this)),
    m_serial(new QSerialPort(this)),
    tabWindow_(new TabWindow(this))
{
    setWindowIcon(QIcon(":/images/logo.png"));
    setWindowTitle(tr("qtmonitor"));

    resize(QSize(400, 300));

    setCentralWidget(tabWindow_);


    actionAbout_ = new QAction(QIcon(tr(":/images/logo.png")),
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
    connect(actionConfigure_, &QAction::triggered, m_settings, &ComPortSettings::show);


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

#if 0
  <widget class="QStatusBar" name="statusBar"/>
#endif
//    m_ui->statusBar->addWidget(m_status);


    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::handleWriteTimeout);
    m_timer->setSingleShot(true);

    connect(this, &MainWindow::signalSerialPortOpened,
            tabWindow_, &TabWindow::slotSerialPortOpened);
    connect(this, &MainWindow::signalSerialPortClosed,
            tabWindow_, &TabWindow::slotSerialPortClosed);
    connect(this, &MainWindow::signalRecvSerialPort,
            tabWindow_, &TabWindow::slotRecvData);
    connect(tabWindow_, &TabWindow::signalSendData,
            this, &MainWindow::slotSendSerialPort);

    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::slotRecvSerialPort);
    connect(m_serial, &QSerialPort::bytesWritten, this, &MainWindow::handleBytesWritten);
}

MainWindow::~MainWindow() {
    delete m_settings;
}

void MainWindow::openSerialPort() {
    const ComPortSettings::Settings p = m_settings->settings();

    m_serial->setPortName(p.name);
    m_serial->setBaudRate(p.baudRate);
    m_serial->setDataBits(p.dataBits);
    m_serial->setParity(p.parity);
    m_serial->setStopBits(p.stopBits);
    m_serial->setFlowControl(p.flowControl);
    if (m_serial->open(QIODevice::ReadWrite)) {
        emit signalSerialPortOpened(p.localEchoEnabled);
        actionConnect_->setEnabled(false);
        actionDisconnect_->setEnabled(true);
        actionConfigure_->setEnabled(false);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(p.name, p.stringBaudRate, p.stringDataBits,
                               p.stringParity, p.stringStopBits, p.stringFlowControl));
    } else {
        QMessageBox::critical(this, tr("Error"), m_serial->errorString());

        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort() {
    if (m_serial->isOpen()) {
        m_serial->close();
    }
    emit signalSerialPortClosed();
    actionConnect_->setEnabled(true);
    actionDisconnect_->setEnabled(false);
    actionConfigure_->setEnabled(true);
    showStatusMessage(tr("Disconnected"));
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About Serial Terminal"),
                       tr("The <b>Serial Terminal</b> example demonstrates how to "
                          "use the Qt Serial Port module in modern GUI applications "
                          "using Qt, with a menu bar, toolbars, and a status bar."));
}

void MainWindow::handleError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::handleBytesWritten(qint64 bytes) {
    m_bytesToWrite -= bytes;
    if (m_bytesToWrite == 0) {
        m_timer->stop();
    }
}

void MainWindow::handleWriteTimeout() {
    const QString error = tr("Write operation timed out for port %1.\n"
                             "Error: %2").arg(m_serial->portName(),
                                              m_serial->errorString());
    showWriteError(error);
}


void MainWindow::showStatusMessage(const QString &message) {
    m_status->setText(message);
}

void MainWindow::showWriteError(const QString &message) {
    QMessageBox::warning(this, tr("Warning"), message);
}

void MainWindow::slotSendSerialPort(const QByteArray &data) {
    const qint64 written = m_serial->write(data);
    if (written == data.size()) {
        m_bytesToWrite += written;
        m_timer->start(kWriteTimeout);
    } else {
        const QString error = tr("Failed to write all data to port %1.\n"
                                 "Error: %2").arg(m_serial->portName(),
                                                  m_serial->errorString());
        showWriteError(error);
    }
}

void MainWindow::slotRecvSerialPort() {
    const QByteArray data = m_serial->readAll();
    emit signalRecvSerialPort(data);
}

