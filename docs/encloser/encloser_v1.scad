use <MCAD/boxes.scad>
include <encloser_v1_common.scad>
include <encloser_v1_top.scad>
include <encloser_v1_bottom.scad>

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


//lcd_test();
//case_support();
