use <../common/module-bottom-routines.scad>
use <../common/module-top-routines.scad>
use <../common/shared-utils.scad>
include <./shared-variables.scad>

module_size = get_module_size(4);
bottom_pcb_size = get_pcb_size(4);
pcb_inset_xy = [outer_wall_thickness_base, outer_wall_thickness_base];

bottom_connector_inset = 1.5;
pcb_cover_thickness = bottom_connector_inset;

module_shell_bottom_height = 8;

sub_module_pcb_size = [68, 40];
sub_module_centers = [
        [38, 28],
        [38, 76],
        [38, 124]
    ];

sub_module_connector_offset = 31;
sub_module_connector_size = [4.3, 22.7, 4];
sub_module_separation = 8;
sub_module_shell_wall_width = 2;
sub_module_shell_wall_height = pcb_thickness + sub_module_connector_size.z + module_top_thickness -
    bottom_connector_inset;
sub_module_shell_top_thickness = module_top_cover_thickness;
sub_module_shell_bottom_thickness = sub_module_connector_size.z - bottom_connector_inset;
sub_module_shell_total_height = sub_module_shell_top_thickness + sub_module_shell_wall_height;

sub_module_shell_size = [
        sub_module_pcb_size.x + sub_module_shell_wall_width * 2,
        sub_module_pcb_size.y + sub_module_shell_wall_width * 2,
    ];

sub_module_screw_hole_inset = 3;
sub_module_screw_holes = [
        [sub_module_screw_hole_inset, sub_module_screw_hole_inset],
        [sub_module_screw_hole_inset, sub_module_pcb_size.y - sub_module_screw_hole_inset],
        [sub_module_pcb_size.x - sub_module_screw_hole_inset, sub_module_pcb_size.y - sub_module_screw_hole_inset],
        [sub_module_pcb_size.x - sub_module_screw_hole_inset, sub_module_screw_hole_inset]
    ];

not_quite_middle_y = sub_module_pcb_size.y * 2 + sub_module_separation * 3 - sub_module_separation / 4;

module_screw_hole_points = [
        [screw_hole_inset, screw_hole_inset],
        [module_size.x - screw_hole_inset, screw_hole_inset],
        [screw_hole_inset, not_quite_middle_y],
        [module_size.x - screw_hole_inset, not_quite_middle_y],
        [screw_hole_inset, module_size.y - screw_hole_inset],
        [module_size.x - screw_hole_inset, module_size.y - screw_hole_inset]
    ];

main_pcb_component_area_height_y = 37;

connector_pcb_standoff_thickness = sub_module_connector_size.z - pcb_thickness + bottom_connector_inset;
connector_pcb_size = [bottom_pcb_size.x, bottom_pcb_size.y - 40];

translate([-500, 0]) {
    connector_pcb();
}

translate([-400, 0]) {
    main_pcb();
}

translate([-300, 0]) {
    sub_module_pcb();
}

translate([-300, 120]) {
    sub_module_1_cover();
}

translate([-300, 180]) {
    sub_module_1_shell();
}

translate([-300, 240]) {
    sub_module_2_cover();
}

translate([-300, 300]) {
    sub_module_2_shell();
}

translate([-300, 60]) {
    sub_module_shell_bottom();
}

translate([-200, 0]) {
    connector_pcb_standoffs();
}

translate([-100, 0]) {
    pcb_cover();
}

translate([-100, 200]) {
    module_shell_top();
}

translate([-0, 0]) {
    module_shell_bottom();
}


module connector_pcb() {
    difference() {
        linear_extrude(pcb_thickness) {
            rounded_rectangle(connector_pcb_size, corner_radius = 1);
        }

        translate(-pcb_inset_xy) {
            module_screw_hole_cutouts(module_size);
        }
    }

    translate([0, 0, pcb_thickness])
        for (m = sub_module_centers) {
            translate(m) {
                translate([-sub_module_connector_offset, 0]) {
                    sub_module_connector();
                }

                translate([sub_module_connector_offset, 0]) {
                    sub_module_connector();
                }
            }
        }
}

module connector_pcb_standoffs() {
    size = [connector_pcb_size.x, connector_pcb_size.y - 10];

