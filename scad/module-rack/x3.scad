use <../common/module-common-routines.scad>
use <../common/shared-utils.scad>
include <../common/shared-variables.scad>

module_size = 4;
num_modules = 3;

rack_inner_size = get_module_size(module_size * num_modules);

rim_wall_width = 2;
rim_wall_height = 5;

top_height = 25;
top_wall_width = 15;
top_wall_bevel_depth = 2;
top_wall_bevel_inset_x = 25;
top_wall_bevel_inset_y = 25;
top_wall_bevel_inset_z = 5;

bottom_cutout_height = 20;

feet_diameter = 9;
feet_inset = top_wall_width/2;
feet_depth = 1.5;

angle = 5;

model();

module model() {
    difference() {
        translate([0, 0, -bottom_cutout_height]) {
            difference() {
                rotate(5, [1, 0, 0])
                    top();

                bottomCutout();
            }
        }

        bottomFeetCutouts();
    }

}

translate([rack_inner_size.x + 20, 0]) {
    top();
}

module top() {
    difference() {
        translate([0, -rim_wall_width]) {
            linear_extrude(top_height) {
                difference() {
                    rounded_rectangle([rack_inner_size.x, rack_inner_size.y + rim_wall_width], corner_radius = 1);
                    translate([top_wall_width, top_wall_width + rim_wall_width]) {
                        rounded_rectangle(
                            [rack_inner_size.x - top_wall_width * 2, rack_inner_size.y - top_wall_width * 2],
                        corner_radius = 1
                        );
                    }
                }
            }

            linear_extrude(top_height + rim_wall_height) {
                rounded_rectangle([rack_inner_size.x, rim_wall_width], corner_radius = 1);
            }
        }

        topMagnetHoles();
        topWallBevels();
    }
}

module topMagnetHoles() {
    top_y_offset = module_height_y - magnet_inset;
    bottom_y_offset = magnet_inset;

    for (i = [0 : num_modules - 1]) {
        left_x = i * (module_width_x_unit * module_size);
        right_x = i * (module_width_x_unit * module_size) + module_width_x_unit * 3;

        points = [
                [left_x + (module_width_x_unit / 2), top_y_offset],
                [left_x + (module_width_x_unit / 2), bottom_y_offset],
                [right_x + (module_width_x_unit / 2), top_y_offset],
                [right_x + (module_width_x_unit / 2), bottom_y_offset]
            ];

        translate([0, 0, top_height - magnet_depth]) {
            for (p = points) {
                translate(p) {
                    magnet_hole_cutout();
                }
            }
        }
    }
}

module topWallBevels() {
    // verticals
    {
        x_offsets = [
                [0, 1],
                [top_wall_width, -1],
                [rack_inner_size.x, -1],
                [rack_inner_size.x - top_wall_width, 1]
            ];

        for (x_offset = x_offsets) {
            offset_x = x_offset[0];
            chamfer_offset_modifier = x_offset[1];

            width_x = top_wall_bevel_depth * top_wall_bevel_depth;
            width_y = module_height_y - top_wall_bevel_inset_y * 2;

            difference() {
                linear_extrude(top_height - top_wall_bevel_inset_z) {
                    translate([offset_x - top_wall_bevel_depth, top_wall_bevel_inset_y]) {
                        rounded_rectangle([width_x, width_y], corner_radius = 1);
                    }
                }

                // chamfer
                translate([
                        offset_x + (top_wall_bevel_depth * chamfer_offset_modifier),
                        module_height_y - top_wall_bevel_inset_y,
                        top_height - top_wall_bevel_inset_z
                    ]) {
                    rotate(90, [1, 0, 0]) {
                        points = [
                                [-top_wall_bevel_depth, 0],
                                [top_wall_bevel_depth, 0],
                                [0, -top_wall_bevel_depth],
                            ];

                        linear_extrude(width_y) {
                            polygon(points = points);
                        }
                    }
                }
            }
        }
    }

    // horizontals
    {
        y_offsets = [
                [0, 1],
                [top_wall_width, -1],
                [rack_inner_size.y, -1],
                [rack_inner_size.y - top_wall_width, 1]
            ];

        for (y_offset = y_offsets) {
            offset_y = y_offset[0];
            chamfer_offset_modifier = y_offset[1];

            width_x = rack_inner_size.x - top_wall_bevel_inset_x * 2;
            width_y = top_wall_bevel_depth * top_wall_bevel_depth;

            difference() {
                linear_extrude(top_height - top_wall_bevel_inset_z) {
                    translate([top_wall_bevel_inset_x, offset_y - top_wall_bevel_depth]) {
                        rounded_rectangle([width_x, width_y], corner_radius = 1);
                    }
                }

                // chamfer
                translate([
                    top_wall_bevel_inset_x,
                        offset_y + (top_wall_bevel_depth * chamfer_offset_modifier),
                        top_height - top_wall_bevel_inset_z
                    ]) {

                    rotate(90, [1, 0, 0]) {
                        rotate(90, [0, 1, 0]) {
                            points = [
                                    [-top_wall_bevel_depth, 0],
                                    [top_wall_bevel_depth, 0],
                                    [0, -top_wall_bevel_depth],
                                ];

                            linear_extrude(width_x) {
                                polygon(points = points);
                            }
                        }
                    }
                }
            }
        }
    }
}

module bottomCutout() {
    linear_extrude(bottom_cutout_height) {
        translate([-10, -10]) {
            square([rack_inner_size.x + 20, rack_inner_size.y + 20]);
        }
    }
}

module bottomFeetCutouts() {
    points = [
            [feet_inset, feet_inset],
            [feet_inset, (rack_inner_size.y - feet_inset)* cos(angle)],
            [rack_inner_size.x - feet_inset, feet_inset],
            [rack_inner_size.x - feet_inset, (rack_inner_size.y - feet_inset)* cos(angle)]
        ];

    #linear_extrude(feet_depth) {
        for (p = points) {
            translate(p) {
                circle(d = feet_diameter);
            }
        }
    }
}