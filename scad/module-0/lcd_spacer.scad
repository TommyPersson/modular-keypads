use <../common/module-top-routines.scad>
use <../common/shared-utils.scad>
include <../common/shared-variables.scad>
include <./_variables.scad>

lcd_spacer();

lcd_spacer_height_z = 3;
lcd_screw_hole_diameter = 2.7;

module lcd_spacer() {
    difference() {
        lcd_spacer_base();

        union() {
            screw_hole_cutouts();
            //lcd_spacer_base_cutout();
        }
    }
}

module lcd_spacer_base() {
    side_width = 5;

    linear_extrude(lcd_spacer_height_z) {
        rounded_rectangle([side_width, lcd_size.y], corner_radius = 1);

        translate([lcd_size.x - side_width, 0]) {
            rounded_rectangle([side_width, lcd_size.y], corner_radius = 1);
        }

        translate([0, lcd_size.y - side_width]) {
            rounded_rectangle([lcd_size.x, side_width], corner_radius = 1);
        }
        //rounded_rectangle(lcd_size, corner_radius = 1);
    }
}

module lcd_spacer_base_cutout() {
    inset = 4.5;
    size = [
            lcd_size.x - inset * 2,
            lcd_size.y - inset
        ];

    linear_extrude(lcd_spacer_height_z) {
        translate([inset, 0]) {
            rounded_rectangle(size, corner_radius = 1);
        }
    }
}


module screw_hole_cutouts() {
    points = [
            [2.5, 2.5],
            [2.5, lcd_size.y - 2.5],
            [lcd_size.x - 2.5, 2.5],
            [lcd_size.x - 2.5, lcd_size.y - 2.5],
        ];

    linear_extrude(lcd_spacer_height_z) {
        for (p = points) {
            translate(p) {
                circle(d = lcd_screw_hole_diameter);
            }
        }
    }
}