// Shared variables and helpers for the C-module and its sub-modules.
// Contains no top-level geometry so it can be included anywhere.

use <../common/module-bottom-routines.scad>
use <../common/module-top-routines.scad>
use <../common/shared-utils.scad>
include <./shared-variables.scad>

module_size = get_module_size(4);
bottom_pcb_size = get_pcb_size(4);
pcb_inset_xy = [outer_wall_thickness_base, outer_wall_thickness_base];

bottom_connector_inset = 1.5;
pcb_cover_thickness = bottom_connector_inset;

module_shell_bottom_height = side_connector_height + 2.5;

sub_module_pcb_size = [68, 40];
sub_module_centers = [
        [38, 28],
        [38, 76],
        [38, 124]
    ];

sub_module_connector_offset = 31;
sub_module_connector_size = [5.1, 26.7, 4];
sub_module_separation = 8;
sub_module_shell_wall_width = 2;
sub_module_shell_wall_height = pcb_thickness + sub_module_connector_size.z + module_top_thickness;
sub_module_shell_top_thickness = module_top_cover_thickness;
sub_module_shell_bottom_thickness = sub_module_connector_size.z;
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

connector_pcb_standoff_thickness = sub_module_connector_size.z - bottom_connector_inset;
connector_pcb_size = [bottom_pcb_size.x, bottom_pcb_size.y - 40];

module sub_module_connector() {
    linear_extrude(sub_module_connector_size.z) {
        square([sub_module_connector_size.x, sub_module_connector_size.y], center = true);
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
