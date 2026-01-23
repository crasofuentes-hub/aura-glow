#![allow(non_camel_case_types)]

use core::slice;

#[repr(C)]
pub struct auraglow_status {
    pub ok: bool,
    pub code: i32,
}

#[repr(C)]
pub struct auraglow_rgb8 {
    pub r: u8,
    pub g: u8,
    pub b: u8,
}

#[repr(C)]
pub struct auraglow_dye_params {
    pub dye: auraglow_rgb8,
    pub intensity: f32,     // 0..1
    pub preserve_luma: f32, // 0..1
}

#[repr(C)]
pub struct auraglow_frame_view {
    pub data: *mut u8, // RGBA8 interleaved
    pub width: i32,
    pub height: i32,
    pub stride: i32, // bytes por fila
}

#[repr(C)]
pub struct auraglow_mask_view {
    pub data: *const u8, // mask 0..255
    pub width: i32,
    pub height: i32,
    pub stride: i32, // bytes por fila
}

#[inline]
fn clamp01(x: f32) -> f32 {
    if x < 0.0 { 0.0 } else if x > 1.0 { 1.0 } else { x }
}

#[inline]
fn lerp(a: f32, b: f32, t: f32) -> f32 {
    a + (b - a) * t
}

#[inline]
fn luma(r: f32, g: f32, b: f32) -> f32 {
    0.2126*r + 0.7152*g + 0.0722*b
}

#[inline]
fn sat_u8(x: f32) -> u8 {
    let y = if x < 0.0 { 0.0 } else if x > 255.0 { 255.0 } else { x };
    y.round() as u8
}

#[unsafe(no_mangle)]
pub extern "C" fn auraglow_apply_dye_rgba(
    frame: auraglow_frame_view,
    mask: auraglow_mask_view,
    p: auraglow_dye_params,
) -> auraglow_status {
    // Validaciones mínimas (sin UB)
    if frame.data.is_null() || mask.data.is_null() {
        return auraglow_status { ok: false, code: -1 };
    }
    if frame.width <= 0 || frame.height <= 0 || mask.width <= 0 || mask.height <= 0 {
        return auraglow_status { ok: false, code: -2 };
    }
    if frame.width != mask.width || frame.height != mask.height {
        return auraglow_status { ok: false, code: -3 };
    }
    if frame.stride <= 0 || mask.stride <= 0 {
        return auraglow_status { ok: false, code: -4 };
    }

    // Evitar overflow y asegurar RGBA8
    let w = frame.width as usize;
    let h = frame.height as usize;

    if w > (i32::MAX as usize) / 4 {
        return auraglow_status { ok: false, code: -5 };
    }

    let row_bytes = w * 4;
    if (frame.stride as usize) < row_bytes {
        return auraglow_status { ok: false, code: -6 };
    }
    if (mask.stride as usize) < w {
        return auraglow_status { ok: false, code: -7 };
    }

    let intensity = clamp01(p.intensity);
    let preserve = clamp01(p.preserve_luma);

    let dr = (p.dye.r as f32) / 255.0;
    let dg = (p.dye.g as f32) / 255.0;
    let db = (p.dye.b as f32) / 255.0;

    for y in 0..h {
        // Unsafe mínimo: calcular punteros de fila y crear slices.
        let (frow, mrow) = unsafe {
            let frow_ptr = frame.data.add(y * (frame.stride as usize));
            let mrow_ptr = mask.data.add(y * (mask.stride as usize));
            let f = slice::from_raw_parts_mut(frow_ptr, row_bytes);
            let m = slice::from_raw_parts(mrow_ptr, w);
            (f, m)
        };

        for x in 0..w {
            let m = (mrow[x] as f32) / 255.0;
            if m <= 0.0 { continue; }

            let i = x * 4;
            let r = (frow[i + 0] as f32) / 255.0;
            let g = (frow[i + 1] as f32) / 255.0;
            let b = (frow[i + 2] as f32) / 255.0;
            let a = frow[i + 3];

            let t = clamp01(m * intensity);

            let mut rr = lerp(r, dr, t);
            let mut gg = lerp(g, dg, t);
            let mut bb = lerp(b, db, t);

            if preserve > 0.0 {
                let l0 = luma(r, g, b);
                let l1 = luma(rr, gg, bb);
                let delta = l0 - l1;
                rr = clamp01(rr + delta * preserve);
                gg = clamp01(gg + delta * preserve);
                bb = clamp01(bb + delta * preserve);
            }

            frow[i + 0] = sat_u8(rr * 255.0);
            frow[i + 1] = sat_u8(gg * 255.0);
            frow[i + 2] = sat_u8(bb * 255.0);
            frow[i + 3] = a;
        }
    }

    auraglow_status { ok: true, code: 0 }
}
