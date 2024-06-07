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

PwmSlider::PwmSlider(QWidget *parent, AttributeType *cfg)
    : QWidget(parent) {
    setMinimumHeight(100);
    setMinimumWidth(1000);
    objname_ = QString((*cfg)["ObjName"].to_string());
    attrname_ = QString((*cfg)["AttrName"].to_string());
    colorStart_ = QColor((*cfg)["ColorStart"].to_string());
    colorEnd_ = QColor((*cfg)["ColorEnd"].to_string());
}

bool PwmSlider::event(QEvent *e) {
    if (e->type() == QEvent::Leave){
        update();
    } else if (e->type() == QEvent::Show){
        emit signalRequestReadAttribute(objname_, attrname_);
        pointCursor_.setY(height() / 2);
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

    double minStepNearLimit = height() / 2 / (static_cast<double>(width()) / 100.0);
    double rate = event->pos().x() / (static_cast<double>(width()) / 100.0);
    if (rate <= minStepNearLimit) {
        rate = 0;
    } else if (rate >= (100.0 - minStepNearLimit)) {
        rate = 100.0;
    }

    emit signalRequestWriteAttribute(objname_, attrname_, static_cast<quint32>(rate));
    setValue(static_cast<quint32>(rate));
}

void PwmSlider::setValue(quint32 val) {
    if (val > 100) {
        val = 100;
    }
    double rate = (static_cast<double>(width()) / 100.0);
    rate *= val;
    if (rate > width()) {
        rate = width();
    }
    pointCursor_.setX(static_cast<int>(rate));
    if (isVisible()) {
        update();
    }
}

void PwmSlider::paintEvent(QPaintEvent *event) {
    QPixmap pixmapPaint(size());
    
    QPainter p1(&pixmapPaint);
    QLinearGradient g(0, 0, width(), 0);
    g.setColorAt(0, colorStart_);
    g.setColorAt(1, colorEnd_);

    p1.setRenderHint(QPainter::Antialiasing);
    p1.setCompositionMode(QPainter::CompositionMode_Source);
    p1.setBrush(g);
    p1.setPen(Qt::NoPen);
    p1.drawRect(2, 2, width() - 4, height() - 4);

    p1.setPen(QPen(Qt::black, 5));
    p1.setBrush(Qt::NoBrush);
    p1.drawEllipse(pointCursor_, height() / 2 - 5, height() / 2 - 5);
    p1.end();

    QPainter p(this);
    p.drawPixmap(QPoint(0,0), pixmapPaint);
    p.end();
}

void PwmSlider::slotResponseAttribute(const QString &objname,
                                      const QString &atrname,
                                      quint32 data) {
    if (atrname == attrname_ && objname == objname_) {
        setValue(data);
    }
}