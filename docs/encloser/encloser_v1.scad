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

module din_rail(len=100) {
    // 1 mm thickness
    din_thick = 1;
    points = [
        [0,0],
        [-din_thick,0],
        [-din_thick, 0.5*(35-27)],
        [-7.5, 0.5*(35-27)],
        [-7.5, 0.5*(35-27) + 27],
        [-din_thick, 0.5*(35-27) + 27],
        [-din_thick, 35],
        [0, 35],
        [0, 0.5*(35-27) + 27 - din_thick],
        [-7.5 - din_thick , 0.5*(35-27) + 27 - din_thick],
        [-7.5 - din_thick , 0.5*(35-27) + din_thick],
        [0, 0.5*(35-27) + din_thick]
    ];
    linear_extrude(len) color([1,0,0]) polygon(points);
}

module din_slot(width = 10) {
    gap = 0.1;
    High = 4;
    Low = 2.0;    // 4 mm is a maximum height of din = 0.5*(37-25)
    Slide = 2.3;  // Slide + Low = 4 mm
    points = [
        [5, -gap - 5],
        [5, 35 + 7],
        [-2.5, 35 + 7],
        [-4, 35 + 3.5],
        [-4, (35 + Slide + gap) - High + 1.1],
        [-1 - (0.4) - gap, (35 + Slide + gap) - High],
        [-1 - gap, 35 + Slide + gap],
        [gap, 35 + Slide + gap],
        // small pressure buldge
        [gap - 0.4, 35],
        [gap - 1*gap - 0.4, 33],
        [gap - 1*gap - 0.4, 31],
        [gap, 27],
        
        [gap, -gap + 2],
        [0 - 0.05, -gap],
        [-1 + 0.05, -gap],
        [-1 - gap, -gap + 2],
        [-1 - gap, Low - 0.2],  // 0.2 to make insertion more smooth
        [-4, Low],
        [-4, -2.0],
        [-1.5, -5],
    ];
    translate([-5, 5 - 0.5*(35+7+5), 0]) linear_extrude(width) color([0,1,0]) polygon(points);
}

module enclose_bottom(x=100, y=20, z=20) {
    thick = Thickness;
    points_side = [
        [0, 0.03*y],
        [0.7*x, 0],
        [x, 0],
        [x, y],
        [0.25*x, y],
        [0, y - 0.1*y]
    ];
    points_side_brd = [
        points_side[0],
        points_side[1],
        [points_side[2].x - 1.5, points_side[2].y],
        [points_side[3].x - 1.5, points_side[3].y],
        points_side[4],
        points_side[5]
    ];
    
    points_back = [
        points_side[0],  // 0
        points_side[1],  // 1
        points_side[2],  // 2
        [points_side[2].x, points_side[2].y + thick/2],  // 3
        [points_side[2].x - 1.5, points_side[2].y + thick/2], // 4
        [points_side[2].x - 1.5, points_side[2].y + thick],  // 5
        [points_side[1].x + thick, points_side[1].y + thick],  // 6
        [thick, points_side[0].y + thick],  // 7
        
        [thick, points_side[5].y - thick],  // 8
        [points_side[4].x + thick, points_side[4].y - thick],  // 9
        [points_side[3].x - 1.5, points_side[4].y - thick],  // 10
        [points_side[3].x - 1.5, points_side[4].y - thick/2],  // 11
        [points_side[3].x, points_side[4].y - thick/2],  // 11
        
        points_side[3],
        points_side[4],
        points_side[5]
    ];
    linear_extrude(0.5 * thick) polygon(points_side);
    linear_extrude(thick) polygon(points_side_brd);
    linear_extrude(z) polygon(points_back);
    translate([0,0,z - thick]) linear_extrude(thick) polygon(points_side_brd);
    translate([0,0,z - 0.5 * thick]) linear_extrude(0.5 * thick) polygon(points_side);
    
