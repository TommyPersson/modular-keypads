use <../common/module-common-routines.scad>
use <../common/shared-utils.scad>
include <../common/shared-variables.scad>

module_base_height = 5;
pipe_mount_inner_diameter = 25;
pipe_mount_outer_diameter = pipe_mount_inner_diameter + 2;
pipe_separation = 150;
support_wall_thickness = 2;
support_wall_height = 8;

module_base(4);

module module_base(width_units) {
    size = get_module_size(width_units);

    intersection() {
        union() {
            // main body
            difference() {
                linear_extrude(module_base_height) {
                    square(size);
                }

                translate([0, 0, module_base_height - magnet_depth]) {
                    #magnet_hole_cutouts(size, variant = "teardrop", rotation = 90);
                }
            }

            // near grip
            translate([0, module_height_y / 2 + pipe_separation / 2, -pipe_mount_inner_diameter / 2]) {
                pipe_mount(size.x);
            }

            // far grip
            translate([0, module_height_y / 2 - pipe_separation / 2, -pipe_mount_inner_diameter / 2]) {
                pipe_mount(size.x);
            }

            // near wall
            translate([0, -support_wall_thickness]) {
                linear_extrude(module_base_height + support_wall_height) {
                    square([size.x, support_wall_thickness]);
                }
            }
        }

        translate([1, -support_wall_thickness, -50]) {
            // bounding box
            linear_extrude(100) {
                rounded_rectangle([size.x - 2, size.y + support_wall_thickness], corner_radius = 1);
            }
        }

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
                                [-1.25 * r, -r],
                                [-r, 0],
                                [r, 0],
                                [1.25 * r, -r]
                            ];
                        polygon(points);
                        circle(d = pipe_mount_outer_diameter);

                    }

                    union() {
                        circle(d = pipe_mount_inner_diameter);
                        translate([-pipe_mount_outer_diameter / 2, 0]) {
                            square([pipe_mount_outer_diameter, pipe_mount_outer_diameter / 2]);
                        }
                    }
                }
            }

            linear_extrude(width) {
                translate([-pipe_mount_inner_diameter / 2 - support_wall_thickness, -5]) {
                    square([support_wall_thickness, 10]);
                }
            }
        }
    }
}