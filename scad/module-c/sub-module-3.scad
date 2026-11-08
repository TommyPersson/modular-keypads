// Sub-module 3: single large rotary encoder.

include <./module-c-common.scad>
use <./sub-module-common.scad>

sub_module_3_cover();

translate([0, 60]) {
    sub_module_3_shell();
}

sub_module_3_encoder_diameter = 37.5;

module sub_module_3_cover() {
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
                linear_extrude(module_top_thickness) {
                    circle(d = sub_module_3_encoder_diameter);
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

module sub_module_3_shell() {
    difference() {
        sub_module_shell_base();

        translate([0, 0, 2]) {
            linear_extrude(sub_module_shell_top_thickness) {

                translate([sub_module_shell_size.x / 2, sub_module_shell_size.y / 2]) {
                    circle(d = sub_module_3_encoder_diameter);
                }
            }
        }

        linear_extrude(sub_module_shell_top_thickness - 2) {
            translate([sub_module_shell_size.x / 2, sub_module_shell_size.y / 2]) {
                scale(0.8) {
                    circle(d = sub_module_3_encoder_diameter);
                }
            }
        }
    }
}
