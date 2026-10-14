"""
Fit a fast analytic (lat, lon) -> (i, j) inverse for a NEMO ORCA tripolar grid (here ORCA12,
4322 x 3059) and emit a self-contained GLSL function.

Model
-----
South of `LAT_SWITCH` the ORCA grid is an exact isotropic Mercator grid:
    i = (lon - LON0) / DLON                     (mod NPERIOD)
    j = J_EQ + ln(tan(pi/4 + lat/2)) / DLON_RAD
North of it the two grid poles (Siberia / Canada) lie on the LON_FOLD meridian. In polar
stereographic coordinates z = tan(colat/2) * exp(i*(lon - LON_FOLD)) the grid is close to
elliptic coordinates with foci at the two poles:
    w = mu + i*nu = acosh((z - c) / a)
    i ~ 1 + nu / DLON_RAD,  j ~ J_EQ - (ln(a/2) + mu) / DLON_RAD      (-> Mercator far south)
plus a small smooth correction that is a Chebyshev(mu) x Fourier(nu) series. The grid is mirror
symmetric around the fold meridian, so di only needs sin(n nu) and dj only cos(n nu).

Usage:  python orca_inverse_fit.py <file.nc> [out.glsl] [M] [F]
"""

import sys
import numpy as np
import h5py
from numpy.polynomial import chebyshev as C
from scipy.optimize import minimize

path = sys.argv[1]
out = sys.argv[2] if len(sys.argv) > 2 else "orca_inverse.glsl"
M = int(sys.argv[3]) if len(sys.argv) > 3 else 8  # Chebyshev degree in mu
F = int(sys.argv[4]) if len(sys.argv) > 4 else 8  # Fourier degree in nu
LAT_SWITCH = 20.0
MU_SCALE = 3.0  # t = 2*mu/MU_SCALE - 1 must cover mu at LAT_SWITCH (~1.7)

with h5py.File(path, "r") as f:
    lat = f["nav_lat"][:].astype(np.float64)
    lon = f["nav_lon"][:].astype(np.float64)
ny, nx = lat.shape
valid = ~((lat == -1) & (lon == -1))  # eliminated land-only domains are filled with -1
J, I = np.mgrid[0:ny, 0:nx]
NPERIOD = nx - 2  # two cyclic halo columns

