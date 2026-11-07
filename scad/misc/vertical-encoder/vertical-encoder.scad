use <../../common/shared-utils.scad>
include <../../common/shared-variables.scad>

encoder_height = 9.0; // pcb to center hole
stem_diameter = 2.0;
stem_clearance = 0.04;
wheel_diameter = 16;
num_steps = 36;

*translate([-50, 0]) {
    left_wheel();
}

*translate([-100, 0]) {
    right_wheel();
}

translate([0, 0]) {
    combinded();
}

module combinded() {
    left_wheel();
    rotate(180, [1, 0, 0]) {
        right_wheel();
    }
}

module left_wheel() {
    wheel_thickness = 6;
    stem_depth = 4;

    linear_extrude(wheel_thickness) {
        wheel_shape();
    }

    translate([0, 0, wheel_thickness]) {
        linear_extrude(stem_depth) {
            circle(d = stem_diameter - stem_clearance, $fn = 6);
        }

        cylinder(h = 1, d1 = 3, d2 = 2);
    }
}

module right_wheel() {
    wheel_thickness = 10;
    stem_depth = 4;

    linear_extrude(wheel_thickness) {
        wheel_shape();
    }

    translate([0, 0, wheel_thickness]) {
        linear_extrude(stem_depth) {
            circle(d = stem_diameter - stem_clearance);
        }

        cylinder(h = 1, d1 = 3, d2 = 2);
    }
}

module wheel_shape() {
    difference() {
        circle(d = wheel_diameter);

        for (i = [1:num_steps]) {
            angle = i * (360 / num_steps);
            rotate(angle) {
                translate([wheel_diameter / 2, 0]) {
                    circle(d = 1);
                }
            }
        }
    }
}