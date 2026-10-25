// ============================================================================
// risoprintEngine.ts — High-Fidelity Client-Side Risograph Simulation Engine
// Directly mirrors the C++ core architecture (Pass 1, Pass 2, Pass 3).
// ============================================================================

export interface ColorRGB {
  r: number; // 0..1
  g: number;
  b: number;
}

export interface SpotInkOption {
  name: string;
  color: string; // hex
}

export const SPOT_INKS_CATALOG: SpotInkOption[] = [
  { name: 'Custom', color: '#50D9F1' },
  { name: 'Black', color: '#231815' },
  { name: 'Blue', color: '#0078BF' },
  { name: 'Bright Red', color: '#F63819' },
  { name: 'Brown', color: '#8A5A2B' },
  { name: 'Burgundy', color: '#92203F' },
  { name: 'Crimson', color: '#E01B24' },
  { name: 'Federal Blue', color: '#425696' },
  { name: 'Flat Gold', color: '#C8A54B' },
  { name: 'Fluorescent Green', color: '#00FF66' },
  { name: 'Fluorescent Orange', color: '#FF7300' },
  { name: 'Fluorescent Pink', color: '#FF48A7' },
  { name: 'Fluorescent Red', color: '#FF3333' },
  { name: 'Fluorescent Yellow', color: '#FAFF00' },
  { name: 'Forest', color: '#2E5738' },
  { name: 'Granite', color: '#737373' },
  { name: 'Green', color: '#00A061' },
  { name: 'Hunter Green', color: '#406148' },
  { name: 'Indigo', color: '#485778' },
  { name: 'Ivy', color: '#16755C' },
  { name: 'Lake', color: '#237DA3' },
  { name: 'Light Gray', color: '#B4B4B4' },
  { name: 'Light Lime', color: '#E0F04B' },
  { name: 'Light Mauve', color: '#BAA0BA' },
  { name: 'Light Teal', color: '#00B0AB' },
  { name: 'Marine Red', color: '#DD2A38' },
  { name: 'Medium Blue', color: '#3266B0' },
  { name: 'Melon', color: '#FF7043' },
  { name: 'Metallic Gold', color: '#AB8A40' },
  { name: 'Midnight', color: '#33425C' },
  { name: 'Mint', color: '#82D2A5' },
  { name: 'Orange', color: '#FF6E00' },
  { name: 'Orchid', color: '#BF5496' },
  { name: 'Paprika', color: '#EE5033' },
  { name: 'Pine', color: '#23503D' },
  { name: 'Plum', color: '#8A3361' },
  { name: 'Pumpkin', color: '#FF6F00' },
  { name: 'Purple', color: '#70338A' },
  { name: 'Raisin', color: '#73384B' },
  { name: 'Raspberry', color: '#CF105D' },
  { name: 'Red', color: '#FF3828' },
  { name: 'Scarlet', color: '#F02030' },
  { name: 'Sea Blue', color: '#0068A3' },
  { name: 'Seafoam', color: '#61BFAD' },
  { name: 'Sky Blue', color: '#4BB0E0' },
  { name: 'Slate', color: '#616E78' },
  { name: 'Smoky Teal', color: '#5E8687' },
  { name: 'Spruce', color: '#476B61' },
  { name: 'Steel', color: '#4B7591' },
  { name: 'Sunflower', color: '#FFC400' },
  { name: 'Sunshine', color: '#FFDC00' },
  { name: 'Teal', color: '#00838A' },
  { name: 'Turquoise', color: '#00A7B8' },
  { name: 'Violet', color: '#A042A8' },
  { name: 'Wine', color: '#913040' },
  { name: 'Yellow', color: '#FFE900' },
];

export interface RisoParams {
  // Top Level
  shuffleSeed: number;
  pressPreset: number;

  // Inspect
  plateProof: number; // 1=Off, 2=Dot Mask, 3=Cyan, 4=Magenta, 5=Yellow, 6=Black, 7=4-Up
  magnifyZoom: number;
  magnifyCenterX: number;
  magnifyCenterY: number;
  cyanPlateEn: boolean;
  magentaPlateEn: boolean;
  yellowPlateEn: boolean;
  blackPlateEn: boolean;
  haloHalfRes: boolean;

