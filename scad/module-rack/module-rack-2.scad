use <../common/module-common-routines.scad>
use <../common/shared-utils.scad>
include <../common/shared-variables.scad>

rack_top_thickness = 4;
rack_lip_width = 2;
rack_lip_height = module_total_height;
rack_base_height = 20;
rack_angle = 10;

// TODO many of the local calculations only "work" with the current 'rack_angle', presumably some better use of basic trigonometry will work.

module_rack(4);

module module_rack(width_units) {
    module_size = get_module_size(width_units);

    rack_size = [module_size.x, module_size.y + rack_lip_width];

    translate([0, 0, rack_base_height]) {
        rotate(rack_angle, [1, 0, 0]) {
            rack_top(width_units, rack_size);
        }
    }

    rack_base(width_units, rack_size);
}

module rack_top(width_units, rack_size) {
    difference() {
        union() {
            linear_extrude(rack_top_thickness) {
                square(rack_size);
            }

            linear_extrude(module_total_height) {
                translate([0, 0, rack_top_thickness]) {
                    square([rack_size.x, rack_lip_width]);
                }
            }
        }

        union() {
            rack_top_magnet_hole_cutouts(width_units);
            rack_top_cutout(rack_size);
        }
    }
}

module rack_top_magnet_hole_cutouts(width_units) {
    num_magnets = width_units;

    top_y_offset = module_height_y - magnet_inset + rack_lip_width;
    bottom_y_offset = magnet_inset + rack_lip_width;

    points = [for (i = [0 : 3]) each [
            [(module_width_x_unit * i) + (module_width_x_unit / 2), top_y_offset],
            [(module_width_x_unit * i) + (module_width_x_unit / 2), bottom_y_offset]
        ]
        ];

    translate([0, 0, rack_top_thickness - magnet_depth]) {
        for (p = points) {
            translate(p) {
                magnet_hole_cutout();
            }
        }
    }
}

module rack_top_cutout(rack_size) {
    linear_extrude(rack_top_thickness) {
        offset = [0, magnet_inset * 2 + rack_lip_width];
        translate(offset) {
            square([rack_size.x, rack_size.y - magnet_inset * 4 - rack_lip_width]);
        }
    }
}

module rack_base(width_units, rack_size) {
    difference() {
        rack_base_hull(rack_size);

        union() {
            rack_base_cutout(rack_size);
            side_magnet_cutouts(rack_size);
        }
    }
}

module rack_base_hull(rack_size) {
    // polygon shape extruded along x-axis

    c = rack_size.y;
    a = cos(rack_angle) * c;
    b = sin(rack_angle) * c;

    points = [
            [0, 0],
            [0, rack_base_height],
            [a, b + rack_base_height],
            [a, 0],
        ];

    rotate(90, [1, 0, 0]) {
        rotate(90, [0, 1, 0]) {
            linear_extrude(rack_size.x) {
                polygon(points);
            }
        }
    }
}

module rack_base_cutout(rack_size) {
    near_offset_y = (rack_lip_width + (magnet_inset * 2)) * cos(rack_angle);
    length_y = (rack_size.y - magnet_inset * 4 - rack_lip_width) * cos(rack_angle);

    translate([0, 0, 10]) {
        linear_extrude(1000) {
            translate([0, near_offset_y]) {
                square([rack_size.x, length_y]);
            }
        }
    }
}

module side_magnet_cutouts(rack_size) {
    far_offset_y = (rack_size.y - magnet_inset) * cos(rack_angle);

    // near left
    translate([0, magnet_inset, magnet_inset])
        rotate(90, [0, 0, 1])
            rotate(90, [1, 0, 0])
                #linear_extrude(magnet_depth) {
                    tear_drop_circle(d = magnet_diameter);
                }

    // far left
    translate([0, far_offset_y, magnet_inset])
        rotate(90, [0, 0, 1])
            rotate(90, [1, 0, 0])
                #linear_extrude(magnet_depth) {
                    tear_drop_circle(d = magnet_diameter);
                }

    // near right
    translate([rack_size.x - magnet_depth, magnet_inset, magnet_inset])
        rotate(90, [0, 0, 1])
            rotate(90, [1, 0, 0])
                #linear_extrude(magnet_depth) {
                    tear_drop_circle(d = magnet_diameter);
                }

    // far right
    translate([rack_size.x - magnet_depth, far_offset_y, magnet_inset])
        rotate(90, [0, 0, 1])
            rotate(90, [1, 0, 0])
                #linear_extrude(magnet_depth) {
                    tear_drop_circle(d = magnet_diameter);
                }

}