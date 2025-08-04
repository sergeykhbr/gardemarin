use <MCAD/boxes.scad>
$fa=1.0;
$fs=0.4;

support_thick = 0.8;
Thickness = 1.2;
M4_diameter = 3.8;

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
    Low = 2.25;    // 4 mm is a maximum height of din = 0.5*(37-25)
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
    
    translate([0.01, y/2, 4]) din_slot(width=6);
    translate([0.01, y/2, z - 6 - 4]) din_slot(width=6);
}

module enclose_top_side(height=100, width=20, thick=1) {
    D_corner = 4;
    linear_extrude(thick) {
        hull() {
            translate([height - D_corner/2, width - D_corner/2, 0]) circle(d=D_corner);
            translate([height - D_corner/2, D_corner/2, 0]) circle(d=D_corner);
            square([1, width]);
        }
    }
}

module enclose_top(x=100, y=20, z=20) {
    
    // Right top side
    translate([x,0,0]) rotate([0,-90,0]) {
        enclose_top_side(height=x, width=y, thick=Thickness);
        translate([-1.5, Thickness/2, Thickness/2 - 0.01]) {
            enclose_top_side(height=(x + 1.5), width = (y - 2*Thickness), thick=Thickness/2 + 0.01);
        }
    }
    
    /*difference() {
        union() {
            hull() {
                translate([x - D_corner/2, y - D_corner/2, 0]) circle(d=D_corner);
                translate([x - D_corner/2, D_corner/2, 0]) circle(d=D_corner);
                square([1, y]);
            }
            translate([-1.5, Thickness/2, 0]) square([1.501, y - Thickness]);
        }
        hull() {
            translate([x - Thickness - D_corner/2, y - Thickness - D_corner/2, 0]) circle(d=D_corner);
            translate([x - Thickness - D_corner/2, Thickness + D_corner/2, 0]) circle(d=D_corner);
            translate([-1.501, Thickness, 0]) square([3, y - 2*Thickness]);
        }
    }*/
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

module enclose_bottom_with_stands() {
    rotate([0,-90,0]) color([0,1,1]) enclose_bottom(x=15, y=80, z=156);
    translate([-37, 23.9, 0]) color([1,0,0]) stand(z=13.6);
    translate([-37, 73.0, 0]) color([1,0,0]) stand(z=13.6, cut=1);
    translate([-150, 73.0, 0]) color([1,0,0]) stand(z=13.6, cut=1);
    translate([-150, 7.0, 0]) color([1,0,0]) stand(z=13.6);
}

module support_cut_circle(x=60, r=6) {
    Diam = 2*r;
    Distance = x - support_thick - Diam;
    y_offset = 0.3 * r > support_thick ? support_thick: 0.3 * r;
    translate([0,0,-0.001]) linear_extrude(support_thick + 0.002) {
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
            translate([6,40,0]) rotate([0,135,0]) translate([15,0,0]) linear_extrude(support_thick + 0.01) {
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

//din_rail(len=20);
enclose_bottom_with_stands();
translate([-192,-23,15]) color([0.5,1,0, 0.3]) import("main_board.stl",convexity=5);
enclose_top(x=15, y=80, z=156);
//case_support();

