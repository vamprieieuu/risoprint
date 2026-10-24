export interface Color {
  r: number; // 0..1
  g: number;
  b: number;
}

export interface InkParams {
  enabled: boolean;
  color: string; // hex
  separation: number; // 0..1
  shiftX: number; // -0.05 .. +0.05
  shiftY: number; // -0.05 .. +0.05
}

export interface RisoParams {
  intensity: number; // 0..100
  blend: number; // 1..11
  showSource: boolean;
  paper: string; // hex
  inkContrast: number; // -1..1
  scale: number; // 0.03..1
  opacity: number; // 0..1
  softness: number; // 0..1
  contrast: number; // 0.25..3
  blendOriginal: number; // 0..100
  inks: [InkParams, InkParams, InkParams];
}

export const BLEND_MODES = [
  { id: 1, name: 'Normal (Subtractive Print)' },
  { id: 2, name: 'Darken' },
  { id: 3, name: 'Multiply' },
  { id: 4, name: 'Color Burn' },
  { id: 5, name: 'Lighten' },
  { id: 6, name: 'Screen' },
  { id: 7, name: 'Plus Lighter' },
  { id: 8, name: 'Color Dodge' },
  { id: 9, name: 'Overlay' },
  { id: 10, name: 'Soft Light' },
  { id: 11, name: 'Hard Light' },
];

export const DEFAULT_PARAMS: RisoParams = {
  intensity: 100,
  blend: 1, // Normal
  showSource: false,
  paper: '#FFFFFF',
  inkContrast: -0.33,
  scale: 0.21,
  opacity: 0.65,
  softness: 0.50,
  contrast: 1.58,
  blendOriginal: 0,
  inks: [
    {
      enabled: true,
      color: '#50D9F1',
      separation: 0.38,
      shiftX: 0.003,
      shiftY: 0.002,
    },
    {
      enabled: true,
      color: '#BF7700',
      separation: 0.64,
      shiftX: 0.000,
      shiftY: -0.001,
    },
    {
      enabled: true,
      color: '#2900FF',
      separation: 0.39,
      shiftX: 0.000,
      shiftY: -0.003,
    },
  ],
};

export function hexToRgb(hex: string): Color {
  let clean = hex.replace('#', '');
  if (clean.length === 3) {
    clean = clean.split('').map((c) => c + c).join('');
  }
  const intVal = parseInt(clean, 16) || 0;
  return {
    r: ((intVal >> 16) & 255) / 255,
    g: ((intVal >> 8) & 255) / 255,
    b: (intVal & 255) / 255,
  };
}

export function rgbToHex(c: Color): string {
  const r = Math.round(Math.max(0, Math.min(1, c.r)) * 255).toString(16).padStart(2, '0');
  const g = Math.round(Math.max(0, Math.min(1, c.g)) * 255).toString(16).padStart(2, '0');
  const b = Math.round(Math.max(0, Math.min(1, c.b)) * 255).toString(16).padStart(2, '0');
  return `#${r}${g}${b}`.toUpperCase();
}

function clamp(v: number, a = 0, b = 1): number {
  return Math.max(a, Math.min(b, v));
}

function smoothstep(a: number, b: number, x: number): number {
  if (Math.abs(b - a) < 1e-6) return x >= a ? 1 : 0;
  const t = clamp((x - a) / (b - a));
  return t * t * (3 - 2 * t);
}

function luma(r: number, g: number, b: number): number {
  return 0.2126 * r + 0.7152 * g + 0.0722 * b;
}

function hashU32(x: number): number {
  x = Math.imul(x ^ (x >>> 16), 0x7feb352d);
  x = Math.imul(x ^ (x >>> 15), 0x846ca68b);
  x ^= x >>> 16;
  return x >>> 0;
}

function noise01(x: number, y: number, seed = 0): number {
  const n = (Math.imul(x, 374761393) ^ Math.imul(y, 668265263) ^ Math.imul(seed, 2246822519)) >>> 0;
  return (hashU32(n) & 0x00ffffff) / 16777216;
}

