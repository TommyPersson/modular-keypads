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
        rack_base_cutout(rack_size);
        rack_legs_cutouts(rack_size);
    }

    rack_base_beams(width_units, rack_size);
    rack_legs_chamfers(rack_size);
}

module rack_base_cutout(rack_size) {
    #linear_extrude(rack_base_height + sin(rack_angle) * rack_size.y) {
        lip_offset = cos(rack_angle) * rack_lip_width;
        inset_offset = cos(rack_angle) * (magnet_inset * 2);
        offset = [0, inset_offset + lip_offset];
        translate(offset) {
            square([rack_size.x, rack_size.y - (offset.y * 2) - lip_offset]);
        }
    }
}

module rack_base_beams(width_units, rack_size) {
    beam_width = 6;
    beam_height = 3;
    beam_length = cos(rack_angle) * rack_size.y;

    for (i = [0 : width_units - 1]) {
        offset = [(module_width_x_unit * i) + (module_width_x_unit / 2) - beam_width / 2, 0];
        translate(offset) {
            linear_extrude(beam_height) {
                square([beam_width, beam_length]);
            }

            translate([beam_width / 2 - beam_width / 4, 0, beam_height]) {
                linear_extrude(beam_height) {
                    square([beam_width / 2, beam_length]);
                }
            }
        }
    }
}

module rack_legs_cutouts(rack_size) {
    c = rack_size.y;
    a = cos(rack_angle) * c;
    b = sin(rack_angle) * c;

    base_length = a;
    offset_z = 6;

    near_cutout_height = rack_base_height - 5;
    far_cutout_height = rack_base_height + b - 10;
    cutout_depth = magnet_inset * 2;

    // far cutout
    translate([rack_lip_width, rack_lip_width, offset_z]) {
        linear_extrude(near_cutout_height) {
            square([rack_size.x - rack_lip_width * 2, cutout_depth]);
        }
    }

    // near cutout
    translate([rack_lip_width, a - rack_lip_width - cutout_depth, offset_z]) {
        linear_extrude(far_cutout_height) {
            square([rack_size.x - rack_lip_width * 2, cutout_depth]);
        }
    }

    screw_hole_inset_y = (magnet_inset) * cos(rack_angle) + rack_lip_width;
    near_screw_hole_offset_z = rack_base_height - 10;
    far_screw_hole_offset_z = rack_base_height + b - 15;

    // near screw holes
    translate([0, screw_hole_inset_y, near_screw_hole_offset_z]) {
        rotate(90, [0, 1, 0]) {
            linear_extrude(rack_size.x) {
                circle(d = 3.3);
            }
        }
    }
    // far screw holes
    translate([0, base_length - screw_hole_inset_y, far_screw_hole_offset_z]) {
        rotate(90, [0, 1, 0]) {
            linear_extrude(rack_size.x) {
                circle(d = 3.3);
            }
        }
    }
}

module rack_legs_chamfers(rack_size) {
    c = rack_size.y;
    a = cos(rack_angle) * c;
    b = sin(rack_angle) * c;


    lip_offset = cos(rack_angle) * rack_lip_width;

    near_chamfer_width = magnet_inset * 2;
    near_offset = [
        0,
        lip_offset,
        rack_base_height + lip_offset,
        ];

    translate(near_offset) {
        rotate(90, [0, 1, 0]) {
            color("#ff0000") {
                #linear_extrude(rack_size.x) {
                    polygon([
                            [0, 0],
                            [0, near_chamfer_width],
                            [near_chamfer_width, 0],
                        ]);
                }
            }
        }
    }

    far_chamfer_width = magnet_inset * 2 + lip_offset / 2;
    far_offset = [
            0 + 0,
            a - far_chamfer_width,
                b + rack_base_height - lip_offset * 2,
        ];

    translate(far_offset) {
        rotate(90, [0, 1, 0]) {
            #linear_extrude(rack_size.x) {
                polygon([
                        [0, 0],
                        [0, far_chamfer_width],
                        [far_chamfer_width, far_chamfer_width]
                    ]);
            }
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