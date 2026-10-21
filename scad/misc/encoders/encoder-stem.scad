use <../../common/shared-utils.scad>
include <../../common/shared-variables.scad>
include <./shared-variables.scad>

button_height_z = 3.2;
button_width_x = 6.2;
button_width_y = 3.6;
button_size_xy = [button_width_x, button_width_y];
button_offset_y = 4.5;

inner_diameter = 16.5; // 15.3
stem_height = 12.5 + button_height_z - 1;
stem_margin_z = 0.1;
spoke_height = 3;
spoke_width = 2.6; // 3;
spoke_depth = 0.8;
inset_margin_d = 0.6;


encoder_stem();

translate([100, 0, 0]) {
    //encoder_stem(include_spokes = true);
}

module encoder_stem(include_spokes = false) {
    difference() {
        union() {
            linear_extrude(stem_height + stem_margin_z) {
                circle(d = inner_diameter);
            }

            if (include_spokes) {
                translate([0, 0, stem_height - spoke_height + stem_margin_z]) {
                    linear_extrude(spoke_height) {
                        spoke_size = [inner_diameter + spoke_depth * 2, spoke_width];
                        square(spoke_size, center = true);
                        rotate(90) {
                            square(spoke_size, center = true);
                        }
                    }
                }
            }

            translate([0, 0, stem_height + stem_margin_z]) {
                linear_extrude(encoder_cover_height - stem_margin_z) {
                    circle(d = encoder_cover_diameter - encoder_cover_inset - inset_margin_d);
                }
            }
        }

        union() {
            threaded_insert_cutouts();
            button_cutouts();
        }
    }
}

module threaded_insert_cutouts() {
    *translate([inner_diameter / 4, 0, 0])
        rotate(180, [0, 1, 0])
            threaded_insert_cutout();


    *translate([-inner_diameter / 4, 0, 0])
        rotate(180, [0, 1, 0])
            threaded_insert_cutout();

    rotate(180, [0, 1, 0])
        threaded_insert_cutout();
}

module button_cutouts() {
    translate([0, button_offset_y]) {
        button_cutout();
    }
    translate([0, -button_offset_y]) {
        button_cutout();
    }
}

module button_cutout() {
    linear_extrude(button_height_z) {
        scale(1.1) {
            square(button_size_xy, center = true);
        }
    }
}