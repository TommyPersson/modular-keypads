// Sub-module 1: six 1U key switches in a 3x2 grid.

include <./module-c-common.scad>
use <./sub-module-common.scad>

sub_module_1_cover();

translate([0, 60]) {
    sub_module_1_shell();
}

sub_module_1_switch_positions = [
        [0, -u_1 / 2],
        [0, u_1 / 2],
        [-u_1, -u_1 / 2],
        [-u_1, u_1 / 2],
        [u_1, -u_1 / 2],
        [u_1, u_1 / 2]
    ];

module sub_module_1_cover() {
    difference() {
        union() {
            linear_extrude(module_top_thickness) {
                translate([sub_module_pcb_size.x / 2, sub_module_pcb_size.y / 2]) {
                    rounded_rectangle(sub_module_pcb_size, corner_radius = 1, center = true);
                }
            }
        }

        union() {
            sub_module_screw_hole_cutouts();

            translate([sub_module_pcb_size.x / 2, sub_module_pcb_size.y / 2]) {
                for (p = sub_module_1_switch_positions) {
                    translate(p) {
                        module_top_key_switch_cutout();
                    }
                }

                translate([-sub_module_connector_offset, 0]) {
                    scale(0.8) sub_module_connector();
                }

                translate([sub_module_connector_offset, 0]) {
                    scale(0.8) sub_module_connector();
                }
            }
        }
    }
}

module sub_module_1_shell() {
    difference() {
        sub_module_shell_base();

        linear_extrude(sub_module_shell_top_thickness) {
            translate([sub_module_shell_size.x / 2, sub_module_shell_size.y / 2]) {
                hull() {
                    for (p = sub_module_1_switch_positions) {
                        translate(p) {
                            rounded_rectangle([u_1, u_1], corner_radius = 2, center = true);
                        }
                    }
                }
            }
        }
    }
}
