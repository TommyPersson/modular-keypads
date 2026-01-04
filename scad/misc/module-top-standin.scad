use <../common/module-bottom-routines.scad>
use <../common/shared-utils.scad>
include <../common/shared-variables.scad>

module_top_standin(3);

module module_top_standin(width_units) {
    module_size = get_module_size(width_units);

    difference() {
        // body
        union() {
            linear_extrude(module_top_thickness) {
                rounded_rectangle(module_size, 1);
            }

            side_connector_fittings(module_size);
            back_connector_fitting(module_size);
        }

        union() {
            screw_hole_cutouts(module_size);
        }
    }
}

module screw_hole_cutouts(module_size) {
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

module side_connector_fittings(module_size) {
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

module back_connector_fitting(module_size) {
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