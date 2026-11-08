// Parts shared by all C-module sub-modules: the shell base and the shell bottom.

include <./module-c-common.scad>

sub_module_shell_bottom();

module sub_module_shell_base() {
    inner_inset = [sub_module_shell_wall_width, sub_module_shell_wall_width];

    difference() {
        union() {
            linear_extrude(sub_module_shell_top_thickness) {
                translate([sub_module_shell_size.x / 2, sub_module_shell_size.y / 2]) {
                    rounded_rectangle(sub_module_shell_size, corner_radius = 2, center = true);
                }
            }
        }

        union() {
            translate([inner_inset.x, inner_inset.y, sub_module_shell_top_thickness]) {
                for (p = sub_module_screw_holes) {
                    translate(p) {
                        threaded_insert_cutout(l = 3);
                    }
                }
            }

            union() {
                grip_length = 30;

                translate([0, sub_module_shell_size.y / 2 + grip_length / 2, sub_module_shell_top_thickness / 2]) {
                    rotate(90, [1, 0, 0]) {
                        linear_extrude(grip_length) {
                            rotate(45, [0, 0, 1]) {
                                square([2, 2], center = true);
                            }
                        }
                    }
                }

                translate([sub_module_shell_size.x, sub_module_shell_size.y / 2 + grip_length / 2,
                        sub_module_shell_top_thickness / 2])
                    {
                        rotate(90, [1, 0, 0]) {
                            linear_extrude(grip_length) {
                                rotate(45, [0, 0, 1]) {
                                    square([2, 2], center = true);
                                }
                            }
                        }
                    }
            }
        }
    }

    translate([0, 0, sub_module_shell_top_thickness]) {
        linear_extrude(sub_module_shell_wall_height) {
            difference() {
                rounded_rectangle(sub_module_shell_size, corner_radius = 2);

                cutout_size = [
                        sub_module_shell_size.x - outer_wall_thickness * 2,
                        sub_module_shell_size.y - outer_wall_thickness * 2,
                    ];

                inset = [outer_wall_thickness, outer_wall_thickness];

                translate(inset) {
                    rounded_rectangle(cutout_size, corner_radius = 1);
                }
            }
        }
    }
}

module sub_module_shell_bottom() {
    pcb_area_thickness = 0.5;

    difference() {
        union() {
            linear_extrude(sub_module_shell_bottom_thickness - 0.5) {
                rounded_rectangle(sub_module_pcb_size, corner_radius = 1);
            }
        }

        union() {
            translate([sub_module_pcb_size.x / 2, sub_module_pcb_size.y / 2]) {
                translate([0, 0, pcb_area_thickness]) {
                    linear_extrude(sub_module_shell_bottom_thickness - pcb_area_thickness) {
                        size = [
                                sub_module_pcb_size.x - 10.5,
                                sub_module_pcb_size.y - 4
                            ];
                        rounded_rectangle(size, corner_radius = 1, center = true);
                    }
                }

                translate([-sub_module_connector_offset, 0]) {
                    scale(1.05) sub_module_connector();
                }

                translate([sub_module_connector_offset, 0]) {
                    scale(1.05) sub_module_connector();
                }
            }

            translate([0, 0, 0])
                linear_extrude(screw_head_height) {
                    for (p = sub_module_screw_holes) {
                        translate(p) {
                            circle(d = screw_head_diameter);
                        }
                    }
                }

            sub_module_screw_hole_cutouts();
        }
    }
}
