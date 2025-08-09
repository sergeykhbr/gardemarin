include <encloser_v1_common.scad>

module btn_tunnel_test(pos_l=25, pos_w=15, btn_sz=8, btn_depth=20) {
    dlt_down = 1.2;
    dlt_up= 1;
    stem_d = 3;
    dlt_stem_d = 0.3;
    btn_h = 5;
    dlt_btn_h = 0.3;
    stem_h = btn_depth - (btn_h + Thickness) + dlt_down;
    translate([pos_l, pos_w, -top_height]) {
        union() {
            //translate([0,0,-(top_h_inclin+eps)]) linear_extrude(btn_h+top_h_inclin+eps) square(btn_sz, center=true);
            translate([0,0,-dlt_up-eps]) linear_extrude(btn_depth) circle(d=stem_d - dlt_stem_d);
            translate([0,0,btn_h+Thickness]) linear_extrude(stem_h) circle(d=btn_sz - dlt_btn_h);
        }
    }
}

module btn_tunnel_cap() {
    stem_d = 2.7;
    btn_sz = 6-0.5;
    r = 1.2;
    difference() 
    {
        hull() 
        {
            translate([0,0,3]) linear_extrude(0.1) square([btn_sz,btn_sz], center=true);
            
            hull() 
            {
                translate([-btn_sz/2+r, -btn_sz/2+r, 0]) sphere(r=r, $fn=64);
                translate([-btn_sz/2+r, btn_sz/2-r]) sphere(r=r, $fn=64);
                translate([btn_sz/2-r, -btn_sz/2+r]) sphere(r=r, $fn=64);
                translate([btn_sz/2-r, btn_sz/2-r]) sphere(r=r, $fn=64);
            }
        }
        translate([0,0,1]) linear_extrude(2.5) circle(d=stem_d);
    }
}
btn_tunnel_cap();

//rotate([0,180,0]) btn_tunnel_test(pos_l=23.8, pos_w=13.7, btn_sz=6, btn_depth=18);
