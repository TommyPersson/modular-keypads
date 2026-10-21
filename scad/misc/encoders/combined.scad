use <./encoder-stem.scad>
use <./encoder-cover.scad>

$fn = 250;

translate([100, 0, 0]) {
    encoder_stem(include_spokes = true);
}

translate([200, 0, 0]) {
    encoder_stem(include_spokes = false);
}

translate([300, 0, 0]) {
    encoder_cover();
}