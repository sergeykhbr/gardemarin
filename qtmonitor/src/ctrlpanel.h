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
        KEY_red,
        KEY_blue,
        KEY_white,
        KEY_mixed,
        KEY_dc0,
        KEY_dc1,
        KEY_dc2,
        KEY_dc3,
        KEY_dc4,
        KEY_dc5,
        KEY_dc6,
        KEY_dc7,
        KEY_Total
    };


 protected slots:
    void slotBtn1Toggled(bool val);
    void slotBtn2Toggled(bool val);
    void slotBtnRedToggled(bool val);
    void slotBtnBlueToggled(bool val);
    void slotBtnWhiteToggled(bool val);
    void slotBtnMixedToggled(bool val);
    void slotBtnDc0Toggled(bool val);
    void slotBtnDc1Toggled(bool val);
    void slotBtnDc2Toggled(bool val);
    void slotBtnDc3Toggled(bool val);
    void slotBtnDc4Toggled(bool val);
    void slotBtnDc5Toggled(bool val);
    void slotBtnDc6Toggled(bool val);
    void slotBtnDc7Toggled(bool val);

 protected:
    void btnToggledGeneric(int idx, bool val);
    bool event(QEvent *e) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void paintEvent(QPaintEvent *event_) override;

 signals:
    void signalBtn1Toggled(bool val);
    void signalBtn2Toggled(bool val);
    void signalBtnRedToggled(bool val);
    void signalBtnBlueToggled(bool val);
    void signalBtnWhiteToggled(bool val);
    void signalBtnMixedToggled(bool val);
    void signalBtnDc0Toggled(bool val);
    void signalBtnDc1Toggled(bool val);
    void signalBtnDc2Toggled(bool val);
    void signalBtnDc3Toggled(bool val);
    void signalBtnDc4Toggled(bool val);
    void signalBtnDc5Toggled(bool val);
    void signalBtnDc6Toggled(bool val);
    void signalBtnDc7Toggled(bool val);
    void signalGetData(const QByteArray &data);

 private:
    QPixmap pixmapBkg_;
    QPixmap pixmapBtn_[KEY_Total];
    QPoint pointCursor_;
    QRect rectBtn_[KEY_Total];

    int btnSelected_;
    int btnPressed_;
};


