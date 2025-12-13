use <../common/module-common-routines.scad>
use <../common/shared-utils.scad>
include <../common/shared-variables.scad>

module_base_height = 5;
pipe_mount_inner_diameter = 16.4;
pipe_mount_outer_diameter = 20;
pipe_separation = 50;

module_base(4);

module module_base(width_units) {
    size = get_module_size(width_units);

    difference() {
        linear_extrude(module_base_height) {
            square(size);
        }

        translate([0, 0, module_base_height - magnet_depth]) {
            #magnet_hole_cutouts(size, variant = "teardrop", rotation = 90);
        }
    }

    translate([0, module_height_y / 2 + pipe_separation / 2, -pipe_mount_inner_diameter / 2]) {
        pipe_mount(size.x);
    }

    translate([0, module_height_y / 2 - pipe_separation / 2, -pipe_mount_inner_diameter / 2]) {
        pipe_mount(size.x);
    }
}

module pipe_mount(width) {
    rotate(-90, [1, 0, 0]) {
        rotate(90, [0, 1, 0]) {
            linear_extrude(width) {
                difference() {
                    union() {
                        r = pipe_mount_outer_diameter / 2;
                        points = [
                                [-2 * r, -r],
                                [-r, 0],
                                [r, 0],
                                [2 * r, -r]
                            ];
                        polygon(points);
                        circle(d = pipe_mount_outer_diameter);

                    }
                    circle(d = pipe_mount_inner_diameter);
                }
            }
        }
    }
}