    translate([eps, y/2, 4]) din_slot(width=6);
    translate([eps, y/2, z - 6 - 4]) din_slot(width=6);
}

module enclose_top_ext() {
    translate([top_ext_l_off, 0.5*(CASE_WIDTH - top_ext_w), -top_height + eps]) rotate([0,90,0]) {
        linear_extrude(top_ext_l) {
               hull() {
                    translate([top_ext_h - top_corner/2 + eps, top_ext_w - top_corner/2, 0]) circle(d=top_corner);
                    translate([top_ext_h - top_corner/2 + eps, top_corner/2, 0]) circle(d=top_corner);
                    square([1, 45]);
                }
        }
    }
 }

 module enclose_top_ext_diff() {
    translate([top_ext_l_off + Thickness, 0.5*(CASE_WIDTH - top_ext_w), -top_height]) rotate([0,90,0]) 
    {
        linear_extrude(top_ext_l - 2*Thickness) {
               hull() {
                    translate([top_ext_h - Thickness - top_corner/2, top_ext_w - Thickness - top_corner/2, 0]) circle(d=top_corner);
                    translate([top_ext_h - Thickness - top_corner/2, Thickness + top_corner/2, 0]) circle(d=top_corner);
                    translate([-1.1*Thickness, Thickness, 0]) square([1, 45 - 2*Thickness]);
                }
        }
    }
}

module enclose_motor_diff() {
    translate([41, Thickness + 1, -8.5 + eps]) rotate([90,0,0]) linear_extrude(Thickness + 2) {
        square([98.6, 8.5]);
    }
}
module enclose_motor_frame() {
    translate([39, Thickness, -10.5 + eps]) rotate([90,0,0]) linear_extrude(Thickness + 0.2) {
        square([102.6, 10.5]);
    }
}

module enclose_usb_diff() {
    translate([-1, 4.4, eps]) rotate([0,90,0]) linear_extrude(Thickness + 2) {
        square([5, 9.4]);
    }
}
module enclose_usb_frame() {
    translate([-0.2, 2.4, eps]) rotate([0,90,0]) linear_extrude(Thickness + 0.2) {
        square([7, 13.4]);
    }
}

module enclose_can1_diff() {
    translate([-1, 21.0, eps]) rotate([0,90,0]) linear_extrude(Thickness + 2) {
        square([14, 17.1]);
    }
}
module enclose_can1_frame() {
    translate([-0.2, 19.0, eps]) rotate([0,90,0]) linear_extrude(Thickness + 0.2) {
        square([16, 21.1]);
    }
}

module enclose_can2_diff() {
    translate([-1, 40.0, eps]) rotate([0,90,0]) linear_extrude(Thickness + 2) {
        square([14, 17.1]);
    }
}
module enclose_can2_frame() {
    translate([-0.2, 39.0, eps]) rotate([0,90,0]) linear_extrude(Thickness + 0.2) {
        square([16, 19.1]);
    }
}

module enclose_eth_diff() {
    translate([-1, 58.5, eps]) rotate([0,90,0]) linear_extrude(Thickness + 2) {
        square([14, 17.1]);
    }
}
module enclose_eth_frame() {
    translate([-0.2, 57.5, eps]) rotate([0,90,0]) linear_extrude(Thickness + 0.2) {
        square([16, 20.1]);
    }
}

module enclose_pwr_diff() {
    translate([CASE_LENGTH-Thickness-1, 56.5, eps]) rotate([0,90,0]) linear_extrude(Thickness + 2) {
        square([9, 11.2]);
    }
}
module enclose_pwr_frame() {
    translate([CASE_LENGTH-Thickness, 54.5, eps]) rotate([0,90,0]) linear_extrude(Thickness + 0.2) {
        square([11, 15.2]);
    }
}

