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
#include <QSlider>
#include <QPushButton>

class TabLights : public QWidget {
    Q_OBJECT

 public:
    explicit TabLights(QWidget *parent = nullptr);

 signals:
    void signalRequestReadAttribute(const QString &objname, const QString &atrname);
    void signalRequestWriteAttribute(const QString &objname, const QString &atrname, quint32 data);

 public slots:
    void slotResponseAttribute(const QString &objname, const QString &atrname, quint32 data);

 private slots:
    void slotChangeDim0(int idx);
    void slotChangeDim1(int idx);
    void slotChangeDim2(int idx);
    void slotChangeDim3(int idx);
    void slotLightsMoveUp(bool checked);
    void slotLightsMoveDown(bool checked);

 private:
    QSlider *slider_[4];
    QPushButton *btnUp_;
    QPushButton *btnDown_;
};


