include <./module-common-routines.scad>
include <./shared-variables.scad>
use <./shared-utils.scad>

module module_bottom_base(width_units) {
    module_size = get_module_size(width_units);

    intersection() {
        module_bottom_bounding_box(module_size);

        difference() {
            union() {
                module_bottom_base_base(module_size);
                module_bottom_base_walls(module_size);
                heat_inset_mounts(module_size);
            }

            union() {
                magnet_hole_cutouts(module_size);
                threaded_insert_cutouts(module_size);
                side_connector_cutouts(module_size);
                back_connector_cutouts(module_size);
                module_bottom_base_cutout(module_size);
            }
        }
    }
}

module module_bottom_base_base(module_size) {
    linear_extrude(module_bottom_base_thickness) {
        rounded_rectangle(module_size, 1);
    }
}

module module_bottom_base_walls(module_size) {
    translate([0, 0, module_bottom_base_thickness]) {
        difference() {
            linear_extrude(outer_wall_height) {
                rounded_rectangle(module_size, 1);
            }

            linear_extrude(outer_wall_height) {
                translate([outer_wall_thickness, outer_wall_thickness]) {
                    x = module_size.x - (outer_wall_thickness * 2);
                    y = module_size.y - (outer_wall_thickness * 2);
                    rounded_rectangle([x, y], 1);
                }
            }
        }

        difference() {
            linear_extrude(inner_wall_height) {
                translate([outer_wall_thickness, outer_wall_thickness]) {
                    x = module_size.x - (outer_wall_thickness * 2);
                    y = module_size.y - (outer_wall_thickness * 2);
                    rounded_rectangle([x, y], 1);
                }
            }

            linear_extrude(inner_wall_height) {
                translate([outer_wall_thickness + inner_wall_thickness, outer_wall_thickness + inner_wall_thickness]) {
                    x = module_size.x - (outer_wall_thickness * 2) - (inner_wall_thickness * 2);
                    y = module_size.y - (outer_wall_thickness * 2) - (inner_wall_thickness * 2);
                    rounded_rectangle([x, y], 1);
                }
            }
        }
    }
}

module module_bottom_base_cutout(module_size) {
    inset = [
        bottom_module_base_cutout_inset_x,
        bottom_module_base_cutout_inset_y
        ];

    linear_extrude(module_bottom_base_thickness + outer_wall_height) {
        translate(inset) {
            rounded_rectangle([
                    module_size.x - (inset.x * 2),
                    module_size.y - (inset.y * 2)
                ], 1);
        }
    }
}

module module_bottom_bounding_box(module_size) {
    linear_extrude(module_bottom_base_thickness + outer_wall_height) {
        rounded_rectangle(module_size, 1);
    }
}

module heat_inset_mounts(module_size) {
    points = get_module_bottom_screw_hole_points(module_size);

    translate([0, 0, module_bottom_base_thickness]) {
        for (p = points) {
            translate(p) {
                heat_inset_mount();
            }
        }
    }
}

module heat_inset_mount() {
    difference() {
        cylinder(h = inner_wall_height, d1 = threaded_insert_towers_diameter * 2, d2 = threaded_insert_towers_diameter);
    }
}

module threaded_insert_cutouts(module_size) {
    points = get_module_bottom_screw_hole_points(module_size);

    translate([0, 0, module_bottom_base_thickness]) {
        for (p = points) {
            translate(p) {
                threaded_insert_cutout();
            }
        }
    }
}

module threaded_insert_cutout() {
    k = threaded_insert_knurl_diameter;
    l = threaded_insert_length;

    k1 = k + 0.2;
    k2 = k * 0.9;

    l1 = l * 0.25;
    l2 = l + 1;

    translate([0, 0, inner_wall_height - l1]) {
        linear_extrude(l1) {
            circle(d = k1);
        }
    }

    translate([0, 0, inner_wall_height - l2]) {
        linear_extrude(l2) {
            circle(d = k2);
        }
    }
}

module side_connector_cutouts(module_size) {
    wall_diff = outer_wall_height - inner_wall_height;

    z = module_bottom_base_thickness + outer_wall_height - side_connector_height - wall_diff;
    y = module_size.y - side_connector_offset;

    translate([0, y, z]) {
        linear_extrude(side_connector_height + wall_diff) {
            square([module_size.x, side_connector_length]);
        }
    }
}

module back_connector_cutouts(module_size) {
    wall_diff = outer_wall_height - inner_wall_height;

    y = module_size.y - bottom_module_base_cutout_inset_y - 1;
    z = module_bottom_base_thickness + outer_wall_height - back_connector_height - wall_diff;

    translate([back_connector_offset, y, z]) {
        linear_extrude(back_connector_height + wall_diff) {
            width_x = module_size.x - (back_connector_offset * 2);
            depth_y = bottom_module_base_cutout_inset_y + 1;
            square([width_x, depth_y]);
        }
    }
}

function get_module_bottom_screw_hole_points(module_size) = [
        [screw_hole_inset, screw_hole_inset],
        [module_size.x - screw_hole_inset, screw_hole_inset],
        [screw_hole_inset, module_size.y / 2],
        [module_size.x - screw_hole_inset, module_size.y / 2],
        [screw_hole_inset, module_size.y - screw_hole_inset],
        [module_size.x - screw_hole_inset, module_size.y - screw_hole_inset]
    ];
