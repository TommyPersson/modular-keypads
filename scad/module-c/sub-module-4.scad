// Sub-module 4: two "mouse" wheel encoders.

include <./module-c-common.scad>
use <./sub-module-common.scad>

sub_module_4_cover();

translate([0, 60]) {
    sub_module_4_shell();
}

module sub_module_4_shell() {
    difference() {
        sub_module_shell_base();

        translate([0, 0, -0.05]) {
            linear_extrude(sub_module_shell_top_thickness + 0.1) {
                translate([sub_module_shell_size.x / 2, sub_module_shell_size.y / 2]) {
                    square([57, 17], center = true);
                }
            }
        }

        translate([0, 0, sub_module_shell_top_thickness - 2.5]) {
            #linear_extrude(2.5) {
                translate([sub_module_shell_size.x / 2, sub_module_shell_size.y / 2]) {
                    translate([-24, 0]) {
                        square([10, 22], center = true);
                    }
                    translate([24, 0]) {
                        square([10, 22], center = true);
                    }
                }
            }
        }
    }
}

module sub_module_4_cover() {
    encoder_height = 9;
    encoder_cutout_size = [7.5 + 16.5, 16];
    encoder_cutout_locations = [[19, sub_module_pcb_size.y / 2], [49, sub_module_pcb_size.y / 2]];
    wheel_diameter = 16;
    holder_base_size = [6, wheel_diameter];
    stem_diameter = 2;
    holder_height = encoder_height;
    adjusted_stem_diameter = stem_diameter + 0.5;

    module center_fixture() {
        translate([sub_module_pcb_size.x / 2, sub_module_pcb_size.y / 2]) {
            difference() {
                union() {
                    linear_extrude(holder_height) {
                        square(holder_base_size, center = true);
                    }

                    translate([-holder_base_size.x / 2, 0, encoder_height])
                        rotate(90, [0, 1, 0])
                            linear_extrude(holder_base_size.x) {
                                circle(d = wheel_diameter);
                            }
                }

                union() {
                    rotate(90, [0, 0, 1])
                        translate([0, holder_base_size.y / 2, encoder_height]) {
                            rotate(90, [1, 0, 0]) {
                                linear_extrude(holder_base_size.y) {
                                    circle(d = adjusted_stem_diameter);
                                }

                                linear_extrude(holder_base_size.y) {
                                    translate([0, adjusted_stem_diameter / 4]) {
                                        rotate(45, [0, 0, 1])
                                            square([adjusted_stem_diameter, adjusted_stem_diameter], center = true);
                                    }
                                }
                            }
                        }
                }
            }
        }
    }

    module encoder_cover(side = "left") {
        width = 7;
        inset = side == "left" ? 1 : -1;

        module shape() {
            linear_extrude(encoder_height) {
                square([width, holder_base_size.y], center = true);
            }

            translate([-width / 2, 0, encoder_height]) {
                rotate(90, [0, 0, 1])
                    rotate(90, [1, 0, 0])
                        linear_extrude(width) {
                            circle(d = wheel_diameter);
                        }
            }
        }

        difference() {
            union() {
                shape();
                #linear_extrude(4) {
                    square([width, holder_base_size.y + 4], center = true);
                }
            }

            translate([inset, 0]) {
                scale(0.8) {
                    shape();
                }
            }
        }
    }

    difference() {
        union() {
            linear_extrude(module_top_thickness) {
                translate([sub_module_pcb_size.x / 2, sub_module_pcb_size.y / 2]) {
                    rounded_rectangle(sub_module_pcb_size, corner_radius = 1, center = true);
                }
            }

            center_fixture();

            translate([9.5, sub_module_pcb_size.y / 2]) {
                encoder_cover(side = "left");
            }

            translate([sub_module_pcb_size.x - 9.5, sub_module_pcb_size.y / 2]) {
                encoder_cover(side = "right");
            }
        }

        union() {
            sub_module_screw_hole_cutouts();

            translate([0, 0, -0.1])
                linear_extrude(module_top_thickness + 0.2) {
                    for (p = encoder_cutout_locations) {
                        translate(p) {
                            square(encoder_cutout_size, center = true);
                        }
                    }
                }

            translate([sub_module_pcb_size.x / 2, sub_module_pcb_size.y / 2]) {
                translate([-sub_module_connector_offset, 0]) {
                    scale(0.8) sub_module_connector();
                }

                translate([sub_module_connector_offset, 0]) {
                    scale(0.8) sub_module_connector();
                }
            }

            // center split
            linear_extrude(100) {
                translate([sub_module_pcb_size.x / 2, sub_module_pcb_size.y / 2]) {
                    square([sub_module_pcb_size.x + 1, 0.4], center = true);
                }
            }
        }
    }
}
