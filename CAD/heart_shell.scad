// ============================================================
// CLICKY — HEART SHELL (3D-printed heart for the Clicky pet)
// ============================================================
// Free software: OpenSCAD (openscad.org).
// Part to render via the `part` variable below:
//   "front"   -> front shell half (round eye window for the screen)
//   "back"    -> back shell half
//   "preview" -> both halves in mating position
//
// Sized for the Clicky PCB: 95.25 x 84.67 mm (a heart outline in
// Edge.Cuts). The shell cavity must be larger than the board so it
// seats inside. The round GC9A01 1.28" screen (32.4 mm glass, ~37.5 mm
// module PCB) shows through a round window on the front.
//
// The two halves press-fit together: the FRONT half (lid) is slightly
// larger than the BACK half (tub); its skirt slips over the back's top
// rim with radial interference so it stays put (lid-over-tub).
// ============================================================

/* [Overall size] */
heart_scale    = 3.4;    // heart curve scale (width ~= 32*scale mm)
body_thickness = 18;     // mm assembled front-to-back (both halves)
wall           = 2.0;    // mm side wall thickness
back_wall      = 2.5;    // mm back "floor" (solid back of the tub)

/* [Heart curve shaping] */
// Rounding the outline BEFORE hollowing. Values below ~6 leave a sharp
// bottom cusp (~0 deg) and a ~23 mm deep top cleft, which make the inward
// cavity offset collapse into a thin "V" of material through the centre
// and spike elongations at the cleft. Keep these large enough to round both.
curve_dx = 14;    // rounds the top cleft (fills the notch between lobes)
curve_dy = 12;    // rounds the bottom point (kills the sharp cusp)

/* [Eye / screen window] */
eye_dia          = 33;   // visible round opening (glass is 32.4)
eye_module_dia   = 39;   // shallow recess for the ~37.5 mm module PCB behind
eye_ledge_depth  = 1.5;  // how deep the module recess sits into the front wall
eye_dx           = 0;    // nudge the eye sideways
eye_dy           = -3;   // eye slightly below centre (negative = lower)

/* [Press-fit between the halves] */
// Lid-over-tub: the FRONT (lid) is `lid_grow` mm larger in radius than the
// BACK (tub), so its bottom skirt wraps around the back's top rim. The
// skirt's inner radius is `mating_press` mm tighter than the back's outer
// wall to grip (interference). Interior cavity stays flush (same inner
// radius) so the PCB seats cleanly.
mating_overlap = 3.0;    // skirt height (vertical engagement over the rim)
lid_grow       = 0.6;    // how much bigger the lid is than the tub (radial)
mating_press   = 0.08;   // interference: skirt is this much tighter (radial)

/* [Which piece to render] */
part = "preview"; // "front" | "back" | "preview"

$fn = 96;

// ------------------------------------------------------------------
// Heart outline
// ------------------------------------------------------------------
// Classic parametric heart. Returns [x,y] at parameter t (degrees).
function heart_pt(t, s) =
    let(a = t, r = 1.0)
    [ s*16*pow(sin(a),3),
      s*(13*cos(a) - 5*cos(2*a) - 2*cos(3*a) - cos(4*a)) ];

// Rounded heart outline (round BEFORE hollowing, see notes above).
module heart_2d(s) {
    offset(r = curve_dx) offset(r = -curve_dx)
    offset(r = curve_dy) offset(r = -curve_dy)
    polygon([for (t=[0:0.5:360]) heart_pt(t, s)]);
}

// Outer outline of the shell (module — heart_2d is a module).
module outer() {
    heart_2d(heart_scale);
}

// The hollow interior: same heart inset by the wall thickness.
module cavity_2d() {
    offset(delta = -wall) outer();
}

// A heart ring (annulus) spanning z0..z1. `out_off`/`in_off` are radial
// offsets from the outer outline (negative = inward). The ring is solid
// between those two radii.
module ring(z0, z1, out_off, in_off) {
    linear_extrude(height = z1 - z0)
        difference() {
            offset(delta = out_off) outer();
            offset(delta = in_off)  outer();
        };
}

// A full solid heart slab (no hole) spanning z0..z1 with optional offset.
module slab(z0, z1, off) {
    linear_extrude(height = z1 - z0) offset(delta = off) outer();
}

// ------------------------------------------------------------------
// Eye window (front wall only)
// ------------------------------------------------------------------
// A round hole through the front wall for the glass, plus a shallow
// recess (on the cavity side) that seats the module PCB flush behind.
module eye_window() {
    translate([eye_dx, eye_dy, 0]) {
        // glass opening, through the full front wall thickness
        translate([0, 0, body_thickness - wall - 0.01])
            cylinder(d = eye_dia, h = wall + 0.02, center=false);
        // module recess, on the inside face of the front wall
        translate([0, 0, body_thickness - wall - eye_ledge_depth - 0.01])
            cylinder(d = eye_module_dia, h = eye_ledge_depth + 0.01, center=false);
    }
}

// ------------------------------------------------------------------
// The two halves
// ------------------------------------------------------------------
z_mid = body_thickness/2;   // mating plane

// BACK half (tub): z = 0 .. z_mid.
//   - solid floor at the bottom (back_wall)
//   - hollow cavity above
//   - plain full-size top rim (the lid's skirt grips onto this)
module back_half() {
    difference() {
        union() {
            // solid floor slab
            translate([0,0,0]) slab(0, back_wall, 0);
            // side wall above the floor, full thickness
            translate([0,0, back_wall]) ring(back_wall, z_mid, 0, -wall);
        }
    }
}

// FRONT half (lid): z = (z_mid - mating_overlap) .. body_thickness.
//   - wider by lid_grow (so it is "slightly bigger" than the back)
//   - bottom skirt (mating_overlap tall) hangs below the mating plane; its
//     inner radius is `mating_press` smaller than the back's outer wall, so
//     it grips the back's top rim (interference press fit)
//   - hollow cavity + front wall + eye window
module front_half() {
    z_bottom = z_mid - mating_overlap;
    difference() {
        // solid lid body, larger outline
        translate([0,0, z_bottom]) slab(z_bottom, body_thickness, lid_grow);
        // skirt zone: remove everything inside the grip radius, leaving a
        // thin skirt ring that wraps the back's top rim
        translate([0,0, z_bottom])
            linear_extrude(height = mating_overlap + 0.01)
                offset(delta = -mating_press) outer();
        // main cavity (interior flush with the back's cavity)
        translate([0,0, z_mid])
            linear_extrude(height = body_thickness - z_mid - wall + 0.01) cavity_2d();
        // eye window in the front wall
        eye_window();
    }
}

// ------------------------------------------------------------------
// Render selection
// ------------------------------------------------------------------
if (part == "front") {
    color([0.78,0.79,0.81]) front_half();
} else if (part == "back") {
    color([0.72,0.73,0.75]) back_half();
} else {
    // preview: both halves in mating position
    color([0.55,0.56,0.58]) back_half();
    color([0.78,0.79,0.81]) front_half();
}