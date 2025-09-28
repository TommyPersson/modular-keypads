use <../common/module-top-routines.scad>
use <../common/shared-utils.scad>
include <../common/shared-variables.scad>

module_0_top();

module module_0_top() {
    difference() {
        module_top_base(4);

        union() {
            rotary_encoder_cutouts();
            key_switch_cutouts();
            lcd_cutout();
        }
    }
}

module rotary_encoder_cutouts() {
    points = [
            [40, 123.34],
        ];

    module_top_rotary_encoder_cutouts(points);
}

module key_switch_cutouts() {
    offset = [23.7 + outer_wall_thickness_base, 16.6 + outer_wall_thickness_base];

    u_points = [
            [0, 0], [1.5, 0],
            [0, 1], [1.5, 1],
        ];

    points = u_points * u_1;

    translate(offset) {
        module_top_key_switch_cutouts(points);
    }
}

module lcd_cutout() {
    safety = 0.5;
    offset = [24 - safety/2, 55.55 - safety/2];
    lcd_size = [32 + safety/2, 43.7 + safety/2];

    linear_extrude(module_top_thickness) {
        translate(offset) {
            rectangle_with_circle_corners(lcd_size);
        }
    }
}