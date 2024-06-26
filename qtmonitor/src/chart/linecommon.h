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

#include <attribute.h>
#include <QString>

#pragma once

class LineCommon {
public:
    LineCommon(AttributeType &descr);

    const AttributeType &getDescription();
    unsigned size();
    void append(double y);
    void append(double x, double y);
    const char *getName() { return descr_["Name"].to_string(); }
    const char *getColor() { return color_; }

    void setPlotSize(int w, int h);
    void selectData(int start_idx, int total);
    bool getNext(int &x, int &y);
    bool getMaxMarker(int &x1, int &y1, int &x2, int &y2);
    bool getMinMarker(int &x1, int &y1, int &x2, int &y2);
    bool getXY(int idx, int &x, int &y);
    bool getAxisValue(int axis, int idx, double &outval);
    bool getAxisValue(int axis, int idx, QString &outstr);
    void getAxisMin(int axis, QString &outstr);
    void getAxisMax(int axis, QString &outstr);
    int getNearestByX(int x);
    bool isMarkerMaxEnabled() { return normalMaxEna_; }
    bool isMarkerMinEnabled() { return normalMinEna_; }

private:
    AttributeType descr_;
    bool normalMaxEna_;
    double normalMaxVal_;
    bool normalMinEna_;
    double normalMinVal_;
    struct AxisType {
        double *data;
        double accum;
        bool fixedMinVal;
        double minVal;
        bool fixedMaxVal;
        double maxVal;
    } axis_[2];
    bool is_ring_;
    int start_;
    int cnt_;
    int len_;
    char color_[8];
    char format_[16];
    double plot_w;
    double plot_h;
    double dx;
    double dy;
    int sel_start_idx;
    int sel_cnt;
};
