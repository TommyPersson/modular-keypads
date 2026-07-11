use <../../common/shared-utils.scad>
include <../../common/shared-variables.scad>
include <./shared-variables.scad>

inner_diameter = 15.0; // 15.3
stem_height = 12.5;
stem_margin_z = 0.1;
spoke_height = 3;
spoke_width = 2.6; // 3;
spoke_depth = 0.8;
inset_margin_d = 0.6;


encoder_stem();

translate([100, 0, 0]) {
    encoder_stem(include_spokes = true);
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