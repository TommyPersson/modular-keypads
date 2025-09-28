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

module rectangle_with_circle_corners(size, circle_radius = 1, center = false) {
    x_offset = center ? 0 : size.x / 2;
    y_offset = center ? 0 : size.y / 2;
    offset = [x_offset, y_offset];

    translate(offset) {
            square(size, center = true);

            x = (size.x / 2) - circle_radius/2;
            y = (size.y / 2) - circle_radius/2;
            points = [[-x, -y], [x, -y], [-x, y], [x, y]];

            for (p = points) {
                translate([p.x, p.y]) {
                    circle(r = circle_radius);
                }
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