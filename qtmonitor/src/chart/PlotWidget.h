/**
 * @file
 * @copyright  Copyright 2017 GNSS Sensor Ltd. All right reserved.
 * @author     Sergey Khabarov - sergeykhbr@gmail.com
 * @brief      Generic Plot drawer widget.
 */

#pragma once

#include "attribute.h"
#include "linecommon.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QMenu>
#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtGui/qevent.h>
#include <QtWidgets/QMdiArea>
#include <QtWidgets/QMdiSubWindow>
#include <QtWidgets/QVBoxLayout>

static const int LINES_PER_PLOT_MAX = 8;

class PlotWidget : public QWidget {
    Q_OBJECT

 public:
    explicit PlotWidget(QWidget *parent = 0);
    virtual ~PlotWidget();

    void writeData(int lineidx, int64_t val);

 public slots:
    void slotRightClickMenu(const QPoint &p);
    void slotActionZoomClear();

 protected:
    void resizeEvent(QResizeEvent *);
    void paintEvent(QPaintEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void keyPressEvent(QKeyEvent *event);

 private:
    void renderAll();
    void renderAxis(QPainter &p);
    void renderLine(QPainter &p, LineCommon *pline);
    void renderMarker(QPainter &p);
    void renderSelection(QPainter &p);
    void renderInfoPanel(QPainter &p);
    int pix2epoch(QPoint pix);

    double borderUpValue(double v);

 protected:
    AttributeType defaultLineCfg;

    QColor bkg1;

    Qt::MouseButton pressed;
    QPoint pressStart;          /** Mouse Middle button start coordinates */
    QPoint pressEnd;            /** Mouse Middle button end coordinates */

    int epochStart;             /** Draw data starting from this index  */
    int epochTotal;             /** Draw the following number of epochs */
    int selectedEpoch;

    double dmax;
    double dmin;
    QPixmap pixmap;

    int lineTotal_;
    int trackLineIdx;
    LineCommon *line_[LINES_PER_PLOT_MAX];
    QString groupName;
    QString groupUnits;

    QMenu *contextMenu;
    QRect rectMargined;
    QRect rectPlot;
};
