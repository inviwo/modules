Yes, there's a good analytic approximation. I built one for your file and wrote it out as a GLSL function that uses no textures. I ran it in a float32 emulation against every ocean point in `nav_lat`/`nav_lon`: in the north the typical error is under 0.1 of a grid cell, and it is exact in the south.

## What the grid is

Your file is on the **ORCA12** grid (4322 × 3059 points; the first and last columns are wrap-around copies, so the longitude period is 4320). The inverse splits into two parts:

**South of about 21.7°N the grid is exactly Mercator, so the inverse is exact:**
```
i = mod((lon − 72.916667) · 12, 4320)
j = 1494 + ln(tan(π/4 + lat/2)) / Δ,    Δ = radians(1/12)
```

**North of that, the grid has two poles on the 73°E / 107°W meridian, where the grid folds over.** Here I project onto a polar stereographic plane, z = tan(colat/2)·e^{i(lon−73°)}. In that plane the grid is close to elliptic coordinates centred on the two poles:
```
μ + iν = acosh((z − c)/a)          a = 0.3156, c = 0.0899  (poles ≈ 45.9°N 73°E, 64.6°N 107°W)
i ≈ 1 + ν/Δ + Σ A_kn T_k(μ) sin(nν)
j ≈ 1494 − (ln(a/2) + μ)/Δ + Σ B_kn T_k(μ) cos(nν)
```
- The elliptic part handles the fold and both poles in closed form. Far from the poles it turns back into the Mercator formula above.
- What's left is a small, smooth correction: a Chebyshev series in μ times a Fourier series in ν. The grid is mirror-symmetric about the fold meridian, so `i` only needs sine terms and `j` only needs cosine terms.

## Accuracy

Errors are measured in grid cells, over all ocean points:

| Correction terms (μ deg × ν deg) | Coefficients | North rms | North p99.9 | North max |
|---|---|---|---|---|
| 8×8 (generated) | 2×81 | 0.078 | 0.68 | 2.2 |
| 6×6 (cheaper) | 2×49 | ~0.25 | ~1.9 | ~3.8 |

South of 20°N the formula is exact (max error 0.001 cells).

## Cost per sample

In the north, each sample costs one complex `log`, two complex `sqrt`s, one `sin`/`cos` pair, one `tan`, and about 160 multiply-adds. All the coefficients are compiled into the shader as constant arrays. The shader compiles under GLSL 330 (checked with glslang); I haven't run it on a GPU.

## Things to know when using it

- **Output:** you get a continuous 0-based `(i, j)`. The texture coordinate is `((i+0.5)/4322, (j+0.5)/3059)`, and `i` repeats with period 4320.
- **Land:** points over land give plausible but meaningless indices. About 28% of the grid is land filled with −1, and you'll need your land mask for those points anyway.
- **Where errors are largest:** the worst errors (about 2 cells) are near the two poles, which are over land, and in a few narrow Arctic channels.
- **Cheaper version:** run `python orca_inverse_fit.py <file.nc> out.glsl 6 6` to generate the 6×6 version.
- **Other ORCA files:** the script works out the grid parameters from the file itself, so it should also work for other ORCA resolutions such as ORCA025 or ORCA05.

Files:
- `../glsl/orca_inverse.glsl`: the generated function `orcaLatLonToIJ(latDeg, lonDeg)`.
- `orca_inverse_fit.py`: fits the model, prints the error figures and writes the GLSL.