function sampleBilinear(
  pixels: Float32Array,
  w: number,
  h: number,
  fx: number,
  fy: number
): { r: number; g: number; b: number; a: number } {
  fx = Math.max(0, Math.min(w - 1, fx));
  fy = Math.max(0, Math.min(h - 1, fy));
  const x0 = Math.floor(fx);
  const y0 = Math.floor(fy);
  const x1 = Math.min(w - 1, x0 + 1);
  const y1 = Math.min(h - 1, y0 + 1);
  const tx = fx - x0;
  const ty = fy - y0;

  const idx00 = (y0 * w + x0) * 4;
  const idx10 = (y0 * w + x1) * 4;
  const idx01 = (y1 * w + x0) * 4;
  const idx11 = (y1 * w + x1) * 4;

  const r0 = pixels[idx00] + (pixels[idx10] - pixels[idx00]) * tx;
  const r1 = pixels[idx01] + (pixels[idx11] - pixels[idx01]) * tx;
  const r = r0 + (r1 - r0) * ty;

  const g0 = pixels[idx00 + 1] + (pixels[idx10 + 1] - pixels[idx00 + 1]) * tx;
  const g1 = pixels[idx01 + 1] + (pixels[idx11 + 1] - pixels[idx01 + 1]) * tx;
  const g = g0 + (g1 - g0) * ty;

  const b0 = pixels[idx00 + 2] + (pixels[idx10 + 2] - pixels[idx00 + 2]) * tx;
  const b1 = pixels[idx01 + 2] + (pixels[idx11 + 2] - pixels[idx01 + 2]) * tx;
  const b = b0 + (b1 - b0) * ty;

  const a0 = pixels[idx00 + 3] + (pixels[idx10 + 3] - pixels[idx00 + 3]) * tx;
  const a1 = pixels[idx01 + 3] + (pixels[idx11 + 3] - pixels[idx01 + 3]) * tx;
  const a = a0 + (a1 - a0) * ty;

  return { r, g, b, a };
}

function sampleSoft(
  pixels: Float32Array,
  w: number,
  h: number,
  fx: number,
  fy: number,
  radius: number
): { r: number; g: number; b: number } {
  if (radius < 0.35) {
    const s = sampleBilinear(pixels, w, h, fx, fy);
    return { r: s.r, g: s.g, b: s.b };
  }
  let rSum = 0;
  let gSum = 0;
  let bSum = 0;
  let wSum = 0;
  const r = Math.ceil(radius * 1.5);
  const sigma = Math.max(0.5, radius * 0.65);
  const twoSigmaSq = 2 * sigma * sigma;

  for (let j = -r; j <= r; j++) {
    for (let i = -r; i <= r; i++) {
      const d2 = i * i + j * j;
      const weight = Math.exp(-d2 / twoSigmaSq);
      const s = sampleBilinear(pixels, w, h, fx + i, fy + j);
      rSum += s.r * weight;
      gSum += s.g * weight;
      bSum += s.b * weight;
      wSum += weight;
    }
  }

  return {
    r: wSum > 0 ? rSum / wSum : 0,
    g: wSum > 0 ? gSum / wSum : 0,
    b: wSum > 0 ? bSum / wSum : 0,
  };
}

function hueAffinity(s: Color, ink: Color): number {
  const mx = Math.max(s.r, s.g, s.b);
  const mn = Math.min(s.r, s.g, s.b);
  const d = mx - mn;
  if (d < 1e-5) return 1.0;

  let h = 0;
  if (mx === s.r) {
    h = (((s.g - s.b) / d) % 6) / 6;
  } else if (mx === s.g) {
    h = ((s.b - s.r) / d + 2) / 6;
  } else {
    h = ((s.r - s.g) / d + 4) / 6;
  }
  if (h < 0) h += 1.0;

  const imx = Math.max(ink.r, ink.g, ink.b);
  const imn = Math.min(ink.r, ink.g, ink.b);
  const id = imx - imn;
  let ih = 0;
  if (id > 1e-5) {
    if (imx === ink.r) {
      ih = (((ink.g - ink.b) / id) % 6) / 6;
    } else if (imx === ink.g) {
      ih = ((ink.b - ink.r) / id + 2) / 6;
    } else {
      ih = ((ink.r - ink.g) / id + 4) / 6;
    }
    if (ih < 0) ih += 1.0;
  }

  let hd = Math.abs(h - ih);
  hd = Math.min(hd, 1.0 - hd);
  const a = clamp(1.0 - hd / 0.33);
  return a * a * (3.0 - 2.0 * a);
}

