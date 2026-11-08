// C-module base: shell top/bottom, PCB cover, connector PCB standoffs and solder jig.

include <./module-c-common.scad>

translate([-200, 0]) {
    connector_pcb_standoffs();
}

translate([-100, 0]) {
    pcb_cover();
}

translate([-100, 200]) {
    module_shell_top();
}

translate([0, 0]) {
    module_shell_bottom();
}

translate([0, 200]) {
    connector_pcb_solder_jig();
}

module connector_pcb_solder_jig() {
    size = [connector_pcb_size.x, connector_pcb_size.y + 20];

    difference() {
        linear_extrude(2) {
            rounded_rectangle(size, corner_radius = 1);
        }

        union() {
            translate(-pcb_inset_xy) {
                module_screw_hole_cutouts(module_size);
            }

            linear_extrude(connector_pcb_standoff_thickness + 1) {
                translate([12, 10]) {
                    square([size.x - 24, size.y - 20]);
                }
            }


            for (m = sub_module_centers) {
                translate(m) {
                    translate([-sub_module_connector_offset, 0]) {
                        translate([0, 0, 1]) {
                            scale([0.6, 0.8])
                                sub_module_connector();
                        }
                    }

                    translate([sub_module_connector_offset, 0]) {
                        translate([0, 0, 1]) {
                            scale([0.6, 0.8])
                                sub_module_connector();
                        }
                    }
                }
            }
        }
    }

    translate([0, 0, 2]) {
        difference() {
            linear_extrude(sub_module_connector_size.z) {
                rounded_rectangle(size, corner_radius = 1);
            }

            union() {
                translate(-pcb_inset_xy) {
                    module_screw_hole_cutouts(module_size);
                }

                for (m = sub_module_centers) {
                    translate(m) {
                        translate([-sub_module_connector_offset, 0]) {
                            scale([1.02, 1.00])
                                sub_module_connector();
                            translate([3, 0])
                                scale([1, 0.7])
                                    sub_module_connector();
                        }

                        translate([sub_module_connector_offset, 0]) {
                            scale([1.02, 1.00])
                                sub_module_connector();
                            translate([-3, 0])
                                scale([1, 0.7])
                                    sub_module_connector();
                        }
                    }
                }

                linear_extrude(sub_module_connector_size.z + 1) {
                    translate([12, 10]) {
                        square([size.x - 24, size.y - 20]);
                    }
                }
            }
        }
    }
}

module connector_pcb_standoffs() {
    size = [connector_pcb_size.x, connector_pcb_size.y - 0];

    difference() {
        union() {
            linear_extrude(connector_pcb_standoff_thickness) {
                rounded_rectangle(size, corner_radius = 1);
            }
        }

        union() {
            translate(-pcb_inset_xy) {
                module_screw_hole_cutouts(module_size);
            }

            for (m = sub_module_centers) {
                translate(m) {
                    translate([-sub_module_connector_offset, 0]) {
                        scale([1.20, 1.02])
                            sub_module_connector();

                        linear_extrude(connector_pcb_standoff_thickness + 0.1) {
                            component_area_width = 15;
                            translate([sub_module_connector_size.x / 2 + component_area_width / 2, 0])
                                square([component_area_width, sub_module_connector_size.y + 2], center = true);
                        }
                    }

                    translate([sub_module_connector_offset, 0]) {
                        scale([1.20, 1.02])
                            sub_module_connector();

                        linear_extrude(connector_pcb_standoff_thickness + 0.1) {
                            component_area_width = 15;
                            translate([-sub_module_connector_size.x / 2 - component_area_width / 2, 0])
                                square([component_area_width, sub_module_connector_size.y + 2], center = true);
                        }
                    }
                }
            }

            linear_extrude(connector_pcb_standoff_thickness + 1) {
                connector_components_area = [28, 28 + 24];
                translate([42, connector_pcb_size.y - 26]) {
                    square(connector_components_area, center = true);
                }
            }
        }
    }
}

module pcb_cover() {
    difference() {
        union() {
            linear_extrude(pcb_cover_thickness) {
                rounded_rectangle(bottom_pcb_size, corner_radius = 1);
            }
        }

        union() {
            translate(-pcb_inset_xy) {
                module_screw_hole_cutouts(module_size);
            }

            for (m = sub_module_centers) {
                translate(m) {
                    translate([-sub_module_connector_offset, 0]) {
                        scale([1.20, 1.02])
                            sub_module_connector();
                    }

                    translate([sub_module_connector_offset, 0]) {
                        scale([1.20, 1.02])
                            sub_module_connector();
                    }
                }
            }
        }
    }
}

module module_shell_top() {
    base_shell_height = sub_module_shell_total_height;
    skirt_height = pcb_cover_thickness + pcb_thickness;

    translate([0, 0, skirt_height])
        difference() {
            union() {
                linear_extrude(base_shell_height) {
                    translate([module_size.x / 2, module_size.y / 2]) {
                        rounded_rectangle(module_size, corner_radius = 1, center = true);
                    }
                }
            }

            union() {
                translate([0, 0, 0]) {
                    for (p = module_screw_hole_points) {
                        translate(p) {
                            rotate(180, [0, 1, 0]) {
                                threaded_insert_cutout(l = 3);
                            }
                        }
                    }
                }

                translate(pcb_inset_xy) {
                    for (m = sub_module_centers) {
                        translate(m) {
                            linear_extrude(base_shell_height + 0.1) {
                                extra_size = [0.4, 0.4];
                                rounded_rectangle(sub_module_shell_size + extra_size, corner_radius = 2, center = true);
                            }

                            translate([0, 0, base_shell_height]) {
                                fillet_groove(sub_module_shell_size);
                                fillet_groove([module_size.x + 10, 22]);
                            }

                            translate([-module_size.x / 2 - 0.1, 0, 14])
                                rotate(90, [0, 1, 0])
                                    linear_extrude(module_size.x + 0.2) {
                                        rounded_rectangle([22, 22], corner_radius = 2, center = true);
                                    }
                        }
                    }
                }
            }
        }

