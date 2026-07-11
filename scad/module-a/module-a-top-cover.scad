use <../common/module-top-routines.scad>
use <../common/shared-utils.scad>
include <../common/shared-variables.scad>

module_a_top();

module module_a_top() {
    difference() {
        module_top_cover_base(4);

        union() {
            key_cluster_cutouts();
            #top_cover_sticker_cutout(4);
        }
    }
}

module top_cover_sticker_cutout(width_units) {
    inset_x = 10;
    inset_y = 10;
    module_width = (width_units * module_width_x_unit);
    width = module_width - (inset_x*2);
    height = 10;
    depth = 1;

    translate([module_width/2, module_height_y - inset_y, module_top_cover_thickness - depth]) {
        linear_extrude(depth) {
            rounded_rectangle([width, height], corner_radius = 2, center = true);
        }
    }
}

module key_cluster_cutouts() {
    offset = [23.7 + outer_wall_thickness_base, 16.6 + outer_wall_thickness_base];

    u_point_clusters = [
            [[0, 0], [1.5, 0],
                [0, 1], [1.5, 1]],
            [[0, 2.5], [1.5, 2.5],
                [0, 3.5], [1.5, 3.5]],
            [[0, 5], [1.5, 5],
                [0, 6], [1.5, 6]],
        ];


    linear_extrude(module_top_cover_thickness) {
        translate(offset) {
            for (cluster = u_point_clusters) {
                points = cluster * u_1;
                hull() {
                    for (p = points) {
                        translate(p) {
                            size = [u_1 * 1.5, u_1];
                            rounded_rectangle(size, corner_radius = 2, center = true);
                        }
                    }
                }
            }
        }
    }
}