# --- southern Mercator part ----------------------------------------------------------------
rowSpread = np.array([np.ptp(lat[j, valid[j]]) if valid[j].any() else np.inf for j in range(ny)])
jMerc = np.where(rowSpread < 1e-4)[0]
jMerc = jMerc[jMerc < np.argmax(rowSpread > 1e-3)]
latRow = np.array([lat[j, valid[j]][0] for j in jMerc])
y = np.log(np.tan(np.pi / 4 + np.radians(latRow) / 2))
DLON_RAD, yoff = np.polyfit(jMerc, y, 1)  # y = DLON_RAD * j + yoff
J_EQ = -yoff / DLON_RAD
DLON = np.degrees(DLON_RAD)
jr = jMerc[len(jMerc) // 2]
iv = np.where(valid[jr])[0]
lonU = np.degrees(np.unwrap(np.radians(lon[jr, iv])))
LON0 = float(np.median(lonU - iv * DLON))
LON0 = (LON0 + 180) % 360 - 180
print(f"Mercator: DLON={DLON:.8f} deg, J_EQ={J_EQ:.6f}, LON0={LON0:.6f}, "
      f"Mercator max lat {latRow.max():.3f}")

# --- fold row / fold meridian ---------------------------------------------------------------
jFold = None
for j in range(ny - 1, ny - 5, -1):
    v = valid[j]
    if v.any() and np.all(np.isclose(np.abs(((lon[j, v] - lon[j, v][0]) + 180) % 360 - 180) % 180, 0, atol=1e-3)):
        jFold = j
        break
LON_FOLD = float(np.round(lon[jFold, valid[jFold]][np.argmin(lat[jFold, valid[jFold]])]))
# use the meridian on the i~1 side (east, same side as LON0)
if abs(((LON_FOLD - LON0) + 180) % 360 - 180) > 90:
    LON_FOLD = (LON_FOLD + 180 + 180) % 360 - 180
print(f"fold row j={jFold}, fold meridian {LON_FOLD} / {LON_FOLD - 180}")

# --- northern elliptic model fit ------------------------------------------------------------
m = valid & (lat > LAT_SWITCH - 5) & (J < jFold)
la = np.radians(lat[m])
th = np.radians(lon[m] - LON_FOLD)
Ii, Jj = I[m].astype(float), J[m].astype(float)
z = np.tan((np.pi / 2 - la) / 2) * np.exp(1j * th)
i_shift = (LON_FOLD - LON0) / DLON  # i at nu = 0


def design(mu, nu, trig):
    t = 2 * mu / MU_SCALE - 1
    cols = []
    for k in range(M + 1):
        Tk = C.chebval(t, [0] * k + [1])
        cols.append(Tk)
        for n in range(1, F + 1):
            cols.append(Tk * trig(n * nu))
    return np.stack(cols, 1)


def residuals(a, c, idx):
    w = np.arccosh((z[idx] - c) / a)
    mu, nu = w.real, w.imag
    di = (Ii[idx] - (i_shift + nu / DLON_RAD) + NPERIOD / 2) % NPERIOD - NPERIOD / 2
    dj = Jj[idx] - (J_EQ - (np.log(a / 2) + mu) / DLON_RAD)
    return mu, nu, di, dj


def fit(a, c, idx):
    mu, nu, di, dj = residuals(a, c, idx)
    Ai, Aj = design(mu, nu, np.sin), design(mu, nu, np.cos)
    ci = np.linalg.lstsq(Ai, di, rcond=None)[0]
    cj = np.linalg.lstsq(Aj, dj, rcond=None)[0]
    return ci, cj, np.hypot(Ai @ ci - di, Aj @ cj - dj)


rng = np.random.default_rng(0)
S = rng.choice(len(z), min(200000, len(z)), replace=False)
res = minimize(lambda q: np.sqrt(np.mean(fit(q[0], q[1], S[:60000])[2] ** 2)), [0.3, 0.07],
               method="Nelder-Mead", options=dict(xatol=1e-5, fatol=1e-4))
A_FOC, C_FOC = res.x
ci, cj, _ = fit(A_FOC, C_FOC, S)
print(f"foci: a={A_FOC:.6f} c={C_FOC:.6f} -> poles at lat "
      f"{90 - 2 * np.degrees(np.arctan(C_FOC + A_FOC)):.3f} ({LON_FOLD}E) and "
      f"{90 - 2 * np.degrees(np.arctan(A_FOC - C_FOC)):.3f} ({LON_FOLD - 180}E)")


# --- float32 emulation of the GLSL code, validated against every ocean point -----------------
def predict32(latd, lond):
    f = np.float32
    latd, lond = latd.astype(f), lond.astype(f)
    lr = np.radians(latd)
    iS = np.mod((lond - f(LON0)) / f(DLON), f(NPERIOD))
    jS = f(J_EQ) + np.log(np.tan(f(np.pi / 4) + f(0.5) * lr)) / f(DLON_RAD)
    r = np.tan(f(np.pi / 4) - f(0.5) * lr)
    t = np.radians(lond - f(LON_FOLD))
    zeta = ((r * np.cos(t) - f(C_FOC)) + 1j * (r * np.sin(t))).astype(np.complex64) / f(A_FOC)
    w = np.log(zeta + np.sqrt(zeta + 1) * np.sqrt(zeta - 1)).astype(np.complex64)
    mu, nu = w.real.astype(f), w.imag.astype(f)
    Ai, Aj = design(mu, nu, np.sin).astype(f), design(mu, nu, np.cos).astype(f)
    iN = np.mod(f(i_shift) + nu / f(DLON_RAD) + Ai @ ci.astype(f), f(NPERIOD))
    jN = f(J_EQ) - (f(np.log(A_FOC / 2)) + mu) / f(DLON_RAD) + Aj @ cj.astype(f)
    north = latd > f(LAT_SWITCH)
    return np.where(north, iN, iS), np.where(north, jN, jS)


ev = valid & (J <= jFold)
pi_, pj_ = predict32(lat[ev], lon[ev])
ti, tj = I[ev].astype(float), J[ev].astype(float)


def wrapd(d):
    return (d + NPERIOD / 2) % NPERIOD - NPERIOD / 2


e1 = np.hypot(wrapd(pi_ - ti), pj_ - tj)
e2 = np.hypot(wrapd(pi_ - (nx - ti)), pj_ - (2 * jFold - tj))  # north-fold equivalent point
e = np.minimum(e1, e2)
for name, sel in [("all", np.ones_like(e, bool)), ("south", lat[ev] <= LAT_SWITCH),
                  ("north", lat[ev] > LAT_SWITCH)]:
    print(f"{name:6s} error [cells]: rms {np.sqrt(np.mean(e[sel] ** 2)):.3f}  p99 "
          f"{np.percentile(e[sel], 99):.3f}  p99.9 {np.percentile(e[sel], 99.9):.3f}  max {e[sel].max():.3f}")


# --- GLSL ------------------------------------------------------------------------------------
def arr(name, v):
    body = ",\n    ".join(", ".join(f"{x: .9e}" for x in v[k:k + F + 1]) for k in range(0, len(v), F + 1))
    return f"const float {name}[{len(v)}] = float[](\n    {body});\n"


glsl = f"""// Generated by orca_inverse_fit.py. Fast approximate (lat, lon) -> ORCA grid index.
// Returns continuous 0-based (i, j) T-point index: voxel center (i, j) has texcoord
// ((i + 0.5) / {nx}, (j + 0.5) / {ny}). i is periodic with period {NPERIOD}.
// Fit error (ocean points): rms {np.sqrt(np.mean(e ** 2)):.3f} cells, max {e.max():.2f} cells.

#define ORCA_M {M}
#define ORCA_F {F}
const float ORCA_PI = 3.14159265358979;
const float ORCA_LAT_SWITCH = {LAT_SWITCH:.6f};
const float ORCA_LON0 = {LON0:.9f};
const float ORCA_DLON = {DLON:.12f};
const float ORCA_DLON_RAD = {DLON_RAD:.12e};
const float ORCA_J_EQ = {J_EQ:.6f};
const float ORCA_NPERIOD = {NPERIOD:.1f};
const float ORCA_LON_FOLD = {LON_FOLD:.6f};
const float ORCA_A = {A_FOC:.9f};
const float ORCA_C = {C_FOC:.9f};
const float ORCA_LOG_HALF_A = {np.log(A_FOC / 2):.9f};
const float ORCA_I_SHIFT = {i_shift:.6f};
const float ORCA_MU_SCALE = {MU_SCALE:.6f};

// coefficient [k * (ORCA_F + 1) + n] multiplies T_k(t) * sin(n nu) (i) / cos(n nu) (j); n = 0 -> 1
{arr("ORCA_CI", ci)}
{arr("ORCA_CJ", cj)}
vec2 orcaCMul(vec2 a, vec2 b) {{ return vec2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x); }}
vec2 orcaCSqrt(vec2 z) {{
    float r = length(z);
    return vec2(sqrt(max(0.5 * (r + z.x), 0.0)), (z.y < 0.0 ? -1.0 : 1.0) * sqrt(max(0.5 * (r - z.x), 0.0)));
}}
vec2 orcaCLog(vec2 z) {{ return vec2(log(length(z)), atan(z.y, z.x)); }}

vec2 orcaLatLonToIJ(float latDeg, float lonDeg) {{
    float latR = radians(latDeg);
    if (latDeg <= ORCA_LAT_SWITCH) {{  // exact isotropic Mercator part
        float i = mod((lonDeg - ORCA_LON0) / ORCA_DLON, ORCA_NPERIOD);
        float j = ORCA_J_EQ + log(tan(0.25 * ORCA_PI + 0.5 * latR)) / ORCA_DLON_RAD;
        return vec2(i, j);
    }}
    // polar stereographic plane, fold meridian along +x, then elliptic coords around the 2 poles
    float r = tan(0.25 * ORCA_PI - 0.5 * latR);
    float t = radians(lonDeg - ORCA_LON_FOLD);
    vec2 zeta = vec2(r * cos(t) - ORCA_C, r * sin(t)) / ORCA_A;
    vec2 w = orcaCLog(zeta + orcaCMul(orcaCSqrt(zeta + vec2(1.0, 0.0)), orcaCSqrt(zeta - vec2(1.0, 0.0))));
    float mu = w.x;
    float nu = w.y;

    float tc = 2.0 * mu / ORCA_MU_SCALE - 1.0;
    float T[ORCA_M + 1];
    T[0] = 1.0;
    T[1] = tc;
    for (int k = 2; k <= ORCA_M; ++k) T[k] = 2.0 * tc * T[k - 1] - T[k - 2];

    vec2 e1 = vec2(cos(nu), sin(nu));
    vec2 en = vec2(1.0, 0.0);  // (cos(n nu), sin(n nu))
    float di = 0.0;
    float dj = 0.0;
    for (int n = 0; n <= ORCA_F; ++n) {{
        float si = 0.0;
        float sj = 0.0;
        for (int k = 0; k <= ORCA_M; ++k) {{
            si += ORCA_CI[k * (ORCA_F + 1) + n] * T[k];
            sj += ORCA_CJ[k * (ORCA_F + 1) + n] * T[k];
        }}
        di += si * (n == 0 ? 1.0 : en.y);
        dj += sj * en.x;
        en = orcaCMul(en, e1);
    }}
    float i = mod(ORCA_I_SHIFT + nu / ORCA_DLON_RAD + di, ORCA_NPERIOD);
    float j = ORCA_J_EQ - (ORCA_LOG_HALF_A + mu) / ORCA_DLON_RAD + dj;
    return vec2(i, j);
}}
"""
with open(out, "w") as fh:
    fh.write(glsl)
print("wrote", out)
