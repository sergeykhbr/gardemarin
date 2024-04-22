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

#include "ctrlpanel.h"
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>

static const int BTN_DIGIT_WIDTH = 37;
static const int BTN_DIGIT_HEIGHT = 32;

typedef void (ControlPanel::*toggle_signal_type)(bool);

toggle_signal_type signalArray_[ControlPanel::KEY_Total] = {
    &ControlPanel::signalBtn1Toggled,
    &ControlPanel::signalBtn2Toggled,
    &ControlPanel::signalBtnRedToggled,
    &ControlPanel::signalBtnBlueToggled,
    &ControlPanel::signalBtnWhiteToggled,
    &ControlPanel::signalBtnMixedToggled,
    &ControlPanel::signalBtnDc0Toggled,
    &ControlPanel::signalBtnDc1Toggled,
    &ControlPanel::signalBtnDc2Toggled,
    &ControlPanel::signalBtnDc3Toggled,
    &ControlPanel::signalBtnDc4Toggled,
    &ControlPanel::signalBtnDc5Toggled,
    &ControlPanel::signalBtnDc6Toggled,
    &ControlPanel::signalBtnDc7Toggled,
};

/* 
 * objid: 2 = relais0, atrid: 0=state
 * objid: 3 = relais1, atrid: 0=state
 * objid: 4 = ledrbw, atrid: 2=red_duty; 4=blue_duty; 6=white_duty; 8=mixed_duty
 * objid: 10 = hbrdg0, atrid: 3=dc0_duty; 6=dc1_duty
 * objid: 11 = hbrdg1, atrid: 3=dc0_duty; 6=dc1_duty
 * objid: 12 = hbrdg2, atrid: 3=dc0_duty; 6=dc1_duty
 * objid: 13 = hbrdg3, atrid: 3=dc0_duty; 6=dc1_duty
 */
static const char *commandArray_[2*ControlPanel::KEY_Total] = {
    ">!00000102,2,8000\r\n", ">!00000102,2,8001\r\n",       // relais0  enable/disable
    ">!00000103,2,8000\r\n", ">!00000103,2,8001\r\n",       // relais1  enable/disable
    ">!00000104,2,8200\r\n", ">!00000104,2,8264\r\n",       // red_duty = 100/0
    ">!00000104,2,8400\r\n", ">!00000104,2,8464\r\n",       // blue_duty = 100/0
    ">!00000104,2,8600\r\n", ">!00000104,2,8664\r\n",       // white_duty = 100/0
    ">!00000104,2,8800\r\n", ">!00000104,2,8864\r\n",       // mixed_duty = 100/0
    ">!0000010A,2,8300\r\n", ">!0000010A,2,8364\r\n",       // hbrg0::dc0_duty = 100/0
    ">!0000010A,2,8600\r\n", ">!0000010A,2,8664\r\n",       // hbrg0::dc1_duty = 100/0
    ">!0000010B,2,8300\r\n", ">!0000010B,2,8364\r\n",       // hbrg1::dc0_duty = 100/0
    ">!0000010B,2,8600\r\n", ">!0000010B,2,8664\r\n",       // hbrg1::dc1_duty = 100/0
    ">!0000010C,2,8300\r\n", ">!0000010C,2,8364\r\n",       // hbrg2::dc0_duty = 100/0
    ">!0000010C,2,8600\r\n", ">!0000010C,2,8664\r\n",       // hbrg2::dc1_duty = 100/0
    ">!0000010D,2,8300\r\n", ">!0000010D,2,8364\r\n",       // hbrg3::dc0_duty = 100/0
    ">!0000010D,2,8600\r\n", ">!0000010D,2,8664\r\n",       // hbrg3::dc1_duty = 100/0
};

