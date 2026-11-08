use <./module-c-base.scad>
use <./sub-module-common.scad>
use <./sub-module-1.scad>
use <./sub-module-2.scad>
use <./sub-module-3.scad>
use <./sub-module-4.scad>

translate([-400, 120]) {
    sub_module_1_cover();
}

translate([-400, 180]) {
    sub_module_1_shell();
}

translate([-400, 240]) {
    sub_module_2_cover();
}

translate([-400, 300]) {
    sub_module_2_shell();
}

translate([-400, 360]) {
    sub_module_3_cover();
}

translate([-400, 420]) {
    sub_module_3_shell();
}

translate([-400, 480]) {
    sub_module_4_cover();
}

translate([-400, 540]) {
    sub_module_4_shell();
}


translate([-400, 60]) {
    sub_module_shell_bottom();
}

translate([-300, 0]) {
    connector_pcb_standoffs();
}

translate([-200, 0]) {
    pcb_cover();
}

translate([-200, 200]) {
    module_shell_top();
}

translate([-100, 0]) {
    module_shell_bottom();
}

translate([-100, 200]) {
    connector_pcb_solder_jig();
}