// Uncover 2 of 3 connectors (1 unused)
module enclose_relais1_diff() {
    translate([CASE_LENGTH-Thickness-1, 38.2, eps]) rotate([0,90,0]) linear_extrude(Thickness + 2) {
        square([9, 11.2]);
    }
}
module enclose_relais1_frame() {
    translate([CASE_LENGTH-Thickness, 36.2, eps]) rotate([0,90,0]) linear_extrude(Thickness + 0.2) {
        square([11, 15.2]);
    }
}

module enclose_relais2_diff() {
    translate([CASE_LENGTH-Thickness-1, 22.1, eps]) rotate([0,90,0]) linear_extrude(Thickness + 2) {
        square([9, 11.2]);
    }
}
module enclose_relais2_frame() {
    translate([CASE_LENGTH-Thickness, 19.9, eps]) rotate([0,90,0]) linear_extrude(Thickness + 0.2) {
        square([11, 15.2]);
    }
}

// Temperature sensor hole with round corners
module enclose_T1T2_diff() {
    radius = 2.5;
    translate([CASE_LENGTH-Thickness-1, 12.1, 1.5 + eps]) rotate([0,90,0]) linear_extrude(Thickness + 2) {
        hull() {
            translate([12, radius]) circle(d=2*radius);
            square([12, 2*radius]);
        }
    }
}
module enclose_T1T2_frame() {
    radius = 4.5;
    translate([CASE_LENGTH-Thickness, 10.1, 1.5 + eps]) rotate([0,90,0]) linear_extrude(Thickness + 0.2) {
        hull() {
            translate([12, radius]) circle(d=2*radius);
            square([12, 2*radius]);
        }
    }
}

module display_latch(H, L=3, W=4) {
    platch = [
        [0,0],
        [2,0],
        [2,H],
        [2 + L, H],
        [2 + L, H + 2],
        [0,H + 2],
    ];
    rotate([90, 0, 0]) linear_extrude(W) polygon(platch);
}

module enclose_display_frame(w=24.2,h=24.2,l_off=20) {
    // Frame and latches aligned with *_diff using lcd_test module()
    // TODO: board size relative frame w,h
    lcd_board_w = 28;
    lcd_board_h = 39;
    lcd_board_t = 3.5;
    translate([2+l_off,-2+0.5*(CASE_WIDTH-w),-(top_height+top_ext_h)]) union() {
        union() {
            translate([0,0,Thickness-eps]) union() {
                translate([h+12.8, 0.5*(lcd_board_w - 8), 0]) rotate([0, 0, 180]) display_latch(H=lcd_board_t, L=2, W=8);
                translate([0.5*(lcd_board_h+10), lcd_board_w+2, 0]) rotate([0, 0, -90]) display_latch(H=lcd_board_t, L=2, W=20);
                translate([0.5*(lcd_board_h-30), -2, 0]) rotate([0, 0, 90]) display_latch(H=lcd_board_t, L=3, W=20);
            }
        }
        // Ucomment this for lcd_test()
        //translate([-5,-5,0]) linear_extrude(Thickness) square([lcd_board_h+10, lcd_board_w+10]); // for test purposes
    }
}

module enclose_display_diff(w=20, h=10, l_off=20) {
    frame_w = 3;
    translate([l_off, 0.5*(CASE_WIDTH - (h+2*frame_w)), -(top_height + top_h_inclin + top_ext_h - Thickness - 1) + eps]) // where 1 comes from?
    {
        hull() {
            linear_extrude(0.1) translate([frame_w, frame_w]) square([w,h]);
            translate([0,0,-(Thickness + 2*eps)]) linear_extrude(0.1) square([w + 2*frame_w, h + 2*frame_w]);
        }
    }
}