  // Machine & Paper
  machineProfile: number;
  printResolution: number;
  paperType: number; // 1=Custom, 2=Newsprint, 3=Cotton Rag, 4=Kraft, 5=Papyrus, 6=Smooth
  backgroundMode: number; // 1=Paper, 2=Transparent, 3=Original
  spotInkKit: number;
  workingSpace: number; // 1=Passthrough, 2=Linearize

  // Color Correction
  brightness: number; // -100..100
  contrast: number; // -100..100
  hue: number; // -180..180
  saturation: number; // -100..100
  hsvValue: number; // -100..100
  gamma: number; // 0.1..3.0

  // Ink Palette
  colorMode: number; // 1=CMYK, 2=Spot Riso
  spotSeparation: number; // 1=Greedy, 2=Balanced Mix
  lineArtPlate: number; // 1=Off, 2=K, 3=Plate 1..4
  lineArtThreshold: number;
  lineArtStrength: number;
  activePlates: number; // 2..4
  coverage: number; // 0..200
  inkCyan: string;
  inkMagenta: string;
  inkYellow: string;
  inkBlack: string;
  spotInk1: number;
  spotInk2: number;
  spotInk3: number;
  spotInk4: number;
  customSpotInks: string[];
  blackBoost: number;
  risoTint: number;
  overprintMode: number; // 1=Multiply, 2=Beer-Lambert
  cyanTranslucency: number;
  magentaTranslucency: number;
  yellowTranslucency: number;
  blackTranslucency: number;
  totalCoverageLimit: number; // 100..400
  inkPooling: number;
  haloRadius: number;
  haloStrength: number;
  cyanCurve: number;
  magentaCurve: number;
  yellowCurve: number;
  blackCurve: number;

  // Halftone
  screenType: number; // 1=Voronoi, 2=Bayer, 3=Blue-noise, 4=Line, 5=Stippled, 6=Atkinson, 7=Floyd
  screenDensity: number; // 10..600
  dotShape: number; // 1=Circle, 2=Ellipse, 3=Diamond, 4=Square
  dotHardness: number; // 0..200
  softness: number; // 0..100
  distortion: number; // 0..100
  dotStretch: number; // -100..100
  edgeSoftness: number; // 0..100
  dotGain: number; // 0.5..3.0
  angleCyan: number;
  angleMagenta: number;
  angleYellow: number;
  angleBlack: number;
  weightCyan: number;
  weightMagenta: number;
  weightYellow: number;
  weightBlack: number;
  drumAnimSpeed: number;
  richBlacks: number;

  // Plate Alignment
  allOffsetX: number;
  allOffsetY: number;
  allRotation: number;
  allSkew: number;
  cyanOffsetX: number;
  cyanOffsetY: number;
  magentaOffsetX: number;
  magentaOffsetY: number;
  yellowOffsetX: number;
  yellowOffsetY: number;
  blackOffsetX: number;
  blackOffsetY: number;
  cyanRotation: number;
  magentaRotation: number;
  yellowRotation: number;
  blackRotation: number;
  cyanSkew: number;
  magentaSkew: number;
  yellowSkew: number;
  blackSkew: number;

  // Paper & Grain
  paperColor: string;
  paperGrain: number;
  grainScale: number;
  sheetWarp: number;
  agingAmount: number;
  agingColor: string;

  // Print Wear
  sunBleach: number;
  bleachDirection: number;
  inkVariance: number;
  inkFlecks: number;
  fleckScale: number;
  inkBlots: number;
  rollerStreaks: number;
  streakFreq: number;
  streakAngle: number;
  ghostOffsetX: number;
  ghostOffsetY: number;
  ghostOpacity: number;
  inkReticulation: number;
  reticScale: number;
  impressions: number;
  impressionJitter: number;
  inkDrift: number;
  reversePrint: boolean;

  // Lighting
  lightIntensity: number;
  lightAngle: number;
  lightElevation: number;
  ambientLight: number;
  threePointLight: number;