    difference() {
        linear_extrude(connector_pcb_standoff_thickness) {
            rounded_rectangle(size, corner_radius = 1);
        }

        union() {
            translate(-pcb_inset_xy) {
                module_screw_hole_cutouts(module_size);
            }

            for (m = sub_module_centers) {
                translate(m) {
                    translate([-sub_module_connector_offset, 0]) {
                        scale([1.05, 1.02])
                            sub_module_connector();
                    }

                    translate([sub_module_connector_offset, 0]) {
                        scale([1.05, 1.02])
                            sub_module_connector();
                    }
                }
            }

            linear_extrude(connector_pcb_standoff_thickness + 1) {
                translate([size.x / 4, 20]) {
                    square([size.x / 2, size.y - 20]);
                }
            }
        }
    }
}

module main_pcb() {
    difference() {
        union() {
            linear_extrude(pcb_thickness) {
                translate([0, bottom_pcb_size.y - 38]) {
                    rounded_rectangle([76, 38], corner_radius = 1);

                    translate([14, 0]) {
                        rounded_rectangle([48, 40], corner_radius = 1);
                    }
                }
            }

            // simulated component area
            translate([bottom_pcb_size.x / 2, bottom_pcb_size.y - 22, pcb_thickness]) {
                linear_extrude(3.5) {
                    square([70, 30], center = true);
                }
            }
        }

        union() {
            translate(-pcb_inset_xy) {
                module_screw_hole_cutouts(module_size);
            }

            #for (m = sub_module_centers) {
                translate(m) {
                    translate([-sub_module_connector_offset, 0]) {
                        scale(1.05)
                            sub_module_connector();
                    }

                    translate([sub_module_connector_offset, 0]) {
                        scale(1.05)
                            sub_module_connector();
                    }
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
                        scale([1.05, 1.02])
                            sub_module_connector();
                    }

                    translate([sub_module_connector_offset, 0]) {
                        scale([1.05, 1.02])
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
                            linear_extrude(base_shell_height) {
                                extra_size = [0.4, 0.4];
                                rounded_rectangle(sub_module_shell_size + extra_size, corner_radius = 2, center = true);
                            }

                            translate([-module_size.x / 2, 0, 14])
                                rotate(90, [0, 1, 0])
                                    linear_extrude(module_size.x) {
                                        circle(d = 17);
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

        #translate([module_size.x / 2, module_size.y]) {
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
                    translate([0, 0, 0]) {
                        linear_extrude(module_shell_bottom_height) {
                            size = [
                                    module_size.x - outer_wall_thickness * 2 - 20,
                                    module_size.y - outer_wall_thickness * 2 - 20
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

                #translate([outer_wall_thickness, outer_wall_thickness, module_shell_bottom_height]) {
                    translate([0, 0, -connector_pcb_standoff_thickness]) {
                        linear_extrude(module_shell_bottom_height) {
                            size = [connector_pcb_size.x, connector_pcb_size.y + 5];
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

module sub_module_pcb() {
    difference() {
        union() {
            linear_extrude(pcb_thickness) {
                translate([sub_module_pcb_size.x / 2, sub_module_pcb_size.y / 2]) {
                    rounded_rectangle(sub_module_pcb_size, corner_radius = 1, center = true);
                }
            }

            translate([sub_module_pcb_size.x / 2, sub_module_pcb_size.y / 2, pcb_thickness]) {
                translate([-sub_module_connector_offset, 0]) {
                    sub_module_connector();
                }

                translate([sub_module_connector_offset, 0]) {
                    sub_module_connector();
                }
            }
        }

        union() {
            sub_module_screw_hole_cutouts();
        }
    }
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


sub_module_2_switch_positions = [
        [u_1 * 1.5 / 2, -u_1 / 2],
        [u_1 * 1.5 / 2, u_1 / 2],
        [-u_1 * 1.5 / 2, -u_1 / 2],
        [-u_1 * 1.5 / 2, u_1 / 2],
    ];

module sub_module_2_cover() {
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
                for (p = sub_module_2_switch_positions) {
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

module sub_module_2_shell() {
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

            linear_extrude(sub_module_shell_top_thickness) {
                translate([sub_module_shell_size.x / 2, sub_module_shell_size.y / 2]) {
                    hull() {
                        for (p = sub_module_2_switch_positions) {
                            translate(p) {
                                rounded_rectangle([u_1 * 1.5, u_1], corner_radius = 2, center = true);
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
            linear_extrude(sub_module_shell_bottom_thickness) {
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

module sub_module_connector() {
    linear_extrude(sub_module_connector_size.z) {
        square([sub_module_connector_size.x, sub_module_connector_size.y], center = true);
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

module sub_module_screw_hole_cutouts() {
    linear_extrude(10) {
        for (p = sub_module_screw_holes) {
            translate(p) {
                circle(d = screw_hole_diameter);
            }
        }
    }
}