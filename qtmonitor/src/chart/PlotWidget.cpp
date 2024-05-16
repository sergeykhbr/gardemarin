/*
 *  Copyright 2021 Sergey Khabarov, sergeykhbr@gmail.com
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
 *
 * @brief      Generic Plot drawer widget.
 */

#include <math.h>
#include "PlotWidget.h"

static const int MARGIN = 5;
static const char *PLOTS_COLORS[LINES_PER_PLOT_MAX] = {
    "#007ACC",      // nice blue color (msvc)
    "#40C977",      // green/blue color
    "#CA5100",      // nice orange color (msvc)
    "#BD63C5",      // violet
    "#FFFFFF",      // white color
    "#FFFFFF",      // white color
    "#FFFFFF",      // white color
    "#FFFFFF"       // white color
};

PlotWidget::PlotWidget(QWidget *parent)
    : QWidget(parent) {
    selectedEpoch = 0;
    epochStart = 0;
    epochTotal = 0;
    lineTotal_ = 0;
    trackLineIdx = 0;
    pressed = Qt::NoButton;

    setFocusPolicy(Qt::ClickFocus);

    contextMenu = new QMenu(this);
    contextMenu->addAction(
        tr("Clear zoom"), this, SLOT(slotActionZoomClear()));

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
                  SLOT(slotRightClickMenu(const QPoint &)));   


    bkg1 = QColor(Qt::black);
    groupName = QString("NoGroupName");
    groupUnits = QString("clocks");
    defaultLineCfg.from_config("{"
        "'Name':'NoName',"
        "'Format':'%.1f',"
        "'RingLength':256,"
        "'Color':'#FFFFFF',"
        "'FixedMinY':true,"
        "'FixedMinYVal':0.0,"
        "'FixedMaxY':false,"
        "'FixedMaxYVal':0.0}");

    AttributeType cfgLine1 = defaultLineCfg;
    cfgLine1["Name"].make_string("gram1");
    cfgLine1["FixedMaxY"].make_boolean(true);
    cfgLine1["FixedMaxYVal"].make_floating(4000.0);
    cfgLine1["Color"].make_string("#FFFFFF");

    AttributeType cfgLine2 = defaultLineCfg;
    cfgLine2["Name"].make_string("gram2");
    cfgLine2["FixedMaxY"].make_boolean(true);
    cfgLine2["FixedMaxYVal"].make_floating(4000.0);
    cfgLine2["Color"].make_string("#40C977");

    AttributeType cfgLine3 = defaultLineCfg;
    cfgLine3["Name"].make_string("gram3");
    cfgLine3["FixedMaxY"].make_boolean(true);
    cfgLine3["FixedMaxYVal"].make_floating(4000.0);
    cfgLine3["Color"].make_string("#007ACC");


    line_[lineTotal_++] = new LineCommon(cfgLine1);
    line_[lineTotal_++] = new LineCommon(cfgLine2);
    line_[lineTotal_++] = new LineCommon(cfgLine3);
}


PlotWidget::~PlotWidget() {
    for (int i = 0; i < lineTotal_; i++) {
        delete line_[i];
    }
}

void PlotWidget::writeData(int lineidx, float val) {
    if (lineidx >= lineTotal_) {
        return;
    }
    line_[lineidx]->append(val);
    epochTotal = line_[lineidx]->size();
    update();
}

/*void BusUtilPlot::slotCmdResponse() {
    if (!response_.is_list()) {
        return;
    }
    int mst_total = static_cast<int>(response_.size());
    if (lineTotal < mst_total) {
        char tstr[64];
        for (int i = lineTotal; i < mst_total; i++) {
            RISCV_sprintf(tstr, sizeof(tstr), "mst[%d]", i);
            defaultLineCfg["Name"].make_string(tstr);
            defaultLineCfg["Color"].make_string(PLOTS_COLORS[i]);
            line_[i] = new LineCommon(defaultLineCfg);
            line_[i]->setPlotSize(rectPlot.width(), rectPlot.height());
        }
        lineTotal = mst_total;
    }
    double wr, rd;
    for (int i = 0; i < mst_total; i++) {
        const AttributeType &mst = response_[i];
        if (!mst.is_list() || mst.size() != 2) {
            continue;
        }
        wr = mst[0u].to_float();
        rd = mst[1].to_float();
        line_[i]->append(wr + rd);
    }
    epochTotal = line_[0]->size();
    update();
}*/