ControlPanel::ControlPanel(QWidget *parent)
    : QWidget(parent) {
    setFixedWidth(380);
    setFixedHeight(120);


    connect(this, SIGNAL(signalBtn1Toggled(bool)),
                  SLOT(slotBtn1Toggled(bool)));
    connect(this, SIGNAL(signalBtn2Toggled(bool)),
                  SLOT(slotBtn2Toggled(bool)));
    connect(this, SIGNAL(signalBtnRedToggled(bool)),
                  SLOT(slotBtnRedToggled(bool)));
    connect(this, SIGNAL(signalBtnBlueToggled(bool)),
                  SLOT(slotBtnBlueToggled(bool)));
    connect(this, SIGNAL(signalBtnWhiteToggled(bool)),
                  SLOT(slotBtnWhiteToggled(bool)));
    connect(this, SIGNAL(signalBtnMixedToggled(bool)),
                  SLOT(slotBtnMixedToggled(bool)));
    connect(this, SIGNAL(signalBtnDc0Toggled(bool)),
                  SLOT(slotBtnDc0Toggled(bool)));
    connect(this, SIGNAL(signalBtnDc1Toggled(bool)),
                  SLOT(slotBtnDc1Toggled(bool)));
    connect(this, SIGNAL(signalBtnDc2Toggled(bool)),
                  SLOT(slotBtnDc2Toggled(bool)));
    connect(this, SIGNAL(signalBtnDc3Toggled(bool)),
                  SLOT(slotBtnDc3Toggled(bool)));
    connect(this, SIGNAL(signalBtnDc4Toggled(bool)),
                  SLOT(slotBtnDc4Toggled(bool)));
    connect(this, SIGNAL(signalBtnDc5Toggled(bool)),
                  SLOT(slotBtnDc5Toggled(bool)));
    connect(this, SIGNAL(signalBtnDc6Toggled(bool)),
                  SLOT(slotBtnDc6Toggled(bool)));
    connect(this, SIGNAL(signalBtnDc7Toggled(bool)),
                  SLOT(slotBtnDc7Toggled(bool)));


    pixmapBkg_ = QPixmap(size());
    pixmapBkg_.fill(Qt::white);
    pixmapBtn_[KEY_1].convertFromImage(
        QImage(":/images/1_37x32x4.png"));
    pixmapBtn_[KEY_2].convertFromImage(
        QImage(":/images/2_37x32x4.png"));
    pixmapBtn_[KEY_red].convertFromImage(
        QImage(":/images/led_red_128x128x4.png"));
    pixmapBtn_[KEY_blue].convertFromImage(
        QImage(":/images/led_blue_128x128x4.png"));
    pixmapBtn_[KEY_white].convertFromImage(
        QImage(":/images/led_white_128x128x4.png"));
    pixmapBtn_[KEY_mixed].convertFromImage(
        QImage(":/images/led_mixed_128x128x4.png"));
    pixmapBtn_[KEY_dc0].convertFromImage(
        QImage(":/images/motor_128x128x4.png"));
    pixmapBtn_[KEY_dc1].convertFromImage(
        QImage(":/images/motor_128x128x4.png"));
    pixmapBtn_[KEY_dc2].convertFromImage(
        QImage(":/images/motor_128x128x4.png"));
    pixmapBtn_[KEY_dc3].convertFromImage(
        QImage(":/images/motor_128x128x4.png"));
    pixmapBtn_[KEY_dc4].convertFromImage(
        QImage(":/images/motor_128x128x4.png"));
    pixmapBtn_[KEY_dc5].convertFromImage(
        QImage(":/images/motor_128x128x4.png"));
    pixmapBtn_[KEY_dc6].convertFromImage(
        QImage(":/images/motor_128x128x4.png"));
    pixmapBtn_[KEY_dc7].convertFromImage(
        QImage(":/images/motor_128x128x4.png"));

    rectBtn_[KEY_1] = QRect(40, 15, BTN_DIGIT_WIDTH, BTN_DIGIT_HEIGHT);
    rectBtn_[KEY_2] = QRect(80, 15, BTN_DIGIT_WIDTH, BTN_DIGIT_HEIGHT);
    rectBtn_[KEY_red] = QRect(120, 15, BTN_DIGIT_WIDTH, BTN_DIGIT_HEIGHT);
    rectBtn_[KEY_blue] = QRect(160, 15, BTN_DIGIT_WIDTH, BTN_DIGIT_HEIGHT);
    rectBtn_[KEY_white] = QRect(120, 55, BTN_DIGIT_WIDTH, BTN_DIGIT_HEIGHT);
    rectBtn_[KEY_mixed] = QRect(160, 55, BTN_DIGIT_WIDTH, BTN_DIGIT_HEIGHT);

    rectBtn_[KEY_dc0] = QRect(200, 15, BTN_DIGIT_WIDTH, BTN_DIGIT_HEIGHT);
    rectBtn_[KEY_dc1] = QRect(200, 55, BTN_DIGIT_WIDTH, BTN_DIGIT_HEIGHT);
    rectBtn_[KEY_dc2] = QRect(240, 15, BTN_DIGIT_WIDTH, BTN_DIGIT_HEIGHT);
    rectBtn_[KEY_dc3] = QRect(240, 55, BTN_DIGIT_WIDTH, BTN_DIGIT_HEIGHT);
    rectBtn_[KEY_dc4] = QRect(280, 15, BTN_DIGIT_WIDTH, BTN_DIGIT_HEIGHT);
    rectBtn_[KEY_dc5] = QRect(280, 55, BTN_DIGIT_WIDTH, BTN_DIGIT_HEIGHT);
    rectBtn_[KEY_dc6] = QRect(320, 15, BTN_DIGIT_WIDTH, BTN_DIGIT_HEIGHT);
    rectBtn_[KEY_dc7] = QRect(320, 55, BTN_DIGIT_WIDTH, BTN_DIGIT_HEIGHT);

    setMouseTracking(true);
    btnSelected_ = 0;
    btnPressed_ = 0;
}

