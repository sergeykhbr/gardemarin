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

#include <QWidget>

class ControlPanel : public QWidget {
    Q_OBJECT

 public:
    explicit ControlPanel(QWidget *parent = nullptr);

    enum EBtnNames {
        KEY_1,
        KEY_2,
        KEY_Total
    };


 protected slots:
    void slotBtn1Toggled(bool val);
    void slotBtn2Toggled(bool val);

 protected:
    bool event(QEvent *e) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void paintEvent(QPaintEvent *event_) override;

 signals:
    void signalBtn1Toggled(bool val);
    void signalBtn2Toggled(bool val);

 private:
    QPixmap pixmapBkg_;
    QPixmap pixmapBtn_[KEY_Total];
    QPoint pointCursor_;
    QRect rectBtn_[KEY_Total];

    int btnSelected_;
    int btnPressed_;
};