void PlotWidget::renderAll() {
    pixmap.fill(bkg1);
    QPainter p(&pixmap);

    renderAxis(p);

    p.translate(rectPlot.topLeft());
    for (int lineIdx = 0; lineIdx < lineTotal_; lineIdx++) {
        renderLine(p, line_[lineIdx]);
    }

    renderMarker(p);
    renderInfoPanel(p);

    p.translate(-rectPlot.topLeft());
    renderSelection(p);

    p.end();
}

void PlotWidget::renderAxis(QPainter &p) {
    p.setRenderHint(QPainter::Antialiasing, true);

    p.setPen(QPen(QColor(0x48,0x3D,0x8B)));  // Color Dark State Blue: #483D8B
    p.drawLine(rectPlot.bottomLeft(), rectPlot.bottomRight());
    p.drawLine(rectPlot.bottomLeft(), rectPlot.topLeft());

    /** Draw Group name: */
    QSize groupTextSize = p.fontMetrics().size(Qt::TextDontClip, groupName);
    QPoint groupTextPos = QPoint((width() - groupTextSize.width())/2, 0);
    QRect groupTextRect = QRect(groupTextPos, groupTextSize);
    p.setPen(QPen(QColor("#BFBFBF")));
    p.drawText(groupTextRect, Qt::AlignLeft, groupName);

    /** Draw Y-axis units: */
    p.rotate(-90.0);
    groupTextSize = p.fontMetrics().size(Qt::TextDontClip, groupUnits);
    groupTextPos = QPoint(-(height() + groupTextSize.width())/2, 2);
    groupTextRect = QRect(groupTextPos, groupTextSize);
    p.drawText(groupTextRect, Qt::AlignLeft, groupUnits);
    p.rotate(90.0);
}

void PlotWidget::renderLine(QPainter &p, LineCommon *pline) {
    bool draw_line = false;
    int x, y;
    QPoint ptA, ptB;
    QRect box(0, 0, 4, 4);
    p.setPen(QColor(pline->getColor()));
    pline->selectData(epochStart, epochTotal);
    for (int i = epochStart; i < epochStart + epochTotal; i++) {
        if (!pline->getNext(x, y)) {
            draw_line = false;
            continue;
        }
        box.moveTo(x - 2, y - 2);
        p.drawRect(box);

        if (!draw_line) {
            draw_line = true;
            ptA = QPoint(x, y);
        } else {
            ptB = QPoint(x, y);
            p.drawLine(ptA, ptB);
            ptA = ptB;
        }
    }
}

void PlotWidget::renderMarker(QPainter &p) {
    if (!lineTotal_) {
        return;
    }
    LineCommon *pline = line_[trackLineIdx];
    int x, y;
    p.setPen(Qt::red);
    QRect box(0, 0, 6, 6);
    if (pline->getXY(selectedEpoch, x, y)) {
        /** 
            Vertial Marker line:
        */
        p.setPen(QPen(QColor(0xff, 0, 0, 0xa0)));
        QPoint ptA(x, 0);
        QPoint ptB(x, rectPlot.height());
        p.drawLine(ptA, ptB);

        // Marker box
        box.moveTo(x - 3, y - 3);
        p.drawRect(box);
    }
}

void PlotWidget::renderSelection(QPainter &p) {
    /**
        Highlight selection when mouse pressed
    */
    if (pressed == Qt::MiddleButton) {
        p.setCompositionMode(QPainter::CompositionMode_Difference);
        QRect rect(QPoint(pressStart.x(), rectPlot.top()),
                   QPoint(pressEnd.x(), rectPlot.bottom()));
        p.fillRect(rect, Qt::white);
    }
}

void PlotWidget::renderInfoPanel(QPainter &p) {
    char bufName[256];
    QFont fontPos;
    // Save previous values:
    QBrush oldBrush = p.brush();
    QPen oldPen = p.pen();
    QFont oldFont = p.font();

    fontPos.setPixelSize(10);
    p.setFont(fontPos);

    QSize infoNameSize;
    QString name;
    QString fullString = QString::asprintf("idx: %d\n", selectedEpoch);

    // Find the longest 'name : value' string among lines:
    int NAME_WIDTH_MAX = rectPlot.width() / 2;
    for (int i = 0; i < lineTotal_; i++) {
        LineCommon *pLine = line_[i];
        if (!pLine->getAxisValue(1, selectedEpoch, bufName, sizeof(bufName))) {
            continue;
        }
        name = QString::asprintf("%s: %s\n", pLine->getName(), bufName);
        fullString += name;
    }
    infoNameSize = p.fontMetrics().size(Qt::TextDontClip, fullString);
    /** Guard: to avoid wrong formatted string value */
    if (infoNameSize.width() > NAME_WIDTH_MAX) {
        infoNameSize.setWidth(NAME_WIDTH_MAX);
    }

    QPoint posPanel(rectPlot.width() - infoNameSize.width() - 5, 5);

    p.setPen(QPen(QColor("#450020")));
    p.setBrush(QBrush(QColor(0xff, 0xef, 0xd5, 0x80)));

    QRect rectPanel(posPanel, infoNameSize);
    QRect textPanel = rectPanel;
    rectPanel.setWidth(rectPanel.width() + 4);
    rectPanel.setLeft(rectPanel.left() - 2);
    p.drawRoundedRect(rectPanel, 2, 2);
    p.drawText(textPanel, Qt::AlignLeft, fullString);

    // Restore:
    p.setBrush(oldBrush);
    p.setPen(oldPen);
    p.setFont(oldFont);
}