function grain(x: number, y: number, scale: number, t: number): number {
  const s = Math.max(0.035, scale);
  const gx = Math.floor(x / (s * 5.5));
  const gy = Math.floor(y / (s * 5.5));
  const coarse = noise01(gx, gy, Math.floor(t * 12.0));
  const fine = noise01(x, y, Math.floor(t * 24.0));
  const mx = Math.floor(x / (s * 1.7));
  const my = Math.floor(y / (s * 1.7));
  const mid = noise01(mx, my, 17);
  return clamp(0.16 * coarse + 0.58 * fine + 0.26 * mid);
}

function plateDensity(
  s: Color,
  ink: Color,
  sep: number,
  inkContrast: number,
  contrast: number
): number {
  const lum = luma(s.r, s.g, s.b);
  const darkness = clamp(1.0 - lum);
  const sat = clamp(Math.max(s.r, s.g, s.b) - Math.min(s.r, s.g, s.b));
  const affinity = hueAffinity(s, ink);

  const selective = sat < 0.1 ? 1.0 : 0.28 + 0.72 * Math.pow(affinity, 2.2);
  let d = darkness * (0.48 + 0.52 * selective) + sat * 0.35 * selective;

  const c = Math.max(0.25, Math.min(3.0, contrast));
  d = clamp((d - 0.5) * c + 0.5);

  const sepv = Math.max(0.0, Math.min(1.0, sep));
  const lo = 0.38 - sepv * 0.28;
  const hi = 0.86 - sepv * 0.22;
  d = smoothstep(lo, hi, d);

  const ic = Math.max(-1.0, Math.min(1.0, inkContrast));
  d = clamp((d - 0.5) * (1.0 + ic * 0.55) + 0.5);
  return d;
}

export type RenderMode = 'composite' | 'paper' | 'plate1' | 'plate2' | 'plate3' | 'bugged_simulation';

