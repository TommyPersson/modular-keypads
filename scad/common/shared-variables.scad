$fn = 150;

u_1 = 19.05;

plate_corner_radius = 2;

screw_hole_diameter = 2.7;

switch_neck_thickness = 1.2;
switch_neck_width = 13.9;
switch_hole_width = switch_neck_width + 2;

rotary_encoder_width = 17;
rotary_encoder_height = 17;

magnet_diameter = 5 + 0.3;
magnet_depth = 2 + 0.2;
magnet_inset = 5;

module_width_x_unit = 20;
module_height_y = 180;

module_bottom_base_thickness = 4;

pcb_thickness = 1.6;

wall_tolerance = 0.5;
outer_wall_thickness_base = 2;
outer_wall_thickness = outer_wall_thickness_base - (wall_tolerance / 2);
inner_wall_thickness = 2;

inner_wall_height = 3;
outer_wall_height = inner_wall_height + pcb_thickness;

screw_hole_inset_pcb = 4;
screw_hole_inset = screw_hole_inset_pcb + outer_wall_thickness_base;

threaded_insert_towers_diameter = 5;
threaded_insert_towers_radius = threaded_insert_towers_diameter / 2;

threaded_insert_length = 4;
threaded_insert_knurl_diameter = 3;

side_connector_height = 4.7;
side_connector_length = 23;
side_connector_offset = 40;

back_connector_height = 5;
back_connector_offset = 15;

bottom_module_base_cutout_inset_x = 10;
bottom_module_base_cutout_inset_y = 10;

module_top_thickness = 2;

module_total_height = module_bottom_base_thickness + outer_wall_height + module_top_thickness;