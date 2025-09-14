use <../common/module-bottom-routines.scad>
use <../common/shared-utils.scad>
include <../common/shared-variables.scad>

pcb_standin(3);

module pcb_standin(width_units) {
    pcb_size = get_pcb_size(width_units);

    difference() {
        // body
        linear_extrude(pcb_thickness) {
            rounded_rectangle(pcb_size, 1);
        }

        union() {
            screw_hole_cutouts(pcb_size);
            side_connector_cutouts(pcb_size);
        }
    }
}

module screw_hole_cutouts(pcb_size) {
    inset = screw_hole_inset_pcb;

    points = [
            [inset, inset],
            [pcb_size.x - inset, inset],
            [inset, pcb_size.y / 2],
            [pcb_size.x - inset, pcb_size.y / 2],
            [inset, pcb_size.y - inset],
            [pcb_size.x - inset, pcb_size.y - inset]
        ];

    for (p = points) {
        translate(p) {
            linear_extrude(pcb_thickness) {
                circle(d = screw_hole_diameter);
            }
        }
    }
}

module side_connector_cutouts(pcb_size) {
    w = 3;
    x1 = 2;
    x2 = pcb_size.x - x1 - w;
    y = pcb_size.y - side_connector_offset + outer_wall_thickness_base;

    translate([x1, y]) {
        linear_extrude(pcb_thickness) {
            rounded_rectangle([w, side_connector_length], 1);
        }
    }

    translate([x2, y]) {
        linear_extrude(pcb_thickness) {
            rounded_rectangle([w, side_connector_length], 1);
        }
    }
}