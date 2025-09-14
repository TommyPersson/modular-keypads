include <./module-common-routines.scad>
include <./shared-variables.scad>
use <./shared-utils.scad>

module module_top_base(width_units) {
    module_size = get_module_size(width_units);

    difference() {
        // body
        union() {
            linear_extrude(module_top_thickness) {
                rounded_rectangle(module_size, 1);
            }

            module_top_side_connector_fittings(module_size);
            module_top_back_connector_fitting(module_size);
        }

        union() {
            module_top_screw_hole_cutouts(module_size);
            #module_top_side_connector_solder_joint_cutouts(module_size);
        }
    }
}

module module_top_screw_hole_cutouts(module_size) {
    inset = screw_hole_inset;

    points = [
            [inset, inset],
            [module_size.x - inset, inset],
            [inset, module_size.y / 2],
            [module_size.x - inset, module_size.y / 2],
            [inset, module_size.y - inset],
            [module_size.x - inset, module_size.y - inset]
        ];

    for (p = points) {
        translate(p) {
            linear_extrude(module_top_thickness) {
                circle(d = screw_hole_diameter);
            }
        }
    }
}

module module_top_side_connector_fittings(module_size) {
    tolerance = 0.5;

    w = outer_wall_thickness;
    x1 = 0;
    x2 = module_size.x - w;
    y = module_size.y - side_connector_offset - (tolerance/2);

    translate([x1, y, module_top_thickness]) {
        linear_extrude(pcb_thickness) {
            square([w, side_connector_length]);
        }
    }

    translate([x2, y, module_top_thickness]) {
        linear_extrude(pcb_thickness) {
            square([w, side_connector_length]);
        }
    }
}

module module_top_side_connector_solder_joint_cutouts(module_size) {
    solder_joint_height = 0.5;

    w = 6;
    x1 = outer_wall_thickness;
    x2 = module_size.x - w - outer_wall_thickness;
    y = module_size.y - side_connector_offset;
    z = module_top_thickness - solder_joint_height;

    translate([x1, y, z]) {
        linear_extrude(solder_joint_height) {
            square([w, side_connector_length]);
        }
    }

    translate([x2, y, z]) {
        linear_extrude(pcb_thickness) {
            square([w, side_connector_length]);
        }
    }
}

module module_top_back_connector_fitting(module_size) {
    tolerance = 0.5;

    x = back_connector_offset + tolerance/2;
    w = module_size.x - (x * 2) - tolerance;
    y = module_size.y - outer_wall_thickness;

    translate([x, y, module_top_thickness]) {
        linear_extrude(pcb_thickness) {
            square([w, outer_wall_thickness]);
        }
    }
}

module module_top_key_switch_cutouts(points) {
    for (p = points) {
        translate(p) {
            module_top_key_switch_cutout();
        }
    }
}

module module_top_key_switch_cutout() {
    bottom_offset = module_top_thickness - switch_neck_thickness;

    rotate(180, [1, 0, 0]) {
        translate([0, 0, -module_top_thickness]) {
            linear_extrude(bottom_offset) {
                rounded_rectangle([switch_hole_width, switch_hole_width], 1, center = true);
            }

            translate([0, 0, bottom_offset]) {
                linear_extrude(switch_neck_thickness) {
                    square([switch_neck_width, switch_neck_width], center = true);

                    xy = (switch_neck_width / 2) - 0.5;
                    points = [[-xy, -xy], [xy, -xy], [-xy, xy], [xy, xy]];
                    for (p = points) {
                        translate([p.x, p.y]) {
                            circle(r = 1);
                        }
                    }
                }
            }
        }
    }
}