void ControlPanel::btnToggledGeneric(int idx, bool val) {
    QString text = QString(tr(commandArray_[2*idx + static_cast<int>(val)]));
    emit signalGetData(text.toLocal8Bit());
}

void ControlPanel::slotBtn1Toggled(bool val) {
    btnToggledGeneric(KEY_1, val);
}

void ControlPanel::slotBtn2Toggled(bool val) {
    btnToggledGeneric(KEY_2, val);
}

void ControlPanel::slotBtnRedToggled(bool val) {
    btnToggledGeneric(KEY_red, val);
}

void ControlPanel::slotBtnBlueToggled(bool val) {
    btnToggledGeneric(KEY_blue, val);
}

void ControlPanel::slotBtnWhiteToggled(bool val) {
    btnToggledGeneric(KEY_white, val);
}

void ControlPanel::slotBtnMixedToggled(bool val) {
    btnToggledGeneric(KEY_mixed, val);
}

void ControlPanel::slotBtnDc0Toggled(bool val) {
    btnToggledGeneric(KEY_dc0, val);
}

void ControlPanel::slotBtnDc1Toggled(bool val) {
    btnToggledGeneric(KEY_dc1, val);
}

void ControlPanel::slotBtnDc2Toggled(bool val) {
    btnToggledGeneric(KEY_dc2, val);
}

void ControlPanel::slotBtnDc3Toggled(bool val) {
    btnToggledGeneric(KEY_dc3, val);
}

void ControlPanel::slotBtnDc4Toggled(bool val) {
    btnToggledGeneric(KEY_dc4, val);
}

void ControlPanel::slotBtnDc5Toggled(bool val) {
    btnToggledGeneric(KEY_dc5, val);
}

void ControlPanel::slotBtnDc6Toggled(bool val) {
    btnToggledGeneric(KEY_dc6, val);
}

void ControlPanel::slotBtnDc7Toggled(bool val) {
    btnToggledGeneric(KEY_dc7, val);
}

bool ControlPanel::event(QEvent *e) {
    if (e->type() == QEvent::Leave){
        btnSelected_ = 0;
        update();
    }
    return QWidget::event(e);
}

void ControlPanel::mouseMoveEvent(QMouseEvent *event) {
    pointCursor_ = event->pos();

    int sel = 0;
    for (int i = 0; i < KEY_Total; i++) {
        if (rectBtn_[i].contains(pointCursor_)) {
            sel |= 1 << i;
        }
    }
    if (sel != btnSelected_) {
        btnSelected_ = sel;
        update();
    }
}

void ControlPanel::mousePressEvent(QMouseEvent *event) {
    setFocus();
    /** There're 2 methods: buttons() and button():
        buttons() is a multiple flag of button()
    */
    Qt::MouseButton pressed = event->button();
    if (pressed != Qt::LeftButton) {
        pressed = Qt::NoButton;
        return;
    }

    int press = btnPressed_;
    

    for (int n = 0; n < KEY_Total; n++) {
        if (rectBtn_[n].contains(pointCursor_)) {
            press ^= 1 << n;
            bool toggle = press & (1 << n) ? true : false;
            emit (this->*(signalArray_[n]))(toggle);
        }
    }
    if (press != btnPressed_) {
        btnPressed_ = press;
        update();
    }
}

void ControlPanel::paintEvent(QPaintEvent *event) {
    QPixmap pixmapPaint(pixmapBkg_.size());
    
    QPainter p1(&pixmapPaint);

    p1.drawPixmap(QPoint(0, 0), pixmapBkg_);

    int press;
    int sel;
    for (int i = 0; i < KEY_Total; i++) {
        press = (btnPressed_ >> i) & 0x1;
        sel = (btnSelected_ >> i) & 0x1;
        int pheight = pixmapBtn_[i].height();
        int pwidth = pixmapBtn_[i].width()/4;
        if (press && sel) {
            p1.drawPixmap(rectBtn_[i], pixmapBtn_[i].copy(
                        3*pwidth, 0, pwidth, pheight));
        } else if (press && !sel) {
            p1.drawPixmap(rectBtn_[i], pixmapBtn_[i].copy(
                        2*pwidth, 0, pwidth, pheight));
        } else if (!press && sel) {
            p1.drawPixmap(rectBtn_[i], pixmapBtn_[i].copy(
                        1*pwidth, 0, pwidth, pheight));
        } else {
            p1.drawPixmap(rectBtn_[i], pixmapBtn_[i].copy(
                        0, 0, pwidth, pheight));
        }
    }
    p1.end();

    QPainter p(this);
    p.drawPixmap(QPoint(0,0), pixmapPaint);
    p.end();
}