int PlotWidget::pix2epoch(QPoint pix) {
    pix -= rectPlot.topLeft();
    if (pix.x() > rectPlot.width()) {
        pix.setX(rectPlot.width());
    }
    return line_[trackLineIdx]->getNearestByX(pix.x());
}

void PlotWidget::resizeEvent(QResizeEvent *ev) {
    int w = ev->size().width();
    int h = ev->size().height();
    if (w == 0 || h == 0) {
        return;
    }
    QSize pixmapSingleSize = QSize(w, h);
    rectMargined.setTopLeft(QPoint(MARGIN, MARGIN));
    rectMargined.setBottomRight(QPoint(w - MARGIN, h - MARGIN));

    QFontMetrics fm(font());
    int font_h = fm.height() + 4;
    rectPlot = rectMargined;
    rectPlot.setLeft(rectMargined.left() + font_h);
    rectPlot.setTop(rectMargined.top() + font_h);
    for (int i = 0; i < lineTotal_; i++) {
        line_[i]->setPlotSize(rectPlot.width(), rectPlot.height());
    }

    pixmap = QPixmap(pixmapSingleSize);
}

void PlotWidget::paintEvent(QPaintEvent *event) {
    renderAll();

    QPainter p(this);
    QPoint pos(0,0);
    p.drawPixmap(pos, pixmap);
    p.end();
}

void PlotWidget::mousePressEvent(QMouseEvent *event) {
    setFocus();
    /** There're 2 methods: buttons() and button():
        buttons() is a multiple flag of button()
    */
    pressed = event->button();
    if (pressed != Qt::LeftButton && pressed != Qt::MiddleButton) {
        pressed = Qt::NoButton;
        return;
    }

    selectedEpoch = pix2epoch(event->pos());
    pressStart = event->pos();
}

void PlotWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!event->buttons()) {
        return;
    }

    if (pressed != Qt::NoButton) {
        selectedEpoch = pix2epoch(event->pos());
        pressEnd = event->pos();
        update();
    }
}

void PlotWidget::mouseReleaseEvent(QMouseEvent *) {
    if (pressed == Qt::MiddleButton) {
        /** Warning: changing of epochStart before epochEnd computed 
         *  will raise an errors 
         */
        int tmpStart = pix2epoch(pressStart);
        int tmpEnd = pix2epoch(pressEnd);
        if (tmpStart < tmpEnd) {
            epochStart = tmpStart;
            epochTotal = tmpEnd - epochStart + 1;
        } else if (tmpStart > tmpEnd) {
            epochStart = tmpEnd;
            epochTotal = tmpStart - epochStart + 1;
        }
    }
    pressed = Qt::NoButton;
    update();
}

void PlotWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Left:
        if (selectedEpoch > epochStart) {
            selectedEpoch--;
            update();
        }
        break;
    case Qt::Key_Right:
        if (selectedEpoch < (epochStart + epochTotal) - 1) {
            selectedEpoch++;
            update();
        }
        break;
    default:;
    }
}


void PlotWidget::slotRightClickMenu(const QPoint &p) {
    QPoint globalPos = mapToGlobal(p);
    contextMenu->popup(globalPos);
}

void PlotWidget::slotActionZoomClear() {
    epochStart = 0;
    epochTotal = line_[trackLineIdx]->size();
    update();
}


double PlotWidget::borderUpValue(double v) {
    double tmp;
    if (v == 0) {
        return 0;
    }

    //double border = 1.0;
    double x10 = 1.0;
    if (v > 1.0) {
        tmp = v;
        while (tmp > 1.0) {
            tmp /= 10.0;
            x10 *= 10.0;
        }
        tmp *= 10.0;
        tmp = (double)((int)tmp + 1);
        x10 *= tmp;

    } else if (v < 1.0) {
    }
    return 0.0;
}