  // Output
  outputGamma: number;
  outputQuality: number;
  pressFidelity: number;
  softProof: number;
  negativeOutput: boolean;
  blendOriginal: number; // 0% default (zero ghosting)
}

export const DEFAULT_PARAMS: RisoParams = {
  shuffleSeed: 0,
  pressPreset: 1,

  plateProof: 1,
  magnifyZoom: 1.0,
  magnifyCenterX: 50.0,
  magnifyCenterY: 50.0,
  cyanPlateEn: true,
  magentaPlateEn: true,
  yellowPlateEn: true,
  blackPlateEn: true,
  haloHalfRes: false,

  machineProfile: 1,
  printResolution: 1,
  paperType: 1,
  backgroundMode: 1,
  spotInkKit: 1,
  workingSpace: 1,

  brightness: 0.0,
  contrast: 0.0,
  hue: 0.0,
  saturation: 0.0,
  hsvValue: 0.0,
  gamma: 1.00,

  colorMode: 1, // CMYK
  spotSeparation: 1, // Greedy
  lineArtPlate: 1, // Off
  lineArtThreshold: 55.0,
  lineArtStrength: 100.0,
  activePlates: 4,
  coverage: 100.0,
  inkCyan: '#00A0E9',
  inkMagenta: '#E4007F',
  inkYellow: '#FFF100',
  inkBlack: '#231815',
  spotInk1: 1,
  spotInk2: 1,
  spotInk3: 1,
  spotInk4: 1,
  customSpotInks: ['#50D9F1', '#BF7700', '#2900FF', '#1A1A1A'],
  blackBoost: 100.0,
  risoTint: 0.0,
  overprintMode: 1, // Multiply
  cyanTranslucency: 85.0,
  magentaTranslucency: 85.0,
  yellowTranslucency: 85.0,
  blackTranslucency: 85.0,
  totalCoverageLimit: 300.0,
  inkPooling: 0.0,
  haloRadius: 0.0,
  haloStrength: 0.0,
  cyanCurve: 1.00,
  magentaCurve: 1.00,
  yellowCurve: 1.00,
  blackCurve: 1.00,

  screenType: 1, // Voronoi
  screenDensity: 200.0,
  dotShape: 1, // Circle
  dotHardness: 110.0,
  softness: 23.0,
  distortion: 0.0,
  dotStretch: 0.0,
  edgeSoftness: 30.0,
  dotGain: 1.00,
  angleCyan: 15.0,
  angleMagenta: 75.0,
  angleYellow: 0.0,
  angleBlack: 45.0,
  weightCyan: 128.0,
  weightMagenta: 109.0,
  weightYellow: 100.0,
  weightBlack: 112.0,
  drumAnimSpeed: 0.0,
  richBlacks: 0.0,

  allOffsetX: 0.0,
  allOffsetY: 0.0,
  allRotation: 0.0,
  allSkew: 0.0,
  cyanOffsetX: 0.0,
  cyanOffsetY: 0.0,
  magentaOffsetX: 0.0,
  magentaOffsetY: 0.0,
  yellowOffsetX: 0.0,
  yellowOffsetY: 0.0,
  blackOffsetX: 0.0,
  blackOffsetY: 0.0,
  cyanRotation: 0.0,
  magentaRotation: 0.0,
  yellowRotation: 0.0,
  blackRotation: 0.0,
  cyanSkew: 0.0,
  magentaSkew: 0.0,
  yellowSkew: 0.0,
  blackSkew: 0.0,

  paperColor: '#FAF7F0',
  paperGrain: 20.0,
  grainScale: 50.0,
  sheetWarp: 15.0,
  agingAmount: 15.0,
  agingColor: '#8A5A2B',

  sunBleach: 0.0,
  bleachDirection: 0.0,
  inkVariance: 15.0,
  inkFlecks: 10.0,
  fleckScale: 50.0,
  inkBlots: 5.0,
  rollerStreaks: 0.0,
  streakFreq: 12.0,
  streakAngle: 90.0,
  ghostOffsetX: 0.0,
  ghostOffsetY: 0.0,
  ghostOpacity: 0.0,
  inkReticulation: 0.0,
  reticScale: 60.0,
  impressions: 1,
  impressionJitter: 0.0,
  inkDrift: 0.0,
  reversePrint: false,

  lightIntensity: 0.0,
  lightAngle: 45.0,
  lightElevation: 45.0,
  ambientLight: 50.0,
  threePointLight: 30.0,

  outputGamma: 1.30,
  outputQuality: 3, // High
  pressFidelity: 10.0,
  softProof: 1, // Off
  negativeOutput: false,
  blendOriginal: 0.0, // Zero ghosting
};

