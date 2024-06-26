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

#include "linecommon.h"
#include <QString>

LineCommon::LineCommon(AttributeType &descr) {
    if (descr.is_dict()) {
        descr_ = descr;
    } else {
        descr_.make_dict();
        descr_["Name"].make_string("NoName");
        descr_["Format"].make_string("%.1f");
        descr_["RingLength"].make_int64(256);
        descr_["FixedMinY"].make_boolean(true);
        descr_["FixedMinYVal"].make_floating(0.0);
        descr_["FixedMaxY"].make_boolean(false);
        descr_["FixedMaxYVal"].make_floating(0.0);
        descr_["Color"].make_string("#FF0000");
    }
    strcpy(color_, descr_["Color"].to_string());
    strcpy(format_, descr_["Format"].to_string());

    plot_w = 0;
    plot_h = 0;
    dx = 0;
    dy = 0;
    sel_start_idx = 0;
    sel_cnt = 0;

    is_ring_ = false;
    len_ = 1024;
    cnt_ = 0;
    start_ = 0;
    if (descr_["RingLength"].to_uint32()) {
        is_ring_ = true;
        len_ = descr_["RingLength"].to_int();
    }

    for (int i = 0; i < 2; i++) {
        axis_[i].data = new double[len_];
        axis_[i].accum = 0;
        axis_[i].fixedMinVal = descr_["FixedMinY"].to_bool();
        axis_[i].minVal = descr_["FixedMinYVal"].to_float();
        axis_[i].fixedMaxVal = descr_["FixedMaxY"].to_bool();
        axis_[i].maxVal = descr_["FixedMaxYVal"].to_float();
    }
    normalMaxEna_ = descr_["NormalMaxY"].to_bool();
    normalMaxVal_ = descr_["NormalMaxYVal"].to_float();
    normalMinEna_ = descr_["NormalMinY"].to_bool();
    normalMinVal_ = descr_["NormalMinYVal"].to_float();
}

const AttributeType &LineCommon::getDescription() {
    return descr_;
}
    
unsigned LineCommon::size() {
    return cnt_;
}
void LineCommon::append(double y) {
    append(size(), y);
}

void LineCommon::append(double x, double y) {
    if (cnt_ == 0) {
        axis_[0].minVal = x;
        axis_[0].maxVal = x;
        if (!axis_[1].fixedMinVal) {
            axis_[1].minVal = y;
        }
        if (!axis_[1].fixedMaxVal) {
            axis_[1].maxVal = y;
        }
    }


    int wridx;
    if (is_ring_) {
        if (cnt_ < len_) {
            wridx = cnt_;
        } else  {
            if (++start_ >= len_) {
                start_ = 0;
            }
            wridx = start_ - 1;
            if (wridx < 0) {
                wridx += len_;
            }
        }
    } else {
        if (cnt_ >= len_) {
            len_ *= 2;
            for (int i = 0; i < 2; i++) {
                double *tbuf = new double[len_];
                memcpy(tbuf, axis_[i].data, cnt_*sizeof(double));
                delete []axis_[i].data;
                axis_[i].data = tbuf;
            }
        }
        wridx = cnt_;
    }
    axis_[0].data[wridx] = x;
    axis_[1].data[wridx] = y;
    if (cnt_ < len_) {
        cnt_++;
    }

    // X-axis
    axis_[0].accum += x;
    if (x < axis_[0].minVal) {
        axis_[0].minVal = x;
    }
    if (x > axis_[0].maxVal) {
        axis_[0].maxVal = x;
    }

    // Y-axis
    axis_[1].accum += y;
    if (!axis_[1].fixedMinVal && y < axis_[1].minVal) {
        axis_[1].minVal = y;
    }
    if (!axis_[1].fixedMaxVal && y > axis_[1].maxVal) {
        axis_[1].maxVal = y;
    }
}

void LineCommon::setPlotSize(int w, int h) {
    plot_w = w;
    plot_h = h;
}


void LineCommon::selectData(int start_idx, int total) {
    sel_start_idx = start_idx;
    sel_cnt = start_idx;
    if (plot_w == 0 || plot_h == 0) {
        dx = 0;
        dy = 0;
        return;
    }
    if (total > 1) {
        dx = plot_w / static_cast<double>(total - 1);
    } else {
        dx = 0;
    }
    double diff_y = axis_[1].maxVal - axis_[1].minVal;
    if (diff_y) {
        dy = plot_h / diff_y;
    } else {
        dy = 0;
    }
}

bool LineCommon::getNext(int &x, int &y) {
    double val;
    if (!getAxisValue(1, sel_cnt, val)) {
        return false;
    }
    x = static_cast<int>((sel_cnt - sel_start_idx) * dx + 0.5);
    if (axis_[1].fixedMaxVal && val > axis_[1].maxVal) {
        y = 0;
    } else if (axis_[1].fixedMinVal && val < axis_[1].minVal) {
        y = static_cast<int>(axis_[1].maxVal * dy + 0.5);
    } else {
        y = static_cast<int>((axis_[1].maxVal - val) * dy + 0.5);
    }
    sel_cnt++;
    return true;
}

bool LineCommon::getMaxMarker(int &x1, int &y1, int &x2, int &y2) {
    if (normalMaxEna_) {
        x1 = 0;
        y1 = static_cast<int>((axis_[1].maxVal - normalMaxVal_) * dy + 0.5);
        x2 = plot_w;
        y2 = y1;
    }
    return normalMaxEna_;
}

bool LineCommon::getMinMarker(int &x1, int &y1, int &x2, int &y2) {
    if (normalMinEna_) {
        x1 = 0;
        y1 = static_cast<int>((axis_[1].maxVal - normalMinVal_) * dy + 0.5);
        x2 = plot_w;
        y2 = y1;
    }
    return normalMinEna_;
}


bool LineCommon::getXY(int idx, int &x, int &y) {
    double val;
    if (!getAxisValue(1, idx, val)) {
        return false;
    }
    x = static_cast<int>((idx - sel_start_idx) * dx + 0.5);
    y = static_cast<int>((axis_[1].maxVal - val) * dy + 0.5);
    return true;
}

int LineCommon::getNearestByX(int x) {
    if (x < 0 || dx == 0) {
        return sel_start_idx;
    }
    double idx = static_cast<double>(x) / dx;
    return sel_start_idx + static_cast<int>(idx + 0.5);
}

bool LineCommon::getAxisValue(int axis, int idx, double &outval) {
    if (idx >= 0 && idx < cnt_) {
        int n = (start_ + idx) % len_;
        outval = axis_[axis].data[n];
        return true;
    }
    return false;
}

bool LineCommon::getAxisValue(int axis, int idx, QString &outstr) {
    if (idx >= 0 && idx < cnt_) {
        int n = (start_ + idx) % len_;
        double outval = axis_[axis].data[n];
        outstr = QString::asprintf(format_, outval);
        return true;
    }
    return false;
}

void LineCommon::getAxisMin(int axis, QString &outstr) {
    outstr = QString::asprintf(format_, axis_[axis].minVal);
}

void LineCommon::getAxisMax(int axis, QString &outstr) {
    outstr = QString::asprintf(format_, axis_[axis].maxVal);
}
