use <../common/module-top-routines.scad>
use <../common/shared-utils.scad>
include <../common/shared-variables.scad>

module_b_top();

module module_b_top() {
    difference() {
        module_top_base(4);

        rotary_encoder_cutouts();
    }
}

module rotary_encoder_cutouts() {
    points = [
            [40, 28.12],
            [40, 75.75],
            [40, 123.34],
        ];

    module_top_rotary_encoder_cutouts(points);
}