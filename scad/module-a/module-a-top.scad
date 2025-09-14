use <../common/module-top-routines.scad>
use <../common/shared-utils.scad>
include <../common/shared-variables.scad>

module_a_top();

module module_a_top() {
    difference() {
        module_top_base(4);

        key_switch_cutouts();
    }
}

module key_switch_cutouts() {
    offset = [23.7 + outer_wall_thickness_base, 16.6 + outer_wall_thickness_base];

    u_points = [
            [0, 0], [1.5, 0],
            [0, 1], [1.5, 1],
            [0, 2.5], [1.5, 2.5],
            [0, 3.5], [1.5, 3.5],
            [0, 5], [1.5, 5],
            [0, 6], [1.5, 6],
        ];

    points = u_points * u_1;


    translate(offset) {
        module_top_key_switch_cutouts(points);
    }
}