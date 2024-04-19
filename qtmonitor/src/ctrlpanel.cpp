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

static const char *strcmd[2*ControlPanel::KEY_Total] = {
    "BTN_1 release",
    "BTN_1 press",
    "BTN_2 release",
    "BTN_2 press",
};

ControlPanel::ControlPanel(QWidget *parent)
    : QWidget(parent) {
    setFixedWidth(320);
    setFixedHeight(120);


    connect(this, SIGNAL(signalBtn1Toggled(bool)),
                  SLOT(slotBtn1Toggled(bool)));
    connect(this, SIGNAL(signalBtn2Toggled(bool)),
                  SLOT(slotBtn2Toggled(bool)));


    pixmapBkg_ = QPixmap(size());
    pixmapBkg_.fill(Qt::white);
    pixmapBtn_[KEY_1].convertFromImage(
        QImage(":/images/1_37x32x4.png"));
    pixmapBtn_[KEY_2].convertFromImage(
        QImage(":/images/2_37x32x4.png"));

    rectBtn_[KEY_1] = QRect(40, 15, BTN_DIGIT_WIDTH, BTN_DIGIT_HEIGHT);
    rectBtn_[KEY_2] = QRect(80, 15, BTN_DIGIT_WIDTH, BTN_DIGIT_HEIGHT);

    setMouseTracking(true);
    btnSelected_ = 0;
    btnPressed_ = 0;
}

void ControlPanel::slotBtn1Toggled(bool val) {
//    igui_->registerCommand(static_cast<IGuiCmdHandler *>(this),
//                            strcmd[2*KEY_1 + val], &response_, true);
}

void ControlPanel::slotBtn2Toggled(bool val) {
//    igui_->registerCommand(static_cast<IGuiCmdHandler *>(this),
//                            strcmd[2*KEY_2 + val], &response_, true);
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
    toggle_signal_type s[KEY_Total] = {
        &ControlPanel::signalBtn1Toggled,
        &ControlPanel::signalBtn2Toggled,
    };

    for (int n = 0; n < KEY_Total; n++) {
        if (rectBtn_[n].contains(pointCursor_)) {
            press ^= 1 << n;
            bool toggle = press & (1 << n) ? true : false;
            emit (this->*(s[n]))(toggle);
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