module enclose_jtag_frame(depth=20, w=40, l=8) {
    D=16;
    translate([32.5, CASE_WIDTH/2 + 3.4, -(top_height+top_h_inclin-depth)]) {
        translate([0,0,-depth]) linear_extrude(depth) 
        union() {
            square([l,w], center=true);
            hull() {
                translate([-l/2, +4, 0]) circle(d=D);
                translate([-l/2, -4, 0]) circle(d=D);
                translate([l/2, +4, 0]) circle(d=D);
                translate([l/2, -4, 0]) circle(d=D);
            }
        }
    }
}
module enclose_jtag_diff(depth=20, w=40, l=8) {
    th=1.2;
    D=16;
    translate([32.5, CASE_WIDTH/2 + 3.4, -(top_height+top_h_inclin-depth)]) {
        union() {
            translate([0,0,-depth-th-eps]) linear_extrude(depth+th+2*eps) {
                square([l-2*th, w-2*th], center=true);
            }
            translate([0,0,-depth-th-eps]) linear_extrude(depth+eps) {
                hull() {
                    translate([-(l-th)/2, +4, 0]) circle(d=D-th);
                    translate([-(l-th)/2, -4, 0]) circle(d=D-th);
                    translate([(l-th)/2, +4, 0]) circle(d=D-th);
                    translate([(l-th)/2, -4, 0]) circle(d=D-th);
                }
            }
        }
    }
}

module enclose_mosfet_frame(depth=16, w=0.5*(CASE_WIDTH-top_ext_w), l=14) {
    dlt_h = w / (0.5*(CASE_WIDTH-top_ext_w));
    translate([0, CASE_WIDTH-w, -top_height+depth+eps]) rotate([-90,0,0]) rotate([0,-90,0]) {
        linear_extrude(l) {
            hull() {
                square([w, 1]);
                square([1, depth+dlt_h]);
                translate([w-top_corner/2, depth-top_corner/2, 0]) circle(d=top_corner);
            }
        }
    }
}
module enclose_mosfet_diff(depth=16, w=0.5*(CASE_WIDTH-top_ext_w), l=14) {
    th = Thickness;
    translate([0, CASE_WIDTH-w, -top_height+depth+eps]) rotate([-90,0,0]) rotate([0,-90,0]) {
        translate([0,0,th]) linear_extrude(l-2*th) {
            hull() {
                translate([top_corner/2 + th, depth - th -top_corner/2, 0]) circle(d=top_corner);
                translate([th, -eps, 0]) square([w-th, 1]);
                translate([th+w, -eps, 0]) square([1, depth-th]);
            }
        }
    }
}

module btn_tunnel_diff(pos_l=25, pos_w=15, btn_sz=8, btn_depth=20) {
    btn_h = 5;
    stem_h = btn_depth - (btn_h + Thickness);
    stem_d = 3;
    translate([pos_l, pos_w, -(top_height)]) {
        union() {
            translate([0,0,-(top_h_inclin+eps)]) linear_extrude(btn_h+top_h_inclin+eps) square(btn_sz, center=true);
            translate([0,0,btn_h-eps]) linear_extrude(stem_h+2*eps) circle(d=stem_d);
            translate([0,0,btn_h+Thickness]) linear_extrude(stem_h+eps) circle(d=btn_sz);
        }
    }
}
module btn_tunnel_frame(pos_l=25, pos_w=15, btn_sz = 8, btn_depth = 20) {
    translate([pos_l, pos_w, -(top_height)]) {
        linear_extrude(btn_depth) square(btn_sz+2, center=true);
    }
}


module enclose_top_side(height=100, width=20, thick=1) {
    linear_extrude(thick) {
        union() {
            hull() {
                translate([height - top_corner/2, width - top_corner/2, 0]) circle(d=top_corner);
                translate([height - top_corner/2, top_corner/2, 0]) circle(d=top_corner);
                square([1, width]);
                translate([height, 0.5*(width - 45), 0]) square([top_h_inclin, 45]);
            }
        }
    }
}

