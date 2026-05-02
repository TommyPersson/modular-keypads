use <../common/shared-utils.scad>
include <../common/shared-variables.scad>

cap_size = [u_1 * 1.5 - 1, u_1 - 1];
cap_stem_size = [8, 4];

border_thickness = 2;
base_jig_margin = 20;

num_x = 4;
num_y = 4;

size_x = cap_size.x * num_x + border_thickness * (num_x + 1);
size_y = cap_size.y * num_y + border_thickness * (num_y + 1);

echo(cap_size);

base_jig();

translate([0, size_y + 30]) {
    color("green")
        //rotate(180, [1, 0, 0])
        press();
}

module base_jig() {
    linear_extrude(4) {
        difference() {
            square([size_x + base_jig_margin * 2, size_y + base_jig_margin * 2], center = true);
            union() {
                for (i = [0 : num_x - 1]) {
                    for (j = [0 : num_y - 1]) {
                        x = border_thickness + (i * (cap_size.x + border_thickness)) - size_x / 2;
                        y = border_thickness + (j * (cap_size.y + border_thickness)) - size_y / 2;
                        translate([x, y]) {
                            square(cap_size);
                        }
                    }
                }
            }

        }
    }
}

module press() {
    translate([0, 0, 2]) {
        difference() {
            union() {
                linear_extrude(2) {
                    union() {
                        for (i = [0 : num_x - 1]) {
                            for (j = [0 : num_y - 1]) {
                                x = border_thickness + (i * (cap_size.x + border_thickness)) + 0.2 - size_x / 2;
                                y = border_thickness + (j * (cap_size.y + border_thickness)) + 0.2 - size_y / 2;
                                translate([x, y]) {
                                    difference() {
                                        width_x = cap_size.x - 0.4;
                                        width_y = cap_size.y - 0.4;
                                        square([width_x, width_y]);
                                    }
                                }
                            }
                        }
                    }
                }

                translate([0, 0, 2]) {
                    linear_extrude(2) {
                        difference() {
                            square([size_x, size_y], center = true);

                        }
                    }
                }
            }

            linear_extrude(4) {
                for (i = [0 : num_x - 1]) {
                    for (j = [0 : num_y - 1]) {
                        x = border_thickness + (i * (cap_size.x + border_thickness)) + 0.2 - size_x / 2;
                        y = border_thickness + (j * (cap_size.y + border_thickness)) + 0.2 - size_y / 2;
                        translate([x, y]) {
                            difference() {
                                width_x = cap_size.x - 0.4;
                                width_y = cap_size.y - 0.4;
                                translate([width_x / 2, width_y / 2]) {
                                    square(cap_stem_size, center = true);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}