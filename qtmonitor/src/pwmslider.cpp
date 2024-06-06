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

#include "pwmslider.h"

PwmSlider::PwmSlider(QWidget *parent)
    : QWidget(parent) {
    setMinimumHeight(60);

    pixmapBkg_ = QPixmap(size());
    pixmapBkg_.fill(Qt::black);


}

bool PwmSlider::event(QEvent *e) {
    if (e->type() == QEvent::Leave){
        update();
    }
    return QWidget::event(e);
}

void PwmSlider::mousePressEvent(QMouseEvent *event) {
    setFocus();
    /** There're 2 methods: buttons() and button():
        buttons() is a multiple flag of button()
    */
    Qt::MouseButton pressed = event->button();
    if (pressed != Qt::LeftButton) {
        pressed = Qt::NoButton;
        return;
    }

    pointCursor_ = event->pos();
    
    //emit signalSendData(text.toLocal8Bit());
    update();
}

void PwmSlider::paintEvent(QPaintEvent *event) {
    QPixmap pixmapPaint(pixmapBkg_.size());
    
    QPainter p1(&pixmapPaint);

    p1.drawPixmap(QPoint(0, 0), pixmapBkg_);

    /*int press;
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
    }*/
    p1.end();

    QPainter p(this);
    p.drawPixmap(QPoint(0,0), pixmapPaint);
    p.end();
}
