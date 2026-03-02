use <../common/shared-utils.scad>
include <../common/shared-variables.scad>

width_x = u_1 * 1.5 - 1;
depth_y = u_1 - 1;
height_z = 3.5;
shell_thickness = 1;

pole_width_x = 1.15;
pole_depth_y = 2.95;
pole_height_z = 3.5;
pole_separation = 5.7;
pole_standoff_height = 0.5;

cap_u_1_5();

module cap_u_1_5() {
    //
    shell([width_x, depth_y]);

    poles();
}

module shell(size) {
    difference() {
        linear_extrude(height_z) {
            rounded_rectangle([width_x, depth_y], corner_radius = 2, center = true);
        }

        translate([0, 0, shell_thickness]) {
            linear_extrude(height_z - shell_thickness) {
                rounded_rectangle([width_x - shell_thickness * 2, depth_y - shell_thickness * 2], corner_radius = 2,
                center = true);
            }
        }
    }
}

module poles() {
    translate([0, 0, shell_thickness]) {
        translate([pole_separation / 2, 0]) {
            pole();
        }
        translate([-pole_separation / 2, 0]) {
            pole();
        }

        linear_extrude(pole_standoff_height) {
            square([pole_separation + 4, pole_depth_y + 1], center = true);
        }
    }
}

module pole() {
    linear_extrude(pole_height_z + pole_standoff_height) {
        difference() {
            square([pole_width_x, pole_depth_y], center = true);

            union() {
                translate([-0.5, 0])
                    square([0.5, pole_depth_y / 2], center = true);
                translate([0.5, 0])
                    square([0.5, pole_depth_y / 2], center = true);
            }
        }
    }
}