export function renderRisograph(
  srcData: ImageData,
  params: RisoParams,
  timeSec = 0,
  mode: RenderMode = 'composite'
): ImageData {
  const w = srcData.width;
  const h = srcData.height;
  const srcFloats = new Float32Array(w * h * 4);
  const rawSrc = srcData.data;

  for (let i = 0; i < rawSrc.length; i += 4) {
    srcFloats[i] = rawSrc[i] / 255;
    srcFloats[i + 1] = rawSrc[i + 1] / 255;
    srcFloats[i + 2] = rawSrc[i + 2] / 255;
    srcFloats[i + 3] = rawSrc[i + 3] / 255;
  }

  const outData = new ImageData(w, h);
  const outBytes = outData.data;

  // Check if bugged simulation is active
  const isBugged = mode === 'bugged_simulation';

  // In bugged mode:
  // - shiftX 1 is 0.38 (38% screen shift left!)
  // - shiftX 2 is 0.64 (64% screen shift left!)
  // - Inks are black/corrupted or read from checkboxes (0 values)
  // - Blend with original mixes in
  const p = isBugged
    ? {
        ...params,
        inks: [
          {
            enabled: true,
            color: '#111111', // Corrupted black/zero reading
            separation: 0.38,
            shiftX: 0.38, // Displaced sideways by 38%!
            shiftY: 0.003,
          },
          {
            enabled: true,
            color: '#222222',
            separation: 0.64,
            shiftX: 0.64, // Displaced sideways by 64%!
            shiftY: 0.0,
          },
          {
            enabled: false,
            color: '#000000',
            separation: 0.39,
            shiftX: 0.39,
            shiftY: -0.001,
          },
        ] as [InkParams, InkParams, InkParams],
        blendOriginal: 35, // Ghosting double exposure!
      }
    : params;

  if (p.showSource) {
    for (let i = 0; i < rawSrc.length; i++) {
      outBytes[i] = rawSrc[i];
    }
    return outData;
  }

  const intensity = clamp(p.intensity / 100);
  const softRadius = p.softness * 2.4;
  const grainScale = Math.max(0.03, p.scale);
  const grainVisibility = clamp(p.opacity);
  const paperColor = hexToRgb(p.paper);

  const inkColors: Color[] = p.inks.map((ink) => hexToRgb(ink.color));

  for (let y = 0; y < h; y++) {
    for (let x = 0; x < w; x++) {
      const pixelIdx = (y * w + x) * 4;

      if (mode === 'paper') {
        const bg = grain(x, y, grainScale, timeSec);
        const factor = 1.0 - (bg - 0.5) * 0.12 * grainVisibility;
        outBytes[pixelIdx] = Math.round(clamp(paperColor.r * factor) * 255);
        outBytes[pixelIdx + 1] = Math.round(clamp(paperColor.g * factor) * 255);
        outBytes[pixelIdx + 2] = Math.round(clamp(paperColor.b * factor) * 255);
        outBytes[pixelIdx + 3] = rawSrc[pixelIdx + 3];
        continue;
      }

      let accumR = paperColor.r;
      let accumG = paperColor.g;
      let accumB = paperColor.b;
      const baseGrain = grain(x, y, grainScale, timeSec);

      // Check single plate isolation
      let plateIndices = [0, 1, 2];
      if (mode === 'plate1') plateIndices = [0];
      else if (mode === 'plate2') plateIndices = [1];
      else if (mode === 'plate3') plateIndices = [2];

      for (const k of plateIndices) {
        const ip = p.inks[k];
        if (!ip.enabled) continue;

        const inkCol = inkColors[k];
        const sx = x + ip.shiftX * w;
        const sy = y + ip.shiftY * h;
        const s = sampleSoft(srcFloats, w, h, sx, sy, softRadius);

        const d = plateDensity(s, inkCol, ip.separation, p.inkContrast, p.contrast);
        const n = noise01(x + k * 4099, y + k * 7919, Math.floor(timeSec * 8.0));

        const grainTerm = (baseGrain - 0.5) * 0.52 + (n - 0.5) * 0.18;
        const thresholded = clamp(d + grainTerm * (0.42 + 0.58 * grainVisibility));

        let coverage = smoothstep(0.015, 0.78, thresholded);
        coverage *= 1.0 - grainVisibility * 0.34 * baseGrain;

        const a = clamp(coverage * intensity * (0.45 + 0.55 * grainVisibility) * 0.95);

        // Blending channels
        const blendChannel = (base: number, ink: number): number => {
          let mixed = ink;
          switch (p.blend) {
            case 2: // Darken
              mixed = Math.min(base, ink);
              break;
            case 3: // Multiply
              mixed = base * ink;
              break;
            case 4: // Color Burn
              mixed = ink <= 0.0001 ? 0.0 : 1.0 - Math.min(1.0, (1.0 - base) / ink);
              break;
            case 5: // Lighten
              mixed = Math.max(base, ink);
              break;
            case 6: // Screen
              mixed = 1.0 - (1.0 - base) * (1.0 - ink);
              break;
            case 7: // Plus Lighter
              mixed = Math.min(1.0, base + ink);
              break;
            case 8: // Color Dodge
              mixed = ink >= 0.9999 ? 1.0 : Math.min(1.0, base / (1.0 - ink));
              break;
            case 9: // Overlay
              mixed = base < 0.5 ? 2.0 * base * ink : 1.0 - 2.0 * (1.0 - base) * (1.0 - ink);
              break;
            case 10: // Soft Light
              mixed = (1.0 - 2.0 * ink) * base * base + 2.0 * ink * base;
              break;
            case 11: // Hard Light
              mixed = ink < 0.5 ? 2.0 * base * ink : 1.0 - 2.0 * (1.0 - base) * (1.0 - ink);
              break;
            default: // Normal (Subtractive ink absorb on paper)
              mixed = base * ink;
              break;
          }
          return clamp(base * (1.0 - a) + mixed * a);
        };

        accumR = blendChannel(accumR, inkCol.r);
        accumG = blendChannel(accumG, inkCol.g);
        accumB = blendChannel(accumB, inkCol.b);
      }

      const srcA = srcFloats[pixelIdx + 3];
      const keep = clamp(p.blendOriginal / 100);

      if (keep > 0.0001) {
        const sr = srcFloats[pixelIdx];
        const sg = srcFloats[pixelIdx + 1];
        const sb = srcFloats[pixelIdx + 2];
        outBytes[pixelIdx] = Math.round(clamp(accumR * (1.0 - keep) + sr * keep) * 255);
        outBytes[pixelIdx + 1] = Math.round(clamp(accumG * (1.0 - keep) + sg * keep) * 255);
        outBytes[pixelIdx + 2] = Math.round(clamp(accumB * (1.0 - keep) + sb * keep) * 255);
      } else {
        outBytes[pixelIdx] = Math.round(clamp(accumR) * 255);
        outBytes[pixelIdx + 1] = Math.round(clamp(accumG) * 255);
        outBytes[pixelIdx + 2] = Math.round(clamp(accumB) * 255);
      }
      outBytes[pixelIdx + 3] = Math.round(srcA * 255);
    }
  }

  return outData;
}
