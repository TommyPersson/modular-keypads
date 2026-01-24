use <../common/module-top-routines.scad>
use <../common/shared-utils.scad>
include <../common/shared-variables.scad>

$fn = 50;

width_units = 4;

height = 3;

connector_width = 12.2; // real = 11.9
connector_depth = 3.5;

mounting_pole_height = 1.4;
mounting_pole_diameter = 2.1;

base();

module base() {
    difference() {
        union() {
            main_body();
            translate([0, 0, -mounting_pole_height]) {
                mounting_poles();
            }
        }

        connector_cutout();
    }
}

module main_body() {
    linear_extrude(height) {
        translate([-30, -1]) {
            rounded_rectangle([60, 26], corner_radius = 2);
        }
    }
}

module mounting_poles() {
    points = [
            [26, 1],
            [-26, 1],
            [26, 24],
            [-26, 24],
        ];

    linear_extrude(mounting_pole_height) {
        for (p = points) {
            translate(p) {
                circle(d = mounting_pole_diameter);
            }
        }
    }
}

module connector_cutout() {
    linear_extrude(height) {
        rectangle_with_circle_corners([connector_width, connector_depth], circle_radius = 0.5, center = true);
    }

    linear_extrude(0.5) {
        offset(r = 2) {
            rectangle_with_circle_corners([connector_width, connector_depth], circle_radius = 0.5, center = true);
        }
    }
}