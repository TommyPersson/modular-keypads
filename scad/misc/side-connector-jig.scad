use <../common/module-top-routines.scad>
use <../common/shared-utils.scad>
include <../common/shared-variables.scad>

$fn = 50;

width_units = 4;
extra_wall_height = 4.25;
extra_wall_thickness = 7;
tolerance = 0.5;

base();

translate([0, -100]) {
    top();
}

module base() {
    module_size = get_module_size(width_units);


    difference() {
        union() {
            module_top_base(width_units);
            extra_walls(module_size);
        }

        union() {
            connector_cutouts(module_size);
            insert_mount_cutouts(module_size);
            extra_material_cutouts(module_size);
        }
    }
}

module extra_walls(module_size) {
    translate([0, 0]) {
        linear_extrude(module_top_thickness + pcb_thickness + extra_wall_height) {
            difference() {
                square([module_size.x, module_size.y]);

                translate([outer_wall_thickness, outer_wall_thickness]) {
                    square([module_size.x - outer_wall_thickness * 2, module_size.y - outer_wall_thickness * 2]);
                }
            }

            difference() {
                y = module_size.y - side_connector_offset - (tolerance / 2) - extra_wall_thickness;
                translate([-extra_wall_thickness, y]) {
                    w = module_size.x + extra_wall_thickness * 2;
                    h = side_connector_length + extra_wall_thickness * 2;
                    square([w, h]);
                }

                translate([outer_wall_thickness, outer_wall_thickness]) {
                    square([module_size.x - outer_wall_thickness * 2, module_size.y - outer_wall_thickness * 2]);
                }
            }
        }
    }
}

module connector_cutouts(module_size) {
    y = module_size.y - side_connector_offset - (tolerance / 2);

    translate([0, y, module_top_thickness + pcb_thickness]) {
        linear_extrude(pcb_thickness + 10) {
            square([module_size.x, side_connector_length]);
        }
    }
}

module extra_material_cutouts(module_size) {
    y1 = module_size.y - side_connector_offset - (tolerance / 2);
    y2 = module_size.y - side_connector_offset - (tolerance / 2) + side_connector_length + extra_wall_thickness;

    translate([0, 0, module_top_thickness + pcb_thickness]) {
        linear_extrude(extra_wall_height) {
            square([module_size.x, y1 - extra_wall_thickness]);

            translate([0, y2]) {
                square([module_size.x, module_size.y - y2]);
            }
        }
    }

    hole_inset = 10;

    #linear_extrude(module_top_thickness) {
        translate([hole_inset, hole_inset]) {
            square([module_size.x - hole_inset * 2, module_size.y - hole_inset * 2]);
        }
    }
}

module insert_mount_cutouts(module_size) {
    y = module_size.y - side_connector_offset + side_connector_length / 2;
    z = module_top_thickness + pcb_thickness + extra_wall_height;
    x1 = -extra_wall_thickness/2;
    x2 = module_size.x + extra_wall_thickness/2;

    translate([0, y, z]) {
        translate([x1, 0]) {
            threaded_insert_cutout();
        }
        translate([x2, 0]) {
            threaded_insert_cutout();
        }
    }
}

module top() {
    module_size = get_module_size(width_units);

    z0 = module_top_thickness + pcb_thickness + extra_wall_height;
    x0 = -extra_wall_thickness;

    linear_extrude(module_top_thickness + pcb_thickness) {
        translate([x0, 0, z0]) {
            w = module_size.x + extra_wall_thickness * 2;
            h = side_connector_length * 3;

            difference() {
                square([w, h]);

                union() {
                    translate([-x0 + outer_wall_thickness, 5]) {
                        square([module_size.x - outer_wall_thickness * 2, h - 10]);
                    }

                    union() { // screw holes
                        y = h/2;
                        x1 = -extra_wall_thickness/2 - x0;
                        x2 = module_size.x + extra_wall_thickness/2 - x0;

                        translate([x1, y]) {
                            circle(d = screw_hole_diameter);
                        }

                        translate([x2, y]) {
                            circle(d = screw_hole_diameter);
                        }
                    }
                }
            }
        }
    }
}
