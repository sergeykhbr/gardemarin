include <encloser_v1_common.scad>

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
        //[gap - 0.4, 35],
        //[gap - 1*gap - 0.4, 33],
        //[gap - 1*gap - 0.4, 31],
        //[gap, 27],
        
        [gap, -gap + 0],
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
    
    translate([eps, y/2, 0]) din_slot(width=CASE_LENGTH);
    //translate([eps, y/2, z - 6 - 4]) din_slot(width=6);
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


module bottom_support_1() {
    l = 130;
    xcut = 22;
    translate([-l-15, CASE_WIDTH/2, -5+eps]) rotate([90,0,0]) {
        polygon_support(points=[[0,0], [l, 0], [l,-xcut], [0.5*(l+xcut),-0.5*(l+xcut)]], cut=1.0, nocut=0.3, off=0);
    }
    //small triangle for stability
    translate([-20, CASE_WIDTH/2, -32]) rotate([0,-135,0]) {
        linear_extrude(support_thick) polygon([[0,10], [10,0], [0,-10]]);
    }
}
module bottom_support_2() {
    x1 = -37.5;
    x2 = 31.5;
    h = 8.1;
    translate([-0.1, CASE_WIDTH/2, 0.1]) rotate([0,45,0]) 
    {
        polygon_support(points=[[0,x1], [0, x2], [h,x2], [h,x1]], cut=1.2, nocut=0.3, off=0);
    }
}
module bottom_support_3() {
    x1 = -18.5;
    x2 = 18.5;
    h = 5.0;
    translate([-0.1, CASE_WIDTH/2, -5+0.1]) rotate([0,45,0]) 
    {
        polygon_support(points=[[0,x1], [0, x2], [h,x2], [h,x1]], cut=1.2, nocut=0.3, off=0);
    }
}
module bottom_support_4() {
    x1 = -20.5;
    x2 = 20.5;
    h = 2.2;
    translate([-0.1, CASE_WIDTH/2, -9+0.1]) rotate([0,45,0]) 
    {
        polygon_support(points=[[0,x1], [0, x2], [h,x2], [h,x1]], cut=1.2, nocut=0.3, off=0);
    }
}
module bottom_support_5() {
    x1 = -5;
    x2 = 4.;
    h = 10.0;
    translate([-0.1, 36.5+CASE_WIDTH/2, 2.1]) rotate([0,45,0]) 
    {
        rotate([18,0,0]) polygon_support(points=[[1.5,x1], [-1.5, x2], [h,x2], [h,x1]], cut=1.2, nocut=0.3, off=0);
    }
}
module bottom_support_6() {
    x1 = -2.0;
    x2 = 2.0;
    h = 3.2;
    translate([-0.1, -22.5+CASE_WIDTH/2, -7.5]) rotate([0,45,0]) 
    {
        rotate([-27,0,0]) polygon_support(points=[[-1.2,x1], [0.7, x2], [h,x2], [h,x1]], cut=1.2, nocut=0.3, off=0);
    }
}
module bottom_support_7() {
    x1 = -2.0;
    x2 = 2.0;
    h = 3.2;
    translate([-0.1, 22.1+CASE_WIDTH/2, -7.7]) rotate([0,45,0]) 
    {
        rotate([18,0,0]) polygon_support(points=[[0.8,x1], [-0.4, x2], [h,x2], [h,x1]], cut=1.2, nocut=0.3, off=0);
    }
}
module bottom_support_base() {
    translate([6,40,-5]) rotate([0,135,0]) translate([15,0,0]) linear_extrude(support_thick + eps) {
        hull() {
            translate([-20,40,0]) circle(d=30);
            translate([-20,-40,0]) circle(d=30);
            translate([85,0,0]) circle(d=30);
        }
    }
}

module enclose_bottom_with_stands(x=15, y=80, z=159) {
    rotate([0,-90,0]) color([0,1,1]) enclose_bottom(x=x, y=y, z=z);
    translate([-38.2, 23.9, 0]) color([1,0,0]) stand(z=13.6);
    translate([-38.2, 73.0, 0]) color([1,0,0]) stand(z=13.6, cut=1);
    translate([-151.2, 73.0, 0]) color([1,0,0]) stand(z=13.6, cut=1);
    translate([-151.2, 7.0, 0]) color([1,0,0]) stand(z=13.6);

    bottom_support_1();
    bottom_support_2();
    bottom_support_3();
    bottom_support_4();
    bottom_support_5();
    bottom_support_6();
    bottom_support_7();
    bottom_support_base();
}

rotate([0,45,0])
enclose_bottom_with_stands(x=bottom_height, y=CASE_WIDTH, z=CASE_LENGTH);