    difference() {
        linear_extrude(skirt_height) {
            translate([module_size.x / 2, module_size.y / 2]) {
                skirt_shape();
            }
        }

        translate([module_size.x / 2, module_size.y]) {
            linear_extrude(pcb_thickness) {
                square([48 + wall_tolerance, outer_wall_thickness_base * 2], center = true);
            }
        }
    }
}

module module_shell_bottom() {
    base_thickness = 3.5;
    connector_pcb_component_height = 2;

    translate([0, 0, 0])
        difference() {
            union() {
                linear_extrude(module_shell_bottom_height) {
                    translate([module_size.x / 2, module_size.y / 2]) {
                        rounded_rectangle(module_size, corner_radius = 1, center = true);
                    }
                }
            }

            union() {
                module_shell_bottom_screw_holes();
                magnet_hole_cutouts(module_size);
                module_shell_bottom_side_connector_cutouts();
                module_shell_bottom_io_panel_cutout();

                translate([module_size.x / 2, module_size.y / 2]) {
                    translate([0, 0, -0.1]) {
                        linear_extrude(module_shell_bottom_height + 0.1) {
                            size = [
                                        module_size.x - outer_wall_thickness * 2 - 20,
                                        module_size.y - outer_wall_thickness * 2 - 20
                                ];
                            rounded_rectangle(size, corner_radius = 1, center = true);
                        }
                    }

                    translate([0, 0, 2]) {
                        linear_extrude(module_shell_bottom_height + 0.1) {
                            size = [
                                        module_size.x - outer_wall_thickness * 2 - 10,
                                        module_size.y - outer_wall_thickness * 2 - 15
                                ];
                            rounded_rectangle(size, corner_radius = 1, center = true);
                        }
                    }

                    translate([0, 0, base_thickness]) {
                        linear_extrude(module_shell_bottom_height) {
                            size = [
                                        module_size.x - (outer_wall_thickness * 2) - (inner_wall_thickness * 2),
                                        module_size.y - outer_wall_thickness * 2 - inner_wall_thickness * 2
                                ];
                            rounded_rectangle(size, corner_radius = 1, center = true);
                        }
                    }
                }

                #translate([outer_wall_thickness - 0.25, outer_wall_thickness - 0.25, module_shell_bottom_height]) {
                    translate([0, 0, -connector_pcb_standoff_thickness]) {
                        linear_extrude(module_shell_bottom_height) {
                            size = [connector_pcb_size.x + 0.5, connector_pcb_size.y + 5.25];
                            rounded_rectangle(size, corner_radius = 1);
                        }
                    }
                }
            }
        }
}

module module_shell_bottom_screw_holes() {
    for (p = module_screw_hole_points) {
        translate(p) {
            translate([0, 0, 0]) {
                linear_extrude(screw_head_height) {
                    circle(d = screw_head_diameter);
                }
            }

            linear_extrude(module_shell_bottom_height) {
                circle(d = screw_hole_diameter);
            }
        }
    }
}

module module_shell_bottom_side_connector_cutouts() {
    offset = [
        0,
            module_size.y - side_connector_offset,
            module_shell_bottom_height - side_connector_height
        ];

    translate(offset) {
        linear_extrude(side_connector_height) {
            square([module_size.x, side_connector_length]);
        }
    }
}

module module_shell_bottom_io_panel_cutout() {
    panel_width = 48 + wall_tolerance;

    offset = [
            module_size.x / 2,
        module_size.y,
            module_shell_bottom_height - side_connector_height
        ];

    translate(offset) {
        linear_extrude(side_connector_height) {
            square([panel_width, 40], center = true);
        }
    }
}


module skirt_shape() {
    difference() {
        rounded_rectangle(module_size, corner_radius = 1, center = true);

        cutout_size = [module_size.x - outer_wall_thickness * 2, module_size.y - outer_wall_thickness * 2];
        rounded_rectangle(cutout_size, corner_radius = 1, center = true);
    }
}

module module_screw_head_cutouts(module_size) {
    for (p = module_screw_hole_points) {
        translate(p) {
            linear_extrude(screw_head_height) {
                circle(d = screw_head_diameter);
            }
        }
    }
}

module module_screw_hole_cutouts(module_size) {
    inset = screw_hole_inset;

    not_quite_middle_y = sub_module_pcb_size.y * 2 + sub_module_separation * 3 - sub_module_separation / 4;

    points = [
            [inset, inset],
            [module_size.x - inset, inset],
            [inset, not_quite_middle_y],
            [module_size.x - inset, not_quite_middle_y],
            [inset, module_size.y - inset],
            [module_size.x - inset, module_size.y - inset]
        ];

    for (p = points) {
        translate(p) {
            linear_extrude(100) {
                circle(d = screw_hole_diameter);
            }
        }
    }
}

module fillet_groove(size) {
    fillet_points = [[-1, -1], [-1, 1], [1, -1], [1, 1]];
    hull() {
        for (p = fillet_points) {
            translate([p.x * size.x / 2, p.y * size.y / 2, 0]) {
                translate([p.x * -1, p.y * -1, -1]) {
                    cylinder(h = 2, r1 = 0, r2 = 2, center = true);
                }
            }
        }
    }
}
