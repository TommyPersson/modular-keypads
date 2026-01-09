use <../common/module-common-routines.scad>
use <../common/shared-utils.scad>
include <../common/shared-variables.scad>

leg_width = 35;

pipe_mount_diameter = 55;
pipe_mount_depth = 16;
pipe_separation = 150;
pipe_angle = 10;
pipe_mount_wall_thickness = 10;

screw_hole_diameter = 6;
screw_hole_inset = 7;

support_wall_thickness = 2;
support_wall_height = 8;

rubber_feet_diameter = 10;
rubber_feet_depth = 1.5;

base_y = 30;
base_y = 30;

model();

module model() {

    y_diff = sin(pipe_angle) * pipe_separation;
    x_diff = sqrt(pipe_separation^2 - y_diff^2);

    mount_1_xy = [0, base_y];
    mount_2_xy = [x_diff, base_y + y_diff];

    difference() {
        union() {
            linear_extrude(leg_width) {
                hull() {
                    translate(mount_1_xy) {
                        circle(d = pipe_mount_diameter + 5);
                    }

                    translate(mount_2_xy) {
                        circle(d = pipe_mount_diameter + 5);
                    }

                    translate([mount_1_xy.x, 0]) {
                        rounded_rectangle([pipe_mount_diameter, pipe_mount_diameter], 2, center = true);
                    }

                    translate([mount_2_xy.x, 0]) {
                        rounded_rectangle([pipe_mount_diameter, pipe_mount_diameter], 2, center = true);
                    }
                }
            }
        }

        union() {
            translate(mount_1_xy) {
                pipe_mount_cutout();
            }

            translate(mount_2_xy) {
                pipe_mount_cutout();
            }

            rotate(270, [1, 0, 0]) {
                #translate([0, -leg_width / 2, -pipe_mount_diameter / 2]) {
                    linear_extrude(rubber_feet_depth) {
                        translate([0, -leg_width / 4]) {
                            circle(d = rubber_feet_diameter);
                        }
                        translate([0, leg_width / 4]) {
                            circle(d = rubber_feet_diameter);
                        }
                        translate([x_diff, -leg_width / 4]) {
                            circle(d = rubber_feet_diameter);
                        }
                        translate([x_diff, leg_width / 4]) {
                            circle(d = rubber_feet_diameter);
                        }
                    }
                }
            }
        }
    }
}

module pipe_mount_cutout() {
    linear_extrude(pipe_mount_depth) {
        circle(d = pipe_mount_diameter);
    }

    translate([0, 0, pipe_mount_depth + pipe_mount_wall_thickness]) {
        #linear_extrude(leg_width) {
            circle(d = pipe_mount_diameter);
        }
    }

    // screw holes
    linear_extrude(leg_width) {
        y = pipe_mount_diameter / 2 - screw_hole_inset;
        translate([0, y]) {
            circle(d = screw_hole_diameter);
        }

        rotate(120, [0, 0, 1]) {
            translate([0, y]) {
                circle(d = screw_hole_diameter);
            }
        }

        rotate(240, [0, 0, 1]) {
            translate([0, y]) {
                circle(d = screw_hole_diameter);
            }
        }
    }
}