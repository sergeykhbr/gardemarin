use <MCAD/boxes.scad>
$fa=1.0;    // minimum angle
$fs=0.1;    // minimum size

// Case formfactor
CASE_WIDTH = 80;        // outter size
CASE_LENGTH = 159;
Thickness = 2.4;        // limited by board size and standard DIN case (2.4 max)

//       ___45__
//    __|       |___  top_ext_h     // Top extension
//   |              | top_h         // Top + top inclination
//   |--------------|
//   |______w_______| bottom_h      // Bottom
//
bottom_height = 15;
top_h_inclin = 1;                   // top side has small inclination
top_height = 25 - top_h_inclin;     // without inclintation
top_corner = 4;                     // top part corners diameter
top_ext_h = 16;                     // top extension
top_ext_w = 45;                     // top extension standard is 45 mm
top_ext_l = 95;                     // top extension length must be <= CASE_LENGTH
top_ext_l_off = 50;                 // top extension length length offset

support_thick = 0.8;
M4_diameter = 3.8;                  // inner hole for M4 screw
eps = 0.01;                         // remove z-fighting artifacts

module circle_support(d=10, cut=6, nocut=0.2) {
    L = 3.1415*d;
    N = L/(cut + nocut);
    difference() {
        circle(d=(d + 2*eps));
        union() {
            for (i = [0: 360/N : 360]) {
                translate([d/2*cos(i), d/2*sin(i)]) circle(d=cut);
            }
        }
    }
}

module square_support(w=10, h=10, cut=6, nocut=0.2) {
    dlt = cut + nocut;
    Nw = w/dlt;
    Nh = h/dlt;
    difference() {
        square([w+2*eps,h+2*eps], center=true);
        union() {
            for (i = [0: Nw]) {
                translate([-w/2 + i*dlt, -h/2]) circle(d=cut);
                translate([-w/2 + i*dlt, h/2]) circle(d=cut);
            }
            for (i = [0: Nh]) {
                translate([-w/2, -h/2 + i*dlt]) circle(d=cut);
                translate([w/2 , -h/2 + i*dlt]) circle(d=cut);
            }
        }
    }
}

module polygon_support(points, cut=1, nocut=0.4, off=0) {
    Lx = (points[1].x - points[0].x);
    Ly = (points[1].y - points[0].y);
    alpha = atan(Ly/Lx);
    Length = sqrt(Lx*Lx + Ly*Ly) - 0*cut;
    N = round(Length / (cut + nocut));
    dx = Lx / N;
    dy = Ly / N;
    x0 = points[0].x;
    y0 = points[0].y;
    cut_adjust = (Length / N) - nocut;
    d_max = 1.0;
    translate([0,0,-support_thick/2]) linear_extrude(support_thick) difference() {
        polygon(points);
        if (cut_adjust <= d_max) {
            union() {
                for (i = [0: N]) {
                    translate([-0*cos(alpha)*(0.5*cut_adjust) + i*dx + x0,
                              -0*sin(alpha)*(0.5*cut_adjust) + i*dy + y0, 0]) {
                          circle(d=cut_adjust);
                    }
                }
            }
        } else {
            union() {
                for (i = [0: N]) {
                    translate([-cos(alpha)*(0.5*cut_adjust + off) + i*dx + x0,
                               -sin(alpha)*(0.5*cut_adjust + off) + i*dy + y0, 0]) hull() {
                        circle(d=d_max);
                        translate([cos(alpha)*(cut_adjust-d_max), sin(alpha)*(cut_adjust-d_max), 0]) circle(d=d_max);
                    }
                }
            }
        }
    }
}