module enclose_top(height=100, width=20, length=20) {
    // Right top side
    translate([0, 0,0]) rotate([0,90,0]) {
        enclose_top_side(height=height, width=width, thick=Thickness);
        translate([-1.5, Thickness/2, Thickness/2 - eps]) {
            enclose_top_side(height=(height + 1.5), width = (width - Thickness), thick=Thickness/2 + eps);
        }
    }
    // Left top side
    translate([length - Thickness, 0,0]) rotate([0,90,0]) {
        enclose_top_side(height=height, width=width, thick=Thickness);
        translate([-1.5, Thickness/2, 0]) 
        {
            enclose_top_side(height=(height + 1.5), width = (width - Thickness), thick=Thickness/2 + eps);
        }
    }

    translate([Thickness/2, 0, 0]) rotate([0,90,0]) linear_extrude(length - Thickness) difference() {
        union() {
            hull() {
                translate([height - top_corner/2, width - top_corner/2, 0]) circle(d=top_corner);
                translate([height - top_corner/2, top_corner/2, 0]) circle(d=top_corner);
                square([1, width]);
                translate([height, 0.5*(width - 45), 0]) square([top_h_inclin, 45]);
            }
            translate([-1.5, Thickness/2, 0]) square([1.501, width - Thickness]);
        }
        hull() {
            translate([height - Thickness - top_corner/2, width - Thickness - top_corner/2, 0]) circle(d=top_corner);
            translate([height - Thickness - top_corner/2, Thickness + top_corner/2, 0]) circle(d=top_corner);
            translate([-1.501, Thickness, 0]) square([3, width - 2*Thickness]);
            translate([height - Thickness, 0.5*(width - 45), 0]) square([top_h_inclin, 45]);
        }
    }
}

module stand_screw_m4_frame(z=3, d=8) {
    // thcikness of wall =0.8 (not equal to Thickness). +5 additional diff in encloser inclination
    translate([0,0,-z]) difference() {
        linear_extrude(z) circle(d=d + 1.6);
        translate([0,0, -1]) linear_extrude(z+2) circle(d=M4_diameter);
    }

}
module stand_screw_m4_diff(z=3, d=8) {
    l = z + 5 + eps;
    translate([0,0, -Thickness - l]) {
        linear_extrude(l) circle(d=d);
    }
}


module enclose_top_with_ext(h=100, w=20, l=20) {
    mosfet_depth = 17;
    mosfet_w = 22;
    mosfet_l = 23;
    mosfet_l_pos = 123;

    scale1_depth = 17;
    scale1_w = 13;
    scale1_l = 18;
    scale1_l_pos = 103;

    scale2_depth = 17;
    scale2_w = 13;
    scale2_l = 18;
    scale2_l_pos = 87.8;

