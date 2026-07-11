use <../../common/shared-utils.scad>
include <../../common/shared-variables.scad>
include <./shared-variables.scad>

inner_diameter = 21.70;
spoke_width = 2.8; // 3.2
spoke_depth = 1.0; // 0.8
spoke_height = 7.5; //7.5;

encoder_cover();

module encoder_cover() {
    linear_extrude(encoder_cover_height) {
        difference() {
            circle(d = encoder_cover_diameter);
            circle(d = encoder_cover_diameter - encoder_cover_inset);
        }
    }

    translate([0, 0, -spoke_height]) {
        difference() {
            union() {
                linear_extrude(spoke_height) {
                    difference() {
                        circle(d = encoder_cover_diameter);
                        circle(d = inner_diameter);
                    }
                }

                upper_spokes_cutouts();
            }

            union() {
            }
        }
    }
}

module upper_spokes_cutouts() {
    linear_extrude(spoke_height) {
        intersection() {
            union() {
                for (i = [1, 2, 3]) {
                    angle = i * 120;

                    rotate(angle) {
                        translate([-spoke_width / 2, 0]) {
                            square([spoke_width, inner_diameter]);
                        }
                    }
                }
            }

            union() {
                #difference() {
                    circle(d = inner_diameter);
                    circle(d = inner_diameter - spoke_depth*2);
                }
            }
        }
    }
}