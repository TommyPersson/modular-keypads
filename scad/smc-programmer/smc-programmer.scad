use <../common/shared-utils.scad>
include <../common/shared-variables.scad>

$fn = 50;

pcb_size = [127, 55];
pcb_screw_positions = [
        [2.54, 2.54],
        [2.54, 52.46],
        [124.46, 2.54],
        [124.46, 52.46],
        [117.18, 10.48],
        [117.18, 22.08],
        [121.16, 28.01],
        [121.16, 41.73],
    ];
pcb_component_height = 2;

pcb_headers_v = [
        [81.58, 17, 4],
        [100.33, 37.66, 3],
    ];

pcb_headers_h = [
        [34.29, 2.1, 6],
        [52.07, 2.1, 6],
        [34.29, 52.9, 6],
        [52.07, 52.9, 6],
    ];


case_wall_width = 2;

case_size = [pcb_size.x + case_wall_width * 2, pcb_size.y + case_wall_width * 2];
case_bottom_height = 8;
case_bottom_thickness = 2;
case_top_height = 8.4;

pin_header_casing_height = 2.5;

translate([0, 200]) {
    case_bottom();
}

translate([0, 100]) {
    case_top();
}

translate([0, 0]) {
    top_board_spacer();
}

module top_board_spacer() {
    intersection() {
        difference() {
            linear_extrude(pin_header_casing_height) {
                rounded_rectangle(case_size, corner_radius = 2, center = true);
            }

            // screw holes
            translate([0, 0, -0.01]) {
                for (point = pcb_screw_positions) {
                    translate([point.x - pcb_size.x / 2, pcb_size.y / 2 - point.y]) {
                        linear_extrude(case_top_height + 0.02) {
                            circle(d = screw_hole_diameter);
                        }

                        translate([0, 0, case_top_height - screw_head_height]) {
                            linear_extrude(screw_head_height + 0.02) {
                                circle(d = screw_head_diameter);
                            }
                        }
                    }
                }
            }
        }

        #linear_extrude(pin_header_casing_height) {
            offset = 75;
            translate([offset, 0]) {
                size = [case_size.x - offset, case_size.y];
                rounded_rectangle(size, corner_radius = 2, center = true);
            }
        }
    }
}

module case_top() {
    difference() {
        linear_extrude(case_top_height) {
            rounded_rectangle(case_size, corner_radius = 2, center = true);
        }

        union() {
            // module fitting
            translate([0, 0, -0.1]) {
                module_size = [73, 45];
                module_center = 39.08; // from pcb edge

                translate([-pcb_size.x / 2 + module_center, 0, 0]) {
                    cutout_size = module_size;
                    linear_extrude(case_top_height + 0.2) {
                        rounded_rectangle(cutout_size, corner_radius = 2, center = true);
                    }
                }
            }

            // screw holes
            translate([0, 0, -0.01]) {
                for (point = pcb_screw_positions) {
                    translate([point.x - pcb_size.x / 2, pcb_size.y / 2 - point.y]) {
                        linear_extrude(case_top_height + 0.02) {
                            circle(d = screw_hole_diameter);
                        }

                        translate([0, 0, case_top_height - screw_head_height]) {
                            linear_extrude(screw_head_height + 0.02) {
                                circle(d = screw_head_diameter);
                            }
                        }
                    }
                }
            }

            // pcb clearance
            linear_extrude(pcb_component_height) {
                size = [pcb_size.x - case_wall_width * 2, pcb_size.y - case_wall_width * 2];
                rounded_rectangle(size, corner_radius = 2, center = true);
            }

            // vertical pin headers
            translate([0, 0, 0.01]) {
                linear_extrude(case_top_height + 0.02) {
                    for (point = pcb_headers_v) {
                        x = point.x - pcb_size.x / 2;
                        y = pcb_size.y / 2 - point.y;
                        w = 2.54 + 1;
                        h = point.z * 2.54 + 3;

                        translate([x, y]) {
                            square([w, h], center = true);
                        }
                    }
                }
            }

            // horizontal pin headers
            translate([0, 0, -0.01]) {
                linear_extrude(case_top_height + 0.02) {
                    for (point = pcb_headers_h) {
                        x = point.x - pcb_size.x / 2;
                        y = pcb_size.y / 2 - point.y;
                        h = 2.54 + 1;
                        w = point.z * 2.54 + 3;

                        translate([x, y]) {
                            square([w, h], center = true);
                        }
                    }
                }
            }
        }
    }
}

module case_bottom() {
    intersection() {
        // bounding box
        linear_extrude(case_bottom_height) {
            rounded_rectangle(case_size, corner_radius = 2, center = true);
        }

        difference() {
            union() {
                difference() {
                    linear_extrude(case_bottom_height) {
                        rounded_rectangle(case_size, corner_radius = 2, center = true);
                    }

                    union() {
                        // top cutout
                        translate([0, 0, case_bottom_height - pcb_thickness]) {
                            cutout_size = [pcb_size.x + wall_tolerance, pcb_size.y + wall_tolerance];
                            linear_extrude(pcb_thickness + 0.1) {
                                rounded_rectangle(cutout_size, corner_radius = 2, center = true);
                            }
                        }

                        // inner cutout
                        translate([0, 0, case_bottom_thickness]) {
                            cutout_size = [case_size.x - case_wall_width * 4, case_size.y - case_wall_width * 4];
                            linear_extrude(case_bottom_height - case_bottom_thickness) {
                                rounded_rectangle(cutout_size, corner_radius = 2, center = true);
                            }
                        }

                        // module connector cutouts
                        translate([0, 0, case_bottom_thickness]) {
                            connector_width = 35;
                            connector_center = 43.2; // from pcb edge

                            translate([-pcb_size.x / 2 + connector_center, 0, 0]) {
                                cutout_size = [connector_width, pcb_size.y];
                                linear_extrude(case_bottom_height - case_bottom_thickness) {
                                    rounded_rectangle(cutout_size, corner_radius = 1, center = true);
                                }
                            }
                        }
                    }
                }

                // heat inset mounts
                translate([0, 0, case_bottom_thickness]) {
                    for (point = pcb_screw_positions) {
                        translate([point.x - pcb_size.x / 2, pcb_size.y / 2 - point.y]) {
                            h = case_bottom_height - case_bottom_thickness - pcb_thickness;
                            d1 = 10;
                            d2 = 5;
                            cylinder(h = h, d1 = d1, d2 = d2);
                        }
                    }
                }
            }

            // heat inset cutouts
            translate([0, 0, case_bottom_thickness]) {
                for (point = pcb_screw_positions) {
                    translate([point.x - pcb_size.x / 2, pcb_size.y / 2 - point.y]) {
                        h = case_bottom_height - case_bottom_thickness - pcb_thickness;
                        translate([0, 0, h]) {
                            #threaded_insert_cutout();
                        }
                    }
                }
            }

            // module fitting cutout
            translate([0, 0, -0.1]) {
                module_size = [71, 43];
                module_center = 39.08; // from pcb edge

                translate([-pcb_size.x / 2 + module_center, 0, 0]) {
                    cutout_size = module_size;
                    linear_extrude(case_bottom_height) {
                        rounded_rectangle(cutout_size, corner_radius = 2, center = true);
                    }
                }
            }
        }
    }
}
