include <encloser_v1_common.scad>

min_support_h = 5;
support_overlay = 0.2;

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
        square([98.6, 10.5+eps]);
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
module enclose_usb_support() {
    h = 9.4;
    w = 5;
    translate([Thickness-support_thick, 4.4 + h/2, -w/2+eps]) rotate([0,90,0]) linear_extrude(support_thick) {
        square_support(w=w, h=h, cut=1.2, nocut=0.2);
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

module enclose_rj45_support(w_pos=10) {
    w = 14;
    h = 17.1;
    translate([Thickness-support_thick, w_pos + h/2, -w/2+eps]) rotate([0,90,0]) linear_extrude(support_thick) {
        square_support(w=w, h=h, cut=1.2, nocut=0.2);
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
module enclose_pwr_support(w_pos=10) {
    w = 9;
    h = 11.2;
    translate([CASE_LENGTH-support_thick-Thickness/2, w_pos + h/2, -w/2+eps]) rotate([0,90,0]) linear_extrude(support_thick) {
        square_support(w=w, h=h, cut=1.2, nocut=0.2);
    }
}

// Uncover 2 of 3 connectors (1 unused)
module enclose_relais1_diff() {
    translate([CASE_LENGTH-Thickness-1, 43.2, eps]) rotate([0,90,0]) linear_extrude(Thickness + 2) {
        square([9, 11.2]);
    }
}
module enclose_relais1_frame() {
    translate([CASE_LENGTH-Thickness, 41.2, eps]) rotate([0,90,0]) linear_extrude(Thickness + 0.2) {
        square([11, 15.2]);
    }
}

module enclose_relais2_diff() {
    translate([CASE_LENGTH-Thickness-1, 27.1, eps]) rotate([0,90,0]) linear_extrude(Thickness + 2) {
        square([9, 11.2]);
    }
}
module enclose_relais2_frame() {
    translate([CASE_LENGTH-Thickness, 24.9, eps]) rotate([0,90,0]) linear_extrude(Thickness + 0.2) {
        square([11, 15.2]);
    }
}

// Temperature sensor hole with round corners
module enclose_T1T2_frame() {
    radius = 4.5;
    translate([CASE_LENGTH-Thickness, 10.1, 1.5 + eps]) rotate([0,90,0]) linear_extrude(Thickness + 0.2) {
        hull() {
            translate([12, radius]) circle(d=2*radius);
            translate([1.5, 0]) square([12, 2*radius]);
        }
    }
}
module enclose_T1T2_diff() {
    radius = 2.5;
    translate([CASE_LENGTH-Thickness-1, 12.1, 1.5 + eps]) rotate([0,90,0]) linear_extrude(Thickness + 2) {
        hull() {
            translate([12, radius]) circle(d=2*radius);
            translate([-eps,0,0]) square([12, 2*radius]);
        }
    }
}

module display_latch(H, L=3, W=4, sup1=0, sup2=0) {
    platch = [
        [0,0],
        [2,0],
        [2,H],
        [2 + L, H],
        [2 + L, H + 2],
        [0,H + 2],
    ];
    rotate([90, 0, 0]) linear_extrude(W) polygon(platch);

    if (sup1) {
        translate([0, H-0.4, 0.7]) rotate([135, 0, 0]) {
            linear_extrude(support_thick) {
                translate([L/2+2,H/2,0]) square_support(w=L, h=sqrt(2)*H+2*eps, cut=0.8, nocut=0.2);
            }
        }
    }
    if (sup2) {
        translate([0,-W+support_thick-H+0.2, 0.1]) {
            rotate([45, 0, 0]) linear_extrude(support_thick) {
                translate([L/2+2,H/2,0]) square_support(w=L, h=sqrt(2)*H+2*eps, cut=0.8, nocut=0.2);
            }
        }
    }
}

module enclose_display_frame(w=24.2,h=24.2,l_off=20) {
    // Frame and latches aligned with *_diff using lcd_test module()
    // TODO: board size relative frame w,h
    lcd_board_w = 28;
    lcd_board_h = 39;
    lcd_board_t = 3.4;
    translate([2+l_off,-2+0.5*(CASE_WIDTH-w),-(top_height+top_ext_h)]) union() {
        union() {
            translate([0,0,Thickness-eps]) union() {
                translate([h+12.8, 0.5*(lcd_board_w - 8), 0]) 
                    rotate([0, 0, 180]) 
                        display_latch(H=lcd_board_t, L=2, W=8);
                
                translate([0.5*(lcd_board_h+10), lcd_board_w+2, 0])
                    rotate([0, 0, -90])
                        display_latch(H=lcd_board_t, L=3, W=20, sup1=1);
                        
                translate([0.5*(lcd_board_h-30), -2, 0])
                    rotate([0, 0, 90])
                        display_latch(H=lcd_board_t, L=3, W=20, sup2=1);
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
            translate([0,0,-(Thickness + 2*eps)]) {
                linear_extrude(0.1) square([w + 2*frame_w, h + 2*frame_w]);
            }
        }
    }
}
module enclose_display_support(w=20, h=10, l_off=20) {
    frame_w = 3;
    cut = 0.8;
    nocut = 0.2;
    N = (w + 2*frame_w)/(cut+nocut);
    alpha = atan(frame_w/Thickness);
    translate([l_off, 0.5*(CASE_WIDTH - (h+2*frame_w)), -(top_height + top_h_inclin + top_ext_h - Thickness - 1) + eps])
    {
        difference() {
        hull() {
            linear_extrude(0.1) translate([frame_w-eps, frame_w+nocut]) {
                square([5,h - 2*nocut]);
            }
            translate([-eps, nocut, -(Thickness + 2*eps)]) linear_extrude(0.1) {
                square([5+frame_w, h + 2*(frame_w-nocut)]);
            }
        }
        union() {
            for (i=[0:N]) {
                translate([-(3-Thickness+cut/2), 0 + i*(cut+nocut), -3]) {
                    rotate([0,alpha,0]) linear_extrude(6) circle(d=cut);
                }
            }
        }
        }
    }
}

module enclose_jtag_frame(depth=20, w=40, l=8) {
    D=16;
    translate([32.5, CASE_WIDTH/2 + 3.4, -(top_height+top_h_inclin-depth-eps)]) {
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
    translate([32.5, CASE_WIDTH/2 + 3.4, -(top_height+top_h_inclin-depth-eps)]) {
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
module enclose_jtag_support(depth=20, w=40, l=8) {
    th=1.2;
    D=16;
    translate([32.5, CASE_WIDTH/2 + 3.4, -(top_height+top_h_inclin)]) 
    {
        union() {
            translate([0,0,depth-1.2]) linear_extrude(support_thick) {
                square_support(w=l-2*th+2*eps, h=w-2*th+2*eps, cut=1.2, nocut=0.2);
            }
            linear_extrude(support_thick) {
                square_support(w=l-2*th+2*eps, h=w-2*th+2*eps, cut=1.2, nocut=0.2);
            }
            linear_extrude(support_thick) {
                hull() 
                {
                    translate([-(l-th)/2, +4, 0]) circle(d=D-th-1);
                    translate([-(l-th)/2, -4, 0]) circle(d=D-th-1);
                    translate([(l-th)/2, +4, 0]) circle(d=D-th-1);
                    translate([(l-th)/2, -4, 0]) circle(d=D-th-1);
                }
                translate([-(l-th)/2, +4, 0]) circle_support(d=D-th+eps, cut=2.0, nocut=0.2);
                translate([-(l-th)/2, -4, 0]) circle_support(d=D-th+eps, cut=2.0, nocut=0.2);
                translate([(l-th)/2, +4, 0]) circle_support(d=D-th+eps, cut=2.0, nocut=0.2);
                translate([(l-th)/2, -4, 0]) circle_support(d=D-th+eps, cut=2.0, nocut=0.2);
            }
        }
    }
}

module enclose_mosfet_frame(depth=16, w=0.5*(CASE_WIDTH-top_ext_w), l=14) {
    dlt_h = w / (0.5*(CASE_WIDTH-top_ext_w));
    translate([0, CASE_WIDTH-w, -top_height+depth+0.1]) rotate([-90,0,0]) rotate([0,-90,0]) {
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

module btn_tunnel_frame(pos_l=25, pos_w=15, btn_sz = 8, btn_depth = 20) {
    translate([pos_l, pos_w, -(top_height)]) {
        linear_extrude(btn_depth) square(btn_sz+2, center=true);
    }
}
module btn_tunnel_diff(pos_l=25, pos_w=15, btn_sz=8, btn_depth=20) {
    stem_d = 3;
    btn_h = 5;
    stem_h = btn_depth - (btn_h + Thickness);
    translate([pos_l, pos_w, -(top_height)]) {
        union() {
            translate([0,0,-(top_h_inclin+eps)]) linear_extrude(btn_h+top_h_inclin+eps) square(btn_sz, center=true);
            translate([0,0,btn_h-eps]) linear_extrude(stem_h+2*eps) circle(d=stem_d);
            translate([0,0,btn_h+Thickness]) linear_extrude(stem_h+eps) circle(d=btn_sz);
        }
    }
}
module btn_tunnel_support(pos_l=25, pos_w=15, btn_sz = 8, btn_depth = 20) {
    stem_d = 3;
    btn_h = 5;
    dlt_w = 0.5*(CASE_WIDTH - top_ext_w);
    alpha = atan(1.0/dlt_w);
    dlt_z = pos_w  * tan(alpha);
    translate([pos_l, pos_w, -top_height]) 
    {
        translate([0,0,-dlt_z]) rotate([-alpha,0,0]) linear_extrude(support_thick) {
            square_support(w=btn_sz+2*eps, h=btn_sz+2*eps, cut=1.2, nocut=0.2);
        }
        translate([0,0,btn_h+support_thick/2]) circle_support(d=stem_d, cut=0.8, nocut=0.2);
    }
}

module stand_screw_m4_frame(l=10, w=10, depth=3, d=8, d_screw=M4_diameter) {
    // thcikness of wall =0.8 (not equal to Thickness). +5 additional diff in encloser inclination
    translate([l, w, -(top_height - depth - eps)]) {
        translate([0,0,-depth]) difference() {
            linear_extrude(depth) circle(d=d + 1.6);
            translate([0,0, -1]) linear_extrude(depth+2) circle(d=d_screw);
        }
    }
}
module stand_screw_m4_diff(l=10, w=10, depth=3, d=8, d_screw=M4_diameter) {
    depth_full = depth + 5 + eps;
    translate([l, w, -(top_height - depth - eps)]) {
        translate([0,0, -Thickness - depth_full]) {
            linear_extrude(depth_full) circle(d=d);
        }
    }
}
module stand_screw_m4_support(l=10, w=10, depth=3, d=8, d_screw=M4_diameter) {
    dlt_w = 0.5*(CASE_WIDTH - top_ext_w);
    alpha = atan(1.0/dlt_w);
    alpha_sign = (w < dlt_w) ? -1.0 :
                 (w > (dlt_w + top_ext_w)) ? 1.0 : 0.0;
    dlt_z = (w < dlt_w) ? w  * tan(alpha) :
            (w > (dlt_w + top_ext_w)) ? (CASE_WIDTH - w)  * tan(alpha) : top_h_inclin;
    translate([l, w, -top_height - dlt_z]) 
    {
        translate([0,0,support_thick/2-0.3]) rotate([alpha_sign*alpha,0,0]) {
            linear_extrude(support_thick) circle_support(d=d, cut=0.8, nocut=0.2);
        }
        translate([0,0,depth -Thickness + support_thick/2]) {
            linear_extrude(support_thick) circle_support(d=d_screw, cut=0.6, nocut=0.2);
        }
    }
}


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
    D = 2.8;
    dlt=0.4;
    N = (CASE_WIDTH - dlt) / (D + dlt);
    H = min_support_h + support_overlay + 1.1;
    translate([13.8, 0, 15.8]) rotate([0,45,0]) 
    {
        translate([-support_overlay,0,-0.5*support_thick]) {
            translate([H,CASE_WIDTH/2,0]) rotate([90,0,0]) {
                linear_extrude(support_thick) polygon([[0,5], [0,-5], [-5,0]]);
            }
            
            linear_extrude(support_thick) difference() {
                square([H, CASE_WIDTH]);
                union() {
                    for (i = [0: N]) {
                        translate([0, D+dlt+i*(D+dlt), 0]) circle(d=D);
                    }
                }
            }
        }
    }
}
module top_support_3() {
    D = 1.0;
    length = top_ext_l + cos(45)*(2*min_support_h);
    N = (length - 0.2) / (D + 0.2);
    px = length/sqrt(2);
    points=[
        [-top_ext_l, 0],
        [cos(45)*(2*min_support_h), 0],
        [-px+cos(45)*(2*min_support_h), -px],
    ];
    translate([0,CASE_WIDTH/2,eps]) rotate([90,0,0]) linear_extrude(support_thick) {
            difference()
            {
                polygon(points);
                union() {
                    for (i = [0: N]) {
                        translate([-top_ext_l + D/2+0.4+i*(D+0.2), 0, 0]) circle(d=D);
                    }
                }
            }
    }
}
module top_support_4() {
    off_x = cos(45)*(2*min_support_h);
    translate([off_x-eps, CASE_WIDTH/2, 0]) {
        rotate([0,-45,0]) translate([0,0,-0.5*support_thick]) linear_extrude(support_thick) {
            hull() {
                translate([12, CASE_WIDTH/2, 0]) circle(d=20);
                translate([12, -CASE_WIDTH/2, 0]) circle(d=20);
                translate([-top_ext_l, 0, 0]) circle(d=20);
            }
        }
    }
}
module top_support_5() {
    display_w = 26.4+4;
    D = 1.4;
    dlt=0.2;
    N = (display_w - dlt) / (D + dlt);
    translate([-47.1, 0.5*CASE_WIDTH, 0.0]) {
        rotate([0,45,0]) translate([-support_overlay, 0, -0.5*support_thick]) {
            linear_extrude(support_thick) difference() 
            {
                polygon([[0, -0.5*display_w+0.6], [display_w, 0], [0, 0.5*display_w-0.6]]);
            }
        }
    }
}
/*module top_support_6() {
    jtag_w = 35;
    D = 1.4;
    dlt=0.2;
    N = (jtag_w - dlt) / (D + dlt);
    translate([-116.2, 0.5*CASE_WIDTH, 14.9]) {
        rotate([0,45,0]) translate([-support_overlay,0,-0.5*support_thick]) linear_extrude(support_thick) {
            difference() {
                polygon([[0, -0.38*jtag_w], [2*jtag_w, 0], [0, 0.56*jtag_w]]);
                union() {
                    for (i = [0: N]) {
                        translate([0, -0.38*jtag_w +i*(D+dlt), 0]) circle(d=D);
                    }
                }
            }
        }
    }
}*/
module enclose_top_support() {
    translate([-(-CASE_LENGTH + (CASE_LENGTH - top_ext_l_off - top_ext_l)),
               0,
               -(top_height+top_ext_h)]) {
        top_support_1();
        top_support_2();
        top_support_3();
        top_support_4();
        top_support_5();
        //top_support_6();
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

    scale3_depth = 17;
    scale3_w = 13;
    scale3_l = 18;
    scale3_l_pos = 72.6;

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
            enclose_jtag_frame(depth=15, w=35, l=11.5);
            btn_tunnel_frame(pos_l=23.8, pos_w=13.7, btn_sz=6, btn_depth=18);
            btn_tunnel_frame(pos_l=23.8, pos_w=7.1, btn_sz=6, btn_depth=18);
            translate([mosfet_l_pos, 0, 0]) enclose_mosfet_frame(depth=mosfet_depth, w=mosfet_w, l=mosfet_l);
            translate([scale1_l_pos, 0, 0]) enclose_mosfet_frame(depth=scale1_depth, w=scale1_w, l=scale1_l);
            translate([scale2_l_pos, 0, 0]) enclose_mosfet_frame(depth=scale2_depth, w=scale2_w, l=scale2_l);
            translate([scale3_l_pos, 0, 0]) enclose_mosfet_frame(depth=scale3_depth, w=scale3_w, l=scale3_l);
            stand_screw_m4_frame(l=38.2, w=73, depth=15, d=8, d_screw=M4_diameter);
            stand_screw_m4_frame(l=151.2, w=7, depth=15, d=8, d_screw=M4_diameter);
            stand_screw_m4_frame(l=150.6, w=64.7, depth=7, d=4.5, d_screw=4.5);
            stand_screw_m4_frame(l=150.6, w=59.5, depth=7, d=4.5, d_screw=4.5);
            stand_screw_m4_frame(l=150.6, w=51.5, depth=7, d=4.5, d_screw=4.5);
            stand_screw_m4_frame(l=150.6, w=46.3, depth=7, d=4.5, d_screw=4.5);
            stand_screw_m4_frame(l=150.6, w=34.9, depth=7, d=4.5, d_screw=4.5);
            stand_screw_m4_frame(l=150.6, w=29.7, depth=7, d=4.5, d_screw=4.5);
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
            enclose_jtag_diff(depth=15, w=35, l=11.5);
            btn_tunnel_diff(pos_l=23.8, pos_w=13.7, btn_sz=6, btn_depth=18);
            btn_tunnel_diff(pos_l=23.8, pos_w=7.1, btn_sz=6, btn_depth=18);
            translate([mosfet_l_pos, 0, 0]) enclose_mosfet_diff(depth=mosfet_depth, w=mosfet_w, l=mosfet_l);
            translate([scale1_l_pos, 0, 0]) enclose_mosfet_diff(depth=scale1_depth, w=scale1_w, l=scale1_l);
            translate([scale2_l_pos, 0, 0]) enclose_mosfet_diff(depth=scale2_depth, w=scale2_w, l=scale2_l);
            translate([scale3_l_pos, 0, 0]) enclose_mosfet_diff(depth=scale3_depth, w=scale3_w, l=scale3_l);
            stand_screw_m4_diff(l=38.2, w=73, depth=15, d=8);
            stand_screw_m4_diff(l=151.2, w=7, depth=15, d=8);
            stand_screw_m4_diff(l=150.6, w=64.7, depth=7, d=4.5, d_screw=4.5);
            stand_screw_m4_diff(l=150.6, w=59.5, depth=7, d=4.5, d_screw=4.5);
            stand_screw_m4_diff(l=150.6, w=51.5, depth=7, d=4.5, d_screw=4.5);
            stand_screw_m4_diff(l=150.6, w=46.3, depth=7, d=4.5, d_screw=4.5);
            stand_screw_m4_diff(l=150.6, w=34.9, depth=7, d=4.5, d_screw=4.5);
            stand_screw_m4_diff(l=150.6, w=29.7, depth=7, d=4.5, d_screw=4.5);
            // Hole for LED blinking:
            translate([32, Thickness-eps, -2]) rotate([90,0,0]) linear_extrude(Thickness+2*eps) circle(d=2);
        }
    }
    // Add after everything subtracted
    enclose_display_frame(w=(26.4-2), h=(26.4-2), l_off=90);
    enclose_display_support(w=(26.4-2), h=(26.4-2), l_off=90);
    enclose_usb_support();
    enclose_rj45_support(w_pos=21);
    enclose_rj45_support(w_pos=40);
    enclose_rj45_support(w_pos=58.5);
    enclose_jtag_support(depth=15, w=35, l=11.5);
    enclose_pwr_support(w_pos=56.5);
    enclose_pwr_support(w_pos=43.2);
    enclose_pwr_support(w_pos=27.1);
    btn_tunnel_support(pos_l=23.8, pos_w=13.7, btn_sz=6, btn_depth=18);
    btn_tunnel_support(pos_l=23.8, pos_w=7.1, btn_sz=6, btn_depth=18);
    stand_screw_m4_support(l=38.2, w=73, depth=15, d=8, d_screw=M4_diameter);
    stand_screw_m4_support(l=151.2, w=7, depth=15, d=8, d_screw=M4_diameter);
    stand_screw_m4_support(l=150.6, w=64.7, depth=7, d=4.5, d_screw=4.5);
    stand_screw_m4_support(l=150.6, w=59.5, depth=7, d=4.5, d_screw=4.5);
    stand_screw_m4_support(l=150.6, w=51.5, depth=7, d=4.5, d_screw=4.5);
    stand_screw_m4_support(l=150.6, w=46.3, depth=7, d=4.5, d_screw=4.5);
    stand_screw_m4_support(l=150.6, w=34.9, depth=7, d=4.5, d_screw=4.5);
    stand_screw_m4_support(l=150.6, w=29.7, depth=7, d=4.5, d_screw=4.5);
    enclose_top_support();
}

module enclose_top_with_stands(h, w, l) {
    //translate([0,0,bottom_height]) rotate([0,180,0])
    {
        color([0,0.8,1, 0.5]) enclose_top_with_ext(h=h, w=w, l=l);
    }
}

rotate([0,45,0])
enclose_top_with_stands(h=top_height, w=CASE_WIDTH, l=CASE_LENGTH);
