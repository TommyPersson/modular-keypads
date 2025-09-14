module rounded_rectangle(size, corner_radius, center = false) {

    x_offset = center ? -size.x / 2 : 0;
    y_offset = center ? -size.y / 2 : 0;

    corners_points = [
            [corner_radius, corner_radius],
            [size.x - corner_radius, corner_radius],
            [size.x - corner_radius, size.y - corner_radius],
            [corner_radius, size.y - corner_radius],
        ];

    hull() {
        for (p = corners_points) {
            translate([p.x + x_offset, p.y + y_offset, -1])
                circle(corner_radius);
        }
    }
}


module circle_segment(radius, cut_off_height) {
    intersection() {
        translate([0, -cut_off_height]) {
            circle(radius);
        }
        translate([-radius, 0]) {
            square([radius * 2, cut_off_height * 2]);
        }
    }
}

module tear_drop_circle(d) {
    r = d / 2;
    angle = 30;
    offset_y = r / 2;
    offset_x = cos(angle) * r;

    triangle_points = [
            [-offset_x, r / 2],
            [offset_x, r / 2],
            [0, 3 / 2 * r]
        ];

    polygon(triangle_points);
    circle(d = d);

}