export type RenderMode =
  | 'composite'
  | 'dot_mask'
  | 'plate_c'
  | 'plate_m'
  | 'plate_y'
  | 'plate_k'
  | 'proof_4up'
  | 'bugged_simulation';

export function hexToRgb(hex: string): ColorRGB {
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

export function rgbToHex(c: ColorRGB): string {
  const r = Math.round(Math.max(0, Math.min(1, c.r)) * 255).toString(16).padStart(2, '0');
  const g = Math.round(Math.max(0, Math.min(1, c.g)) * 255).toString(16).padStart(2, '0');
  const b = Math.round(Math.max(0, Math.min(1, c.b)) * 255).toString(16).padStart(2, '0');
  return `#${r}${g}${b}`.toUpperCase();
}

function clamp(v: number, a = 0, b = 1): number {
  return Math.max(a, Math.min(b, v));
}

function smoothstep(e0: number, e1: number, x: number): number {
  const t = clamp((x - e0) / Math.max(e1 - e0, 1e-9));
  return t * t * (3 - 2 * t);
}

function mix(a: number, b: number, t: number): number {
  return a + (b - a) * t;
}

function luma(r: number, g: number, b: number): number {
  return 0.2126 * r + 0.7152 * g + 0.0722 * b;
}

// Procedural Hash and Noise
function risoHash(x: number, y: number): number {
  const dotResult = x * 127.1 + y * 311.7;
  const s = Math.sin(dotResult) * 43758.5453123;
  return s - Math.floor(s);
}

function risoNoise2D(x: number, y: number): number {
  const ix = Math.floor(x), iy = Math.floor(y);
  const fx = x - ix, fy = y - iy;
  const ux = fx * fx * (3 - 2 * fx), uy = fy * fy * (3 - 2 * fy);
  const a = risoHash(ix, iy);
  const b = risoHash(ix + 1, iy);
  const c = risoHash(ix, iy + 1);
  const d = risoHash(ix + 1, iy + 1);
  return mix(mix(a, b, ux), mix(c, d, ux), uy);
}

function risoFBM(x: number, y: number, octaves = 3): number {
  let val = 0;
  let amp = 0.5;
  let qx = x, qy = y;
  for (let i = 0; i < octaves; i++) {
    val += amp * risoNoise2D(qx, qy);
    qx *= 2.0;
    qy *= 2.0;
    amp *= 0.5;
  }
  return val;
}

const BAYER_8X8 = [
  [0 / 64, 32 / 64, 8 / 64, 40 / 64, 2 / 64, 34 / 64, 10 / 64, 42 / 64],
  [48 / 64, 16 / 64, 56 / 64, 24 / 64, 50 / 64, 18 / 64, 58 / 64, 26 / 64],
  [12 / 64, 44 / 64, 4 / 64, 36 / 64, 14 / 64, 46 / 64, 6 / 64, 38 / 64],
  [60 / 64, 28 / 64, 52 / 64, 20 / 64, 62 / 64, 30 / 64, 54 / 64, 22 / 64],
  [3 / 64, 35 / 64, 11 / 64, 43 / 64, 1 / 64, 33 / 64, 9 / 64, 41 / 64],
  [51 / 64, 19 / 64, 59 / 64, 27 / 64, 49 / 64, 17 / 64, 57 / 64, 25 / 64],
  [15 / 64, 47 / 64, 7 / 64, 39 / 64, 13 / 64, 45 / 64, 5 / 64, 37 / 64],
  [63 / 64, 31 / 64, 55 / 64, 23 / 64, 61 / 64, 29 / 64, 53 / 64, 21 / 64],
];

function cellDist(dx: number, dy: number, shape: number): number {
  if (shape === 2) { // Ellipse
    return Math.sqrt(dx * dx + (dy * 1.5) * (dy * 1.5));
  } else if (shape === 3) { // Diamond
    return Math.abs(dx) + Math.abs(dy);
  } else if (shape === 4) { // Square
    return Math.max(Math.abs(dx), Math.abs(dy));
  }
  return Math.sqrt(dx * dx + dy * dy);
}

function voronoiUnionDot(
  gx: number,
  gy: number,
  shape: number,
  randomness: number,
  channelVal: number,
  intensity: number,
  smoothness: number
): number {
  const ix = Math.floor(gx), iy = Math.floor(gy);
  let bestDist = 10.0;
  for (let j = -1; j <= 1; j++) {
    for (let i = -1; i <= 1; i++) {
      const nx = ix + i, ny = iy + j;
      const hx = risoHash(nx, ny);
      const hy = risoHash(nx + 13.7, ny + 7.1);
      const cx = nx + mix(0.5, hx, randomness);
      const cy = ny + mix(0.5, hy, randomness);
      const d = cellDist(gx - cx, gy - cy, shape);
      if (d < bestDist) bestDist = d;
    }
  }

  const adjusted = clamp(channelVal);
  const sat = smoothstep(0.7, 1.0, adjusted);
  const effI = intensity * mix(1.0, 0.55, sat);
  const len = bestDist * effI * 1.732;
  const r = Math.max(1.0 - len, 0.0);
  const inverted = 1.0 - r * r;

  const swidth = smoothness * smoothness * 0.4;
  const th = (swidth < 1e-4)
    ? (adjusted >= inverted ? 1.0 : 0.0)
    : smoothstep(inverted - swidth, inverted + swidth, adjusted);

  const satFloor = smoothstep(0.92, 0.99, adjusted);
  return mix(th, 1.0, satFloor);
}

// ---------------------------------------------------------------------------
// Render Function
// ---------------------------------------------------------------------------

export function renderRisograph(
  srcData: ImageData,
  params: RisoParams,
  timeSec = 0,
  mode: RenderMode = 'composite'
): ImageData {
  const w = srcData.width;
  const h = srcData.height;
  const rawSrc = srcData.data;
  const outData = new ImageData(w, h);
  const out = outData.data;

  const isBugged = mode === 'bugged_simulation';

  // Apply Presets to copy of params
  const p = isBugged
    ? {
        ...params,
        allOffsetX: -38.0,
        cyanOffsetX: 45.0,
        magentaOffsetX: -60.0,
        blendOriginal: 40.0,
        inkCyan: '#111111',
      }
    : { ...params };

  // Spot / CMYK Ink Colors
  const inkC = hexToRgb(p.inkCyan);
  const inkM = hexToRgb(p.inkMagenta);
  const inkY = hexToRgb(p.inkYellow);
  const inkK = hexToRgb(p.inkBlack);

  const activeInks: ColorRGB[] = (p.colorMode === 2)
    ? p.customSpotInks.slice(0, 4).map(hexToRgb)
    : [inkC, inkM, inkY, inkK];

  const paperBase = hexToRgb(p.paperColor);
  const agingColor = hexToRgb(p.agingColor);

  const angles = [
    p.angleCyan * (Math.PI / 180),
    p.angleMagenta * (Math.PI / 180),
    p.angleYellow * (Math.PI / 180),
    p.angleBlack * (Math.PI / 180),
  ];

  const plateRot = [
    (p.allRotation + p.cyanRotation) * (Math.PI / 180),
    (p.allRotation + p.magentaRotation) * (Math.PI / 180),
    (p.allRotation + p.yellowRotation) * (Math.PI / 180),
    (p.allRotation + p.blackRotation) * (Math.PI / 180),
  ];

  const offX = [
    p.allOffsetX + p.cyanOffsetX,
    p.allOffsetX + p.magentaOffsetX,
    p.allOffsetX + p.yellowOffsetX,
    p.allOffsetX + p.blackOffsetX,
  ];

  const offY = [
    p.allOffsetY + p.cyanOffsetY,
    p.allOffsetY + p.magentaOffsetY,
    p.allOffsetY + p.yellowOffsetY,
    p.allOffsetY + p.blackOffsetY,
  ];

  const plateEn = [
    p.cyanPlateEn,
    p.magentaPlateEn,
    p.yellowPlateEn,
    p.blackPlateEn,
  ];

  const plateWeights = [
    p.weightCyan / 100,
    p.weightMagenta / 100,
    p.weightYellow / 100,
    p.weightBlack / 100,
  ];

  const zoom = Math.max(1.0, p.magnifyZoom);
  const magCenterX = p.magnifyCenterX * 0.01;
  const magCenterY = p.magnifyCenterY * 0.01;

  for (let y = 0; y < h; y++) {
    for (let x = 0; x < w; x++) {
      let uvX = (x + 0.5) / w;
      let uvY = (y + 0.5) / h;

      if (zoom > 1.001) {
        uvX = magCenterX + (uvX - 0.5) / zoom;
        uvY = magCenterY + (uvY - 0.5) / zoom;
      }

      const sampleX = Math.max(0, Math.min(w - 1, Math.floor(uvX * w)));
      const sampleY = Math.max(0, Math.min(h - 1, Math.floor(uvY * h)));
      const srcIdx = (sampleY * w + sampleX) * 4;

      let r = rawSrc[srcIdx] / 255;
      let g = rawSrc[srcIdx + 1] / 255;
      let b = rawSrc[srcIdx + 2] / 255;
      const a = rawSrc[srcIdx + 3] / 255;

      // Color Adjustments
      if (Math.abs(p.brightness) > 0.1 || Math.abs(p.contrast) > 0.1) {
        const cFac = 1.0 + p.contrast * 0.01;
        const bOff = p.brightness * 0.01;
        r = clamp((r - 0.5) * cFac + 0.5 + bOff);
        g = clamp((g - 0.5) * cFac + 0.5 + bOff);
        b = clamp((b - 0.5) * cFac + 0.5 + bOff);
      }

      if (Math.abs(p.gamma - 1.0) > 0.01) {
        const invG = 1.0 / Math.max(0.1, p.gamma);
        r = Math.pow(r, invG);
        g = Math.pow(g, invG);
        b = Math.pow(b, invG);
      }

      // Plate Separations (C, M, Y, K)
      const cmyR = 1.0 - r;
      const cmyG = 1.0 - g;
      const cmyB = 1.0 - b;
      const k = Math.min(cmyR, Math.min(cmyG, cmyB));

      let sepC = 0, sepM = 0, sepY = 0, sepK = 0;
      if (p.colorMode === 2) {
        // Spot Separation (Greedy)
        sepC = clamp(cmyR);
        sepM = clamp(cmyG);
        sepY = clamp(cmyB);
        sepK = clamp(k * (p.blackBoost * 0.01));
      } else {
        // CMYK Mode
        const invK = k < 0.999 ? 1.0 / (1.0 - k) : 0;
        const enrich = p.richBlacks * 0.01;
        sepC = ((cmyR - k) * invK) * (1.0 - enrich) + cmyR * enrich;
        sepM = ((cmyG - k) * invK) * (1.0 - enrich) + cmyG * enrich;
        sepY = ((cmyB - k) * invK) * (1.0 - enrich) + cmyB * enrich;
        sepK = k * (1.0 - enrich) * (p.blackBoost * 0.01);
      }

      // Halftone dot generation per plate
      const chVals = [sepC, sepM, sepY, sepK];
      const chDots = [0, 0, 0, 0];

      const freq = p.screenDensity;
      const cellPx = h / Math.max(freq, 1.0);
      const bayerStep = Math.max(cellPx / 8.0, 1.0);

      for (let ch = 0; ch < 4; ch++) {
        if (!plateEn[ch]) continue;

        const rawVal = clamp(chVals[ch] * plateWeights[ch] * (p.coverage * 0.01));

        // Screen Type branch
        if (p.screenType === 2) {
          // Bayer 8x8
          const px = x + offX[ch];
          const py = y + offY[ch];
          const bx = Math.floor(px / bayerStep) % 8;
          const by = Math.floor(py / bayerStep) % 8;
          const th = BAYER_8X8[(by + 8) % 8][(bx + 8) % 8];
          chDots[ch] = rawVal >= (1.0 - th) ? 1.0 : 0.0;
        } else if (p.screenType === 4) {
          // Line Screen
          const ang = angles[ch];
          const coord = (x + offX[ch]) * Math.cos(ang) + (y + offY[ch]) * Math.sin(ang);
          const th = 0.5 + 0.5 * Math.sin((coord / cellPx) * Math.PI * 2);
          chDots[ch] = rawVal >= (1.0 - th) ? 1.0 : 0.0;
        } else if (p.screenType === 5) {
          // Stippled
          const th = risoHash((x + offX[ch]) * 0.8, (y + offY[ch]) * 0.8);
          chDots[ch] = rawVal >= (1.0 - th) ? 1.0 : 0.0;
        } else {
          // Voronoi Union Dot (Screen Type 1)
          const ca = Math.cos(angles[ch] + plateRot[ch]);
          const sa = Math.sin(angles[ch] + plateRot[ch]);
          const px = x - w * 0.5 + offX[ch];
          const py = y - h * 0.5 + offY[ch];
          const rx = px * ca - py * sa;
          const ry = px * sa + py * ca;
          const scale = freq / Math.max(h, 1.0);
          const gx = rx * scale;
          const gy = ry * scale * (1.0 + p.dotStretch * 0.01);

          chDots[ch] = voronoiUnionDot(
            gx, gy,
            p.dotShape,
            0.35,
            rawVal,
            p.dotHardness * 0.01,
            p.softness * 0.01
          );
        }

        // Dot gain
        if (p.dotGain > 1.001 && chDots[ch] > 0) {
          chDots[ch] = Math.pow(chDots[ch], 1.0 / p.dotGain);
        }
      }

      // Paper & Grain
      let paperR = paperBase.r;
      let paperG = paperBase.g;
      let paperB = paperBase.b;

      if (p.paperGrain > 0.1) {
        const gScale = Math.max(1.0, p.grainScale);
        let gNoise = risoFBM(uvX * gScale, uvY * gScale, 3);
        if (p.paperType === 4) { // Kraft
          gNoise += 0.3 * Math.sin(uvY * 300);
        }
        const grainFac = p.paperGrain * 0.01 * (gNoise - 0.5) * 0.3;
        paperR = clamp(paperR + grainFac);
        paperG = clamp(paperG + grainFac);
        paperB = clamp(paperB + grainFac);
      }

      // Aging
      if (p.agingAmount > 0.1) {
        const edgeDist = Math.min(Math.min(uvX, 1.0 - uvX), Math.min(uvY, 1.0 - uvY));
        const vignette = smoothstep(0.25, 0.0, edgeDist);
        const ageFac = p.agingAmount * 0.01 * (0.4 + 0.6 * vignette);
        paperR = mix(paperR, agingColor.r, ageFac);
        paperG = mix(paperG, agingColor.g, ageFac);
        paperB = mix(paperB, agingColor.b, ageFac);
      }

      // Composite Overprint
      let compR = paperR;
      let compG = paperG;
      let compB = paperB;

      if (p.overprintMode === 2) {
        // Beer-Lambert Exponential Absorption
        const transC = p.cyanTranslucency * 0.01;
        const transM = p.magentaTranslucency * 0.01;
        const transY = p.yellowTranslucency * 0.01;
        const transK = p.blackTranslucency * 0.01;
        const trans = [transC, transM, transY, transK];

        let optR = 0, optG = 0, optB = 0;
        for (let ch = 0; ch < 4; ch++) {
          if (!plateEn[ch] || chDots[ch] <= 1e-4) continue;
          const cov = chDots[ch];
          const al = trans[ch];
          optR += (1.0 - activeInks[ch].r) * cov * (1.2 - 0.2 * al);
          optG += (1.0 - activeInks[ch].g) * cov * (1.2 - 0.2 * al);
          optB += (1.0 - activeInks[ch].b) * cov * (1.2 - 0.2 * al);
        }
        compR = paperR * Math.exp(-optR);
        compG = paperG * Math.exp(-optG);
        compB = paperB * Math.exp(-optB);
      } else {
        // Subtractive Multiply
        for (let ch = 0; ch < 4; ch++) {
          if (!plateEn[ch] || chDots[ch] <= 1e-4) continue;
          const cov = chDots[ch];
          const ink = activeInks[ch];
          compR = mix(compR, compR * ink.r, cov);
          compG = mix(compG, compG * ink.g, cov);
          compB = mix(compB, compB * ink.b, cov);
        }
      }

      // Output Gamma
      if (Math.abs(p.outputGamma - 1.0) > 0.01) {
        const invOutG = 1.0 / Math.max(0.1, p.outputGamma);
        compR = Math.pow(clamp(compR), invOutG);
        compG = Math.pow(clamp(compG), invOutG);
        compB = Math.pow(clamp(compB), invOutG);
      }

      // Reverse Print / Negative
      if (p.reversePrint || p.negativeOutput) {
        compR = 1.0 - compR;
        compG = 1.0 - compG;
        compB = 1.0 - compB;
      }

      // Proof Modes
      let finalR = compR, finalG = compG, finalB = compB;
      const proof = (mode !== 'composite' && mode !== 'bugged_simulation')
        ? (mode === 'dot_mask' ? 2 : mode === 'plate_c' ? 3 : mode === 'plate_m' ? 4 : mode === 'plate_y' ? 5 : mode === 'plate_k' ? 6 : 7)
        : p.plateProof;

      if (proof === 2) {
        const m = Math.max(Math.max(chDots[0], chDots[1]), Math.max(chDots[2], chDots[3]));
        finalR = 1.0 - m; finalG = 1.0 - m; finalB = 1.0 - m;
      } else if (proof === 3) {
        finalR = mix(1.0, activeInks[0].r, chDots[0]);
        finalG = mix(1.0, activeInks[0].g, chDots[0]);
        finalB = mix(1.0, activeInks[0].b, chDots[0]);
      } else if (proof === 4) {
        finalR = mix(1.0, activeInks[1].r, chDots[1]);
        finalG = mix(1.0, activeInks[1].g, chDots[1]);
        finalB = mix(1.0, activeInks[1].b, chDots[1]);
      } else if (proof === 5) {
        finalR = mix(1.0, activeInks[2].r, chDots[2]);
        finalG = mix(1.0, activeInks[2].g, chDots[2]);
        finalB = mix(1.0, activeInks[2].b, chDots[2]);
      } else if (proof === 6) {
        finalR = mix(1.0, activeInks[3].r, chDots[3]);
        finalG = mix(1.0, activeInks[3].g, chDots[3]);
        finalB = mix(1.0, activeInks[3].b, chDots[3]);
      } else if (proof === 7) {
        const qx = uvX < 0.5 ? 0 : 1;
        const qy = uvY < 0.5 ? 0 : 1;
        const pIdx = qy === 0 ? (qx === 0 ? 0 : 1) : (qx === 0 ? 2 : 3);
        const guidePx = 1.0 / Math.max(w, h);
        if (Math.abs(uvX - 0.5) < guidePx || Math.abs(uvY - 0.5) < guidePx) {
          finalR = 0.2; finalG = 0.2; finalB = 0.2;
        } else {
          finalR = mix(1.0, activeInks[pIdx].r, chDots[pIdx]);
          finalG = mix(1.0, activeInks[pIdx].g, chDots[pIdx]);
          finalB = mix(1.0, activeInks[pIdx].b, chDots[pIdx]);
        }
      }

      // Blend With Original (Bypassed when 0%)
      if (p.blendOriginal > 0.01) {
        const keep = p.blendOriginal * 0.01;
        finalR = mix(finalR, r, keep);
        finalG = mix(finalG, g, keep);
        finalB = mix(finalB, b, keep);
      }

      const outIdx = (y * w + x) * 4;
      out[outIdx] = Math.round(clamp(finalR) * 255);
      out[outIdx + 1] = Math.round(clamp(finalG) * 255);
      out[outIdx + 2] = Math.round(clamp(finalB) * 255);
      out[outIdx + 3] = Math.round(a * 255);
    }
  }

  return outData;
}
