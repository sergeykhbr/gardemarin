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