    difference() 
    {
        union() {
            enclose_top_ext();
            enclose_top(height=h, width=w, length=l);
            enclose_motor_frame();
            enclose_usb_frame();
            enclose_can1_frame();
            enclose_can2_frame();
            enclose_eth_frame();
            enclose_pwr_frame();
            enclose_relais1_frame();
            enclose_relais2_frame();
            enclose_T1T2_frame();
            enclose_jtag_frame(depth=15, w=35, l=10);
            btn_tunnel_frame(pos_l=23.8, pos_w=13.7, btn_sz=6, btn_depth=18);
            btn_tunnel_frame(pos_l=23.8, pos_w=7.1, btn_sz=6, btn_depth=18);
            translate([mosfet_l_pos, 0, 0]) enclose_mosfet_frame(depth=mosfet_depth, w=mosfet_w, l=mosfet_l);
            translate([scale1_l_pos, 0, 0]) enclose_mosfet_frame(depth=scale1_depth, w=scale1_w, l=scale1_l);
            translate([scale2_l_pos, 0, 0]) enclose_mosfet_frame(depth=scale2_depth, w=scale2_w, l=scale2_l);
            translate([38.2,73,-(top_height - 15 - eps)]) stand_screw_m4_frame(z=15, d=10);
            translate([151.2,7,-(top_height - 15 - eps)]) stand_screw_m4_frame(z=15, d=10);
        }
        union() {
            enclose_top_ext_diff();
            enclose_motor_diff();
            enclose_usb_diff();
            enclose_can1_diff();
            enclose_can2_diff();
            enclose_eth_diff();
            enclose_pwr_diff();
            enclose_relais1_diff();
            enclose_relais2_diff();
            enclose_T1T2_diff();
            enclose_display_diff(w=(26.4-2), h=(26.4-2), l_off=90);
            enclose_jtag_diff(depth=15, w=35, l=10);
            btn_tunnel_diff(pos_l=23.8, pos_w=13.7, btn_sz=6, btn_depth=18);
            btn_tunnel_diff(pos_l=23.8, pos_w=7.1, btn_sz=6, btn_depth=18);
            translate([mosfet_l_pos, 0, 0]) enclose_mosfet_diff(depth=mosfet_depth, w=mosfet_w, l=mosfet_l);
            translate([scale1_l_pos, 0, 0]) enclose_mosfet_diff(depth=scale1_depth, w=scale1_w, l=scale1_l);
            translate([scale2_l_pos, 0, 0]) enclose_mosfet_diff(depth=scale2_depth, w=scale2_w, l=scale2_l);
            translate([38.2,73,-(top_height - 15 - eps)]) stand_screw_m4_diff(z=15, d=10);
            translate([151.2,7,-(top_height - 15 - eps)]) stand_screw_m4_diff(z=15, d=10);
        }
    }
    // Add after everything subtracted
    enclose_display_frame(w=(26.4-2), h=(26.4-2), l_off=90);
}

module stand(z=3, cut=0) {
    difference() 
    {
        difference() {
            linear_extrude(z) circle(d=8);
            translate([0,0, 2]) linear_extrude(z) circle(d=M4_diameter);
        }
        rotate([25,0,0]) translate([-50 + cut*45,-5,-9.5]) cube(10);
    }
}



module enclose_bottom_with_stands(x=15, y=80, z=159) {
    rotate([0,-90,0]) color([0,1,1]) enclose_bottom(x=x, y=y, z=z);
    translate([-38.2, 23.9, 0]) color([1,0,0]) stand(z=13.6);
    translate([-38.2, 73.0, 0]) color([1,0,0]) stand(z=13.6, cut=1);
    translate([-151.2, 73.0, 0]) color([1,0,0]) stand(z=13.6, cut=1);
    translate([-151.2, 7.0, 0]) color([1,0,0]) stand(z=13.6);
}

module enclose_top_with_stands(h, w, l) {
    //translate([0,0,bottom_height]) rotate([0,180,0])
    {
        color([0,0.8,1, 0.5]) enclose_top_with_ext(h=h, w=w, l=l);
    }
}

module support_cut_circle(x=60, r=6) {
    Diam = 2*r;
    Distance = x - support_thick - Diam;
    y_offset = 0.3 * r > support_thick ? support_thick: 0.3 * r;
    translate([0,0,-eps]) linear_extrude(support_thick + 0.002) {
        hull() {
            translate([support_thick/2 + r, y_offset, 0]) circle(d=Diam);
            translate([x - support_thick/2 - r, y_offset, 0]) circle(d=Diam);
        }
    }
}

module support(x=100, y=20, r=6, N=4) {
    //N = 4; // number of supports
    N = (N == 0) ? x / 0.4 : N;
    difference() {
        linear_extrude(support_thick) square([x,y]);
        union() {
            for (i = [0: 1: N-1]) {
                translate([i*(x/N), 0, 0]) support_cut_circle(x=x/N, r=r);
            }
        }
    }
}

