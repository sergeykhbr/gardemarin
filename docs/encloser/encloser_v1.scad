use <MCAD/boxes.scad>
$fa=1.0;
$fs=0.4;

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

//din_rail(len=20);
enclose_bottom_with_stands();
//translate([-192,-23,15]) color([0.5,1,0]) import("main_board.stl",convexity=5);

//translate([-36, 23, 15 - 1.4]) linear_extrude(2) square([10,100]);       // 1.6 = board thickness
