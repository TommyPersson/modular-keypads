include <./shared-variables.scad>
use <../common/shared-utils.scad>

function get_module_size(width_units) =
    [
        width_units * module_width_x_unit,
        module_height_y * 1
    ];

function get_pcb_size(width_units) =
let (xy = get_module_size(width_units))
    [
        xy.x - (outer_wall_thickness_base * 2),
        xy.y - (outer_wall_thickness_base * 2)
    ];


module magnet_hole_cutouts(module_size) {
    x_inset = module_width_x_unit / 2;
    y_inset = magnet_inset;

    points = [
            [x_inset, y_inset],
            [module_size.x - x_inset, y_inset],
            [x_inset, module_size.y - y_inset],
            [module_size.x - x_inset, module_size.y - y_inset]
        ];

    for (p = points) {
        translate(p) {
            magnet_hole_cutout();
        }
    }
}

module magnet_hole_cutout() {
    linear_extrude(magnet_depth) {
        circle(d = magnet_diameter);
    }
}