module case_support() {
    difference()
    {
        union() {
            translate([-145,40,0.2]) rotate([-90,0,0]) color([0.5, 0, 0.5]) support(x=130,y=150,r=2,N=6);
            //translate([-145,40,0.2]) rotate([-90,0,0]) 
            translate([-0.2,71.5,-0.0]) rotate([0,45,0]) rotate([0,0,-90]) color([0.5, 0, 0.5]) support(x=69,y=15,r=0.25,N=30);
            // base of support
            translate([6,40,0]) rotate([0,135,0]) translate([15,0,0]) linear_extrude(support_thick + eps) {
                hull() {
                    circle(d=30);
                    translate([85,0,0]) circle(d=30);
                }
            }
        }
        // cut 45 deg
        translate([6 + support_thick,-10,0]) {
            union() {
                rotate([-90,45,0]) translate([0,0,-0.1]) linear_extrude(100) square([250,250]);
                rotate([-90,45,0]) translate([-200,115,0.1]) linear_extrude(100) square([250,250]);
            }
        }
    }
}


module lcd_test() {
    difference()
    {
        enclose_display_frame(w=(26.4-2), h=(26.4-2),l_off=90);
        enclose_display_diff(w=(26.4-2), h=(26.4-2), l_off=90);  // acutal LCD size 26.4 but visible through frame is 24.4
    }
    translate([105.6,30,-4-(top_height+top_ext_h)]) color([0.5,1,0, 0.3]) import("screen.stl",convexity=5);
}

//enclose_bottom_with_stands(x=bottom_height, y=CASE_WIDTH, z=CASE_LENGTH);
//translate([-193.2,-23,15]) color([0.5,1,0, 1.0]) import("main_board.stl",convexity=5);
translate([-CASE_LENGTH + (CASE_LENGTH - top_ext_l_off - top_ext_l), 0, (top_height+top_ext_h)])
    enclose_top_with_stands(h=top_height, w=CASE_WIDTH, l=CASE_LENGTH);


//lcd_test();
//din_rail(len=20);
//case_support();

min_support_h = 5;
support_overlay = 0.2;
module top_support_1() {
    D = 0.4;
    N = (top_ext_w - 0.2) / (D + 0.2);
    translate([0, 0.5*(CASE_WIDTH-top_ext_w), 0]) {
        rotate([0,45,0]) translate([-support_overlay,0,-0.5*support_thick]) linear_extrude(support_thick) {
            difference() {
                square([min_support_h+support_overlay, top_ext_w]);
                union() {
                    for (i = [0: N]) {
                        translate([0, D/2+0.2+i*(D+0.2), 0]) circle(d=D);
                    }
                }
            }
        }
    }
}
module top_support_2() {
    D = 0.4;
    length = top_ext_l + cos(45)*(2*min_support_h);
    N = (length - 0.2) / (D + 0.2);
    px = 0.5*(-top_ext_l + cos(45)*(2*min_support_h));
    py = 0.5*(cos(45)*(2*min_support_h) - px);
    points=[
        [-top_ext_l, 0],
        [cos(45)*(2*min_support_h), 0],
        [px, -53],
    ];
    translate([0,CASE_WIDTH/2,0]) rotate([90,0,0]) {
            //difference() 
            {
                polygon(points);
                union() {
                    for (i = [0: N]) {
                        translate([-top_ext_l, D/2+0.2+i*(D+0.2), 0]) circle(d=D);
                    }
                }
            }
    }
    /*translate([0, 0.5*(CASE_WIDTH-top_ext_w), 0]) {
        rotate([0,45,0]) translate([-support_overlay,0,-0.5*support_thick]) linear_extrude(support_thick) {
            difference() {
                square([min_support_h+support_overlay, top_ext_w]);
                union() {
                    for (i = [0: N]) {
                        translate([0, D/2+0.2+i*(D+0.2), 0]) circle(d=D);
                    }
                }
            }
        }
    }*/
}
module top_support() {
    off_x = cos(45)*(2*min_support_h);
    translate([off_x-eps,0,0]) {
        rotate([0,-45,0]) translate([0,0,-0.5*support_thick]) linear_extrude(support_thick) square([CASE_LENGTH, CASE_WIDTH], center=true);
    }
}
top_support_1();
top_support_2();
top_support();
