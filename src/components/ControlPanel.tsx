import React, { useState } from 'react';
import {
  RisoParams,
  DEFAULT_PARAMS,
  RenderMode,
  SPOT_INKS_CATALOG,
} from '../engine/risoprintEngine';
import {
  RotateCcw,
  Sliders,
  Eye,
  Layers,
  Palette,
  Grid,
  Move,
  FileText,
  Activity,
  Sun,
  Monitor,
  ChevronDown,
  ChevronRight,
  Sparkles,
} from 'lucide-react';

interface ControlPanelProps {
  params: RisoParams;
  onChange: (params: RisoParams) => void;
  renderMode: RenderMode;
  setRenderMode: (mode: RenderMode) => void;
}

export const ControlPanel: React.FC<ControlPanelProps> = ({
  params,
  onChange,
  renderMode,
  setRenderMode,
}) => {
  // Accordion state
  const [openSections, setOpenSections] = useState<Record<string, boolean>>({
    inspect: true,
    machine: false,
    color: false,
    ink: true,
    halftone: true,
    alignment: false,
    paper: false,
    wear: false,
    lighting: false,
    output: false,
  });

  const toggleSection = (section: string) => {
    setOpenSections((prev) => ({ ...prev, [section]: !prev[section] }));
  };

  const updateParam = <K extends keyof RisoParams>(key: K, val: RisoParams[K]) => {
    onChange({ ...params, [key]: val });
  };

  const resetDefaults = () => {
    onChange({ ...DEFAULT_PARAMS });
    setRenderMode('composite');
  };

  return (
    <div className="w-full lg:w-[420px] flex-shrink-0 bg-zinc-900/90 border border-zinc-800 rounded-2xl p-4 flex flex-col gap-4 overflow-y-auto max-h-[calc(100vh-5rem)] shadow-2xl">
      {/* Header & Reset */}
      <div className="flex items-center justify-between pb-3 border-b border-zinc-800">
        <div className="flex items-center gap-2">
          <Sliders className="w-4 h-4 text-cyan-400" />
          <h2 className="text-sm font-bold text-white tracking-wide uppercase font-mono">
            RisoPrint Controls
          </h2>
        </div>

        <button
          onClick={resetDefaults}
          className="flex items-center gap-1.5 text-xs text-zinc-400 hover:text-white px-2.5 py-1 rounded-lg bg-zinc-800 hover:bg-zinc-700 transition"
          title="Reset all settings to native reference defaults"
        >
          <RotateCcw className="w-3 h-3 text-cyan-400" />
          Reset Defaults
        </button>
      </div>

      {/* Top Level: Shuffle Seed & Press Preset */}
      <div className="bg-zinc-950/60 p-3 rounded-xl border border-zinc-800/80 space-y-3">
        <div className="flex items-center justify-between text-xs">
          <span className="font-medium text-zinc-300">Shuffle Seed</span>
          <span className="font-mono text-cyan-400 font-bold">{params.shuffleSeed}</span>
        </div>
        <input
          type="range"
          min="0"
          max="1000"
          value={params.shuffleSeed}
          onChange={(e) => updateParam('shuffleSeed', parseInt(e.target.value))}
          className="w-full h-1.5 bg-zinc-800 rounded-lg appearance-none cursor-pointer"
        />

        <div className="space-y-1">
          <label className="text-xs font-medium text-zinc-300">Press Preset</label>
          <select
            value={params.pressPreset}
            onChange={(e) => {
              const val = parseInt(e.target.value);
              updateParam('pressPreset', val);
              if (val === 2) {
                onChange({ ...params, pressPreset: 2, screenType: 1, screenDensity: 220, softness: 30, paperGrain: 15, overprintMode: 2 });
              } else if (val === 3) {
                onChange({ ...params, pressPreset: 3, screenType: 2, screenDensity: 160, distortion: 25, paperType: 4, paperGrain: 35, inkFlecks: 20 });
              } else if (val === 4) {
                onChange({ ...params, pressPreset: 4, screenType: 1, screenDensity: 140, dotHardness: 180, contrast: 35, brightness: 10, lineArtStrength: 80 });
              } else if (val === 5) {
                onChange({ ...params, pressPreset: 5, screenType: 4, screenDensity: 130, paperType: 2, dotGain: 1.25, agingAmount: 30 });
              }
            }}
            className="w-full bg-zinc-900 border border-zinc-700 text-xs text-white rounded-lg px-2.5 py-1.5 focus:border-cyan-500 focus:outline-none"
          >
            <option value="1">Custom</option>
            <option value="2">Soft Studio</option>
            <option value="3">1970s Zine</option>
            <option value="4">Blown-out Punk</option>
            <option value="5">Newsprint</option>
            <option value="6">RisoFest 2-color</option>
            <option value="7">Letterpress Indigo</option>
            <option value="8">Bright Comic</option>
            <option value="9">Vintage Magazine</option>
            <option value="10">Midnight Mimeograph</option>
            <option value="11">Fluoro Shop Window</option>
            <option value="12">Library Card</option>
            <option value="13">Blueprint Proof</option>
            <option value="14">Sun-Baked Poster</option>
            <option value="15">Cotton Art Print</option>
          </select>
        </div>
      </div>

      {/* Accordion Group 1: Inspect */}
      <div className="border border-zinc-800 rounded-xl overflow-hidden bg-zinc-950/40">
        <button
          onClick={() => toggleSection('inspect')}
          className="w-full px-3.5 py-2.5 bg-zinc-900/80 flex items-center justify-between text-xs font-semibold text-zinc-200 hover:text-white"
        >
          <span className="flex items-center gap-2">
            <Eye className="w-3.5 h-3.5 text-cyan-400" />
            Inspect &amp; Plate Proof
          </span>
          {openSections.inspect ? <ChevronDown className="w-4 h-4 text-zinc-500" /> : <ChevronRight className="w-4 h-4 text-zinc-500" />}
        </button>

        {openSections.inspect && (
          <div className="p-3.5 space-y-3 text-xs border-t border-zinc-800/80">
            <div>
              <label className="text-zinc-400 block mb-1">Plate Proof Mode</label>
              <select
                value={params.plateProof}
                onChange={(e) => {
                  const val = parseInt(e.target.value);
                  updateParam('plateProof', val);
                  if (val === 1) setRenderMode('composite');
                  else if (val === 2) setRenderMode('dot_mask');
                  else if (val === 3) setRenderMode('plate_c');
                  else if (val === 4) setRenderMode('plate_m');
                  else if (val === 5) setRenderMode('plate_y');
                  else if (val === 6) setRenderMode('plate_k');
                  else if (val === 7) setRenderMode('proof_4up');
                }}
                className="w-full bg-zinc-900 border border-zinc-700 text-white rounded-lg px-2.5 py-1.5 focus:border-cyan-500 focus:outline-none"
              >
                <option value="1">Off (Composite)</option>
                <option value="2">Dot Mask Only</option>
                <option value="3">Cyan Separation</option>
                <option value="4">Magenta Separation</option>
                <option value="5">Yellow Separation</option>
                <option value="6">Black Separation</option>
                <option value="7">4-Up Proof Grid</option>
              </select>
            </div>

            <div className="grid grid-cols-2 gap-2">
              <div>
                <div className="flex justify-between text-[11px] mb-1">
                  <span className="text-zinc-400">Magnify Zoom (x)</span>
                  <span className="font-mono text-cyan-400">{params.magnifyZoom.toFixed(2)}x</span>
                </div>
                <input
                  type="range"
                  min="1"
                  max="5"
                  step="0.1"
                  value={params.magnifyZoom}
                  onChange={(e) => updateParam('magnifyZoom', parseFloat(e.target.value))}
                  className="w-full h-1 bg-zinc-800 rounded appearance-none"
                />
              </div>

              <div>
                <div className="flex justify-between text-[11px] mb-1">
                  <span className="text-zinc-400">Center X/Y (%)</span>
                  <span className="font-mono text-cyan-400">{params.magnifyCenterX.toFixed(0)}%</span>
                </div>
                <input
                  type="range"
                  min="0"
                  max="100"
                  value={params.magnifyCenterX}
                  onChange={(e) => {
                    updateParam('magnifyCenterX', parseFloat(e.target.value));
                    updateParam('magnifyCenterY', parseFloat(e.target.value));
                  }}
                  className="w-full h-1 bg-zinc-800 rounded appearance-none"
                />
              </div>
            </div>

            <div className="pt-2 border-t border-zinc-800/80">
              <span className="text-[11px] font-medium text-zinc-400 block mb-1.5">Active Plates</span>
              <div className="grid grid-cols-4 gap-1.5">
                {[
                  { key: 'cyanPlateEn', label: 'Cyan', color: '#00A0E9' },
                  { key: 'magentaPlateEn', label: 'Magenta', color: '#E4007F' },
                  { key: 'yellowPlateEn', label: 'Yellow', color: '#FFF100' },
                  { key: 'blackPlateEn', label: 'Black', color: '#231815' },
                ].map(({ key, label, color }) => (
                  <label
                    key={key}
                    className={`flex items-center gap-1.5 px-2 py-1 rounded border text-[11px] font-medium cursor-pointer transition ${
                      params[key as keyof RisoParams]
                        ? 'bg-zinc-800 border-zinc-600 text-white'
                        : 'bg-zinc-950 border-zinc-800 text-zinc-600'
                    }`}
                  >
                    <input
                      type="checkbox"
                      checked={!!params[key as keyof RisoParams]}
                      onChange={(e) => updateParam(key as keyof RisoParams, e.target.checked as any)}
                      className="sr-only"
                    />
                    <span className="w-2 h-2 rounded-full" style={{ backgroundColor: color }} />
                    {label}
                  </label>
                ))}
              </div>
            </div>
          </div>
        )}
      </div>

      {/* Accordion Group 2: Machine & Paper */}
      <div className="border border-zinc-800 rounded-xl overflow-hidden bg-zinc-950/40">
        <button
          onClick={() => toggleSection('machine')}
          className="w-full px-3.5 py-2.5 bg-zinc-900/80 flex items-center justify-between text-xs font-semibold text-zinc-200 hover:text-white"
        >
          <span className="flex items-center gap-2">
            <Monitor className="w-3.5 h-3.5 text-blue-400" />
            Machine &amp; Paper Type
          </span>
          {openSections.machine ? <ChevronDown className="w-4 h-4 text-zinc-500" /> : <ChevronRight className="w-4 h-4 text-zinc-500" />}
        </button>

        {openSections.machine && (
          <div className="p-3.5 space-y-3 text-xs border-t border-zinc-800/80">
            <div>
              <label className="text-zinc-400 block mb-1">Machine Profile</label>
              <select
                value={params.machineProfile}
                onChange={(e) => updateParam('machineProfile', parseInt(e.target.value))}
                className="w-full bg-zinc-900 border border-zinc-700 text-white rounded-lg px-2.5 py-1.5 focus:border-cyan-500 focus:outline-none"
              >
                <option value="1">Custom</option>
                <option value="2">GR3750 (1990s, 400 DPI)</option>
                <option value="3">EZ591 (600e, single-drum)</option>
                <option value="4">MZ-790 (600e, 2-color)</option>
                <option value="5">RZ970 (600x600, one-drum)</option>
                <option value="6">SF5350 (300x600, modern)</option>
                <option value="7">CR1610 (Compact, 300 DPI)</option>
                <option value="8">SF9450 (Flagship, 600 DPI)</option>
                <option value="9">MH9450 (Modern, 600 DPI, 2-color)</option>
              </select>
            </div>

            <div className="grid grid-cols-2 gap-2">
              <div>
                <label className="text-zinc-400 block mb-1">Paper Type</label>
                <select
                  value={params.paperType}
                  onChange={(e) => updateParam('paperType', parseInt(e.target.value))}
                  className="w-full bg-zinc-900 border border-zinc-700 text-white rounded-lg px-2.5 py-1.5 focus:border-cyan-500 focus:outline-none"
                >
                  <option value="1">Custom</option>
                  <option value="2">Newsprint</option>
                  <option value="3">Cotton Rag</option>
                  <option value="4">Kraft (Banded)</option>
                  <option value="5">Papyrus</option>
                  <option value="6">Smooth Coated</option>
                </select>
              </div>

              <div>
                <label className="text-zinc-400 block mb-1">Background Mode</label>
                <select
                  value={params.backgroundMode}
                  onChange={(e) => updateParam('backgroundMode', parseInt(e.target.value))}
                  className="w-full bg-zinc-900 border border-zinc-700 text-white rounded-lg px-2.5 py-1.5 focus:border-cyan-500 focus:outline-none"
                >
                  <option value="1">Paper Stock</option>
                  <option value="2">Transparent</option>
                  <option value="3">Original Image</option>
                </select>
              </div>
            </div>

            <div>
              <label className="text-zinc-400 block mb-1">Spot Ink Kit</label>
              <select
                value={params.spotInkKit}
                onChange={(e) => {
                  const val = parseInt(e.target.value);
                  updateParam('spotInkKit', val);
                  if (val === 2) {
                    onChange({ ...params, spotInkKit: 2, colorMode: 2, activePlates: 2, customSpotInks: ['#FF48A7', '#0078BF', '#2900FF', '#1A1A1A'] });
                  } else if (val === 3) {
                    onChange({ ...params, spotInkKit: 3, colorMode: 2, activePlates: 3, customSpotInks: ['#82D2A5', '#4BB0E0', '#00838A', '#1A1A1A'] });
                  } else if (val === 4) {
                    onChange({ ...params, spotInkKit: 4, colorMode: 2, activePlates: 3, customSpotInks: ['#8A5A2B', '#92203F', '#C8A54B', '#1A1A1A'] });
                  } else if (val === 5) {
                    onChange({ ...params, spotInkKit: 5, colorMode: 2, activePlates: 3, customSpotInks: ['#485778', '#8A3361', '#E01B24', '#1A1A1A'] });
                  } else if (val === 6) {
                    onChange({ ...params, spotInkKit: 6, colorMode: 2, activePlates: 3, customSpotInks: ['#FFC400', '#FF6F00', '#FF3828', '#1A1A1A'] });
                  }
                }}
                className="w-full bg-zinc-900 border border-zinc-700 text-white rounded-lg px-2.5 py-1.5 focus:border-cyan-500 focus:outline-none"
              >
                <option value="1">Custom</option>
                <option value="2">Risograph Pink+Blue</option>
                <option value="3">Cool Trio (Mint+Sky+Teal)</option>
                <option value="4">Earthy (Brown+Burgundy+Gold)</option>
                <option value="5">Twilight (Indigo+Plum+Crimson)</option>
                <option value="6">Sunset (Sunflower+Pumpkin+Red)</option>
                <option value="7">Forest (Hunter+Sea+Brown)</option>
                <option value="8">Mint+Pink Duo</option>
                <option value="9">Cyan+Magenta Duo</option>
              </select>
            </div>
          </div>
        )}
      </div>

      {/* Accordion Group 3: Ink Palette & Separation */}
      <div className="border border-zinc-800 rounded-xl overflow-hidden bg-zinc-950/40">
        <button
          onClick={() => toggleSection('ink')}
          className="w-full px-3.5 py-2.5 bg-zinc-900/80 flex items-center justify-between text-xs font-semibold text-zinc-200 hover:text-white"
        >
          <span className="flex items-center gap-2">
            <Palette className="w-3.5 h-3.5 text-pink-400" />
            Ink Palette &amp; Overprint
          </span>
          {openSections.ink ? <ChevronDown className="w-4 h-4 text-zinc-500" /> : <ChevronRight className="w-4 h-4 text-zinc-500" />}
        </button>

        {openSections.ink && (
          <div className="p-3.5 space-y-3.5 text-xs border-t border-zinc-800/80">
            <div className="grid grid-cols-2 gap-2">
              <div>
                <label className="text-zinc-400 block mb-1">Color Mode</label>
                <select
                  value={params.colorMode}
                  onChange={(e) => updateParam('colorMode', parseInt(e.target.value))}
                  className="w-full bg-zinc-900 border border-zinc-700 text-white rounded-lg px-2.5 py-1.5 focus:border-cyan-500 focus:outline-none"
                >
                  <option value="1">CMYK</option>
                  <option value="2">Spot Riso</option>
                </select>
              </div>

              <div>
                <label className="text-zinc-400 block mb-1">Overprint Mode</label>
                <select
                  value={params.overprintMode}
                  onChange={(e) => updateParam('overprintMode', parseInt(e.target.value))}
                  className="w-full bg-zinc-900 border border-zinc-700 text-white rounded-lg px-2.5 py-1.5 focus:border-cyan-500 focus:outline-none"
                >
                  <option value="1">Multiply (Subtractive)</option>
                  <option value="2">Beer-Lambert (Physics)</option>
                </select>
              </div>
            </div>

            {params.colorMode === 2 && (
              <div className="p-2.5 bg-zinc-900/60 rounded-xl border border-zinc-800 space-y-2">
                <div className="flex justify-between items-center">
                  <span className="text-zinc-300 font-medium">Spot Separation</span>
                  <select
                    value={params.spotSeparation}
                    onChange={(e) => updateParam('spotSeparation', parseInt(e.target.value))}
                    className="bg-zinc-800 border border-zinc-700 text-white text-[11px] rounded px-2 py-0.5"
                  >
                    <option value="1">Greedy (Legacy)</option>
                    <option value="2">Balanced Mix (4-sweep)</option>
                  </select>
                </div>
                <div className="flex justify-between items-center">
                  <span className="text-zinc-400">Active Spot Plates</span>
                  <div className="flex gap-1">
                    {[2, 3, 4].map((count) => (
                      <button
                        key={count}
                        onClick={() => updateParam('activePlates', count)}
                        className={`px-2 py-0.5 rounded text-[11px] font-mono ${
                          params.activePlates === count ? 'bg-cyan-500 text-black font-bold' : 'bg-zinc-800 text-zinc-400'
                        }`}
                      >
                        {count}
                      </button>
                    ))}
                  </div>
                </div>
              </div>
            )}

            {/* Sliders */}
            <div className="space-y-2">
              <div>
                <div className="flex justify-between text-[11px] mb-1">
                  <span className="text-zinc-400">Total Coverage Limit (TAC)</span>
                  <span className="font-mono text-cyan-400">{params.totalCoverageLimit}%</span>
                </div>
                <input
                  type="range"
                  min="100"
                  max="400"
                  value={params.totalCoverageLimit}
                  onChange={(e) => updateParam('totalCoverageLimit', parseFloat(e.target.value))}
                  className="w-full h-1 bg-zinc-800 rounded appearance-none"
                />
              </div>

              <div>
                <div className="flex justify-between text-[11px] mb-1">
                  <span className="text-zinc-400">Black Boost / Rich Blacks</span>
                  <span className="font-mono text-cyan-400">{params.blackBoost}%</span>
                </div>
                <input
                  type="range"
                  min="0"
                  max="200"
                  value={params.blackBoost}
                  onChange={(e) => updateParam('blackBoost', parseFloat(e.target.value))}
                  className="w-full h-1 bg-zinc-800 rounded appearance-none"
                />
              </div>
            </div>
          </div>
        )}
      </div>

      {/* Accordion Group 4: Halftone Screens */}
      <div className="border border-zinc-800 rounded-xl overflow-hidden bg-zinc-950/40">
        <button
          onClick={() => toggleSection('halftone')}
          className="w-full px-3.5 py-2.5 bg-zinc-900/80 flex items-center justify-between text-xs font-semibold text-zinc-200 hover:text-white"
        >
          <span className="flex items-center gap-2">
            <Grid className="w-3.5 h-3.5 text-emerald-400" />
            Halftone &amp; Screen Dither
          </span>
          {openSections.halftone ? <ChevronDown className="w-4 h-4 text-zinc-500" /> : <ChevronRight className="w-4 h-4 text-zinc-500" />}
        </button>

        {openSections.halftone && (
          <div className="p-3.5 space-y-3.5 text-xs border-t border-zinc-800/80">
            <div className="grid grid-cols-2 gap-2">
              <div>
                <label className="text-zinc-400 block mb-1">Screen Type</label>
                <select
                  value={params.screenType}
                  onChange={(e) => updateParam('screenType', parseInt(e.target.value))}
                  className="w-full bg-zinc-900 border border-zinc-700 text-white rounded-lg px-2.5 py-1.5 focus:border-cyan-500 focus:outline-none"
                >
                  <option value="1">Voronoi (Union Dot)</option>
                  <option value="2">Bayer 8x8 (Ordered)</option>
                  <option value="3">Blue-noise</option>
                  <option value="4">Line Screen</option>
                  <option value="5">Stippled (Sand)</option>
                  <option value="6">Atkinson (sparse)</option>
                  <option value="7">Floyd-Steinberg</option>
                </select>
              </div>

              <div>
                <label className="text-zinc-400 block mb-1">Dot Shape</label>
                <select
                  value={params.dotShape}
                  onChange={(e) => updateParam('dotShape', parseInt(e.target.value))}
                  className="w-full bg-zinc-900 border border-zinc-700 text-white rounded-lg px-2.5 py-1.5 focus:border-cyan-500 focus:outline-none"
                >
                  <option value="1">Circle</option>
                  <option value="2">Ellipse</option>
                  <option value="3">Diamond</option>
                  <option value="4">Square</option>
                </select>
              </div>
            </div>

            <div className="space-y-2">
              <div>
                <div className="flex justify-between text-[11px] mb-1">
                  <span className="text-zinc-400">Screen Density</span>
                  <span className="font-mono text-cyan-400">{params.screenDensity} LPI</span>
                </div>
                <input
                  type="range"
                  min="30"
                  max="400"
                  value={params.screenDensity}
                  onChange={(e) => updateParam('screenDensity', parseFloat(e.target.value))}
                  className="w-full h-1 bg-zinc-800 rounded appearance-none"
                />
              </div>

              <div>
                <div className="flex justify-between text-[11px] mb-1">
                  <span className="text-zinc-400">Dot Hardness</span>
                  <span className="font-mono text-cyan-400">{params.dotHardness}%</span>
                </div>
                <input
                  type="range"
                  min="0"
                  max="200"
                  value={params.dotHardness}
                  onChange={(e) => updateParam('dotHardness', parseFloat(e.target.value))}
                  className="w-full h-1 bg-zinc-800 rounded appearance-none"
                />
              </div>

              <div>
                <div className="flex justify-between text-[11px] mb-1">
                  <span className="text-zinc-400">Softness / Spread</span>
                  <span className="font-mono text-cyan-400">{params.softness}%</span>
                </div>
                <input
                  type="range"
                  min="0"
                  max="100"
                  value={params.softness}
                  onChange={(e) => updateParam('softness', parseFloat(e.target.value))}
                  className="w-full h-1 bg-zinc-800 rounded appearance-none"
                />
              </div>

              <div>
                <div className="flex justify-between text-[11px] mb-1">
                  <span className="text-zinc-400">Grid Distortion (FBM)</span>
                  <span className="font-mono text-cyan-400">{params.distortion}%</span>
                </div>
                <input
                  type="range"
                  min="0"
                  max="60"
                  value={params.distortion}
                  onChange={(e) => updateParam('distortion', parseFloat(e.target.value))}
                  className="w-full h-1 bg-zinc-800 rounded appearance-none"
                />
              </div>
            </div>
          </div>
        )}
      </div>

      {/* Accordion Group 5: Plate Alignment & Micro-Chatter */}
      <div className="border border-zinc-800 rounded-xl overflow-hidden bg-zinc-950/40">
        <button
          onClick={() => toggleSection('alignment')}
          className="w-full px-3.5 py-2.5 bg-zinc-900/80 flex items-center justify-between text-xs font-semibold text-zinc-200 hover:text-white"
        >
          <span className="flex items-center gap-2">
            <Move className="w-3.5 h-3.5 text-amber-400" />
            Plate Alignment &amp; Drift
          </span>
          {openSections.alignment ? <ChevronDown className="w-4 h-4 text-zinc-500" /> : <ChevronRight className="w-4 h-4 text-zinc-500" />}
        </button>

        {openSections.alignment && (
          <div className="p-3.5 space-y-3 text-xs border-t border-zinc-800/80">
            <div>
              <div className="flex justify-between text-[11px] mb-1">
                <span className="text-zinc-400">All Plates Offset X (px)</span>
                <span className="font-mono text-cyan-400">{params.allOffsetX.toFixed(1)} px</span>
              </div>
              <input
                type="range"
                min="-50"
                max="50"
                step="0.5"
                value={params.allOffsetX}
                onChange={(e) => updateParam('allOffsetX', parseFloat(e.target.value))}
                className="w-full h-1 bg-zinc-800 rounded appearance-none"
              />
            </div>

            <div className="grid grid-cols-2 gap-2">
              <div>
                <div className="flex justify-between text-[11px] mb-1">
                  <span className="text-zinc-400">Cyan Offset X</span>
                  <span className="font-mono text-cyan-400">{params.cyanOffsetX}</span>
                </div>
                <input
                  type="range"
                  min="-20"
                  max="20"
                  value={params.cyanOffsetX}
                  onChange={(e) => updateParam('cyanOffsetX', parseFloat(e.target.value))}
                  className="w-full h-1 bg-zinc-800 rounded appearance-none"
                />
              </div>

              <div>
                <div className="flex justify-between text-[11px] mb-1">
                  <span className="text-zinc-400">Magenta Offset X</span>
                  <span className="font-mono text-cyan-400">{params.magentaOffsetX}</span>
                </div>
                <input
                  type="range"
                  min="-20"
                  max="20"
                  value={params.magentaOffsetX}
                  onChange={(e) => updateParam('magentaOffsetX', parseFloat(e.target.value))}
                  className="w-full h-1 bg-zinc-800 rounded appearance-none"
                />
              </div>
            </div>
          </div>
        )}
      </div>

      {/* Accordion Group 6: Paper & Grain */}
      <div className="border border-zinc-800 rounded-xl overflow-hidden bg-zinc-950/40">
        <button
          onClick={() => toggleSection('paper')}
          className="w-full px-3.5 py-2.5 bg-zinc-900/80 flex items-center justify-between text-xs font-semibold text-zinc-200 hover:text-white"
        >
          <span className="flex items-center gap-2">
            <FileText className="w-3.5 h-3.5 text-orange-400" />
            Paper Stock &amp; Aging
          </span>
          {openSections.paper ? <ChevronDown className="w-4 h-4 text-zinc-500" /> : <ChevronRight className="w-4 h-4 text-zinc-500" />}
        </button>

        {openSections.paper && (
          <div className="p-3.5 space-y-3 text-xs border-t border-zinc-800/80">
            <div className="flex items-center justify-between">
              <span className="text-zinc-300">Paper Base Color</span>
              <input
                type="color"
                value={params.paperColor}
                onChange={(e) => updateParam('paperColor', e.target.value)}
                className="w-7 h-7 rounded border border-zinc-700 bg-transparent cursor-pointer"
              />
            </div>

            <div>
              <div className="flex justify-between text-[11px] mb-1">
                <span className="text-zinc-400">Paper Grain Structure</span>
                <span className="font-mono text-cyan-400">{params.paperGrain}%</span>
              </div>
              <input
                type="range"
                min="0"
                max="80"
                value={params.paperGrain}
                onChange={(e) => updateParam('paperGrain', parseFloat(e.target.value))}
                className="w-full h-1 bg-zinc-800 rounded appearance-none"
              />
            </div>

            <div>
              <div className="flex justify-between text-[11px] mb-1">
                <span className="text-zinc-400">Aging Amount</span>
                <span className="font-mono text-cyan-400">{params.agingAmount}%</span>
              </div>
              <input
                type="range"
                min="0"
                max="60"
                value={params.agingAmount}
                onChange={(e) => updateParam('agingAmount', parseFloat(e.target.value))}
                className="w-full h-1 bg-zinc-800 rounded appearance-none"
              />
            </div>
          </div>
        )}
      </div>

      {/* Accordion Group 7: Print Wear & Imperfections */}
      <div className="border border-zinc-800 rounded-xl overflow-hidden bg-zinc-950/40">
        <button
          onClick={() => toggleSection('wear')}
          className="w-full px-3.5 py-2.5 bg-zinc-900/80 flex items-center justify-between text-xs font-semibold text-zinc-200 hover:text-white"
        >
          <span className="flex items-center gap-2">
            <Activity className="w-3.5 h-3.5 text-purple-400" />
            Print Wear &amp; Streaks
          </span>
          {openSections.wear ? <ChevronDown className="w-4 h-4 text-zinc-500" /> : <ChevronRight className="w-4 h-4 text-zinc-500" />}
        </button>

        {openSections.wear && (
          <div className="p-3.5 space-y-3 text-xs border-t border-zinc-800/80">
            <div>
              <div className="flex justify-between text-[11px] mb-1">
                <span className="text-zinc-400">Ink Flecks &amp; Specks</span>
                <span className="font-mono text-cyan-400">{params.inkFlecks}%</span>
              </div>
              <input
                type="range"
                min="0"
                max="50"
                value={params.inkFlecks}
                onChange={(e) => updateParam('inkFlecks', parseFloat(e.target.value))}
                className="w-full h-1 bg-zinc-800 rounded appearance-none"
              />
            </div>

            <div>
              <div className="flex justify-between text-[11px] mb-1">
                <span className="text-zinc-400">Roller Streaks</span>
                <span className="font-mono text-cyan-400">{params.rollerStreaks}%</span>
              </div>
              <input
                type="range"
                min="0"
                max="50"
                value={params.rollerStreaks}
                onChange={(e) => updateParam('rollerStreaks', parseFloat(e.target.value))}
                className="w-full h-1 bg-zinc-800 rounded appearance-none"
              />
            </div>

            <div className="pt-1">
              <label className="flex items-center gap-2 cursor-pointer text-zinc-300">
                <input
                  type="checkbox"
                  checked={params.reversePrint}
                  onChange={(e) => updateParam('reversePrint', e.target.checked)}
                  className="rounded border-zinc-700 bg-zinc-800 text-cyan-500 focus:ring-0"
                />
                Reverse Print (white on dark)
              </label>
            </div>
          </div>
        )}
      </div>

      {/* Accordion Group 8: Output & Safety */}
      <div className="border border-zinc-800 rounded-xl overflow-hidden bg-zinc-950/40">
        <button
          onClick={() => toggleSection('output')}
          className="w-full px-3.5 py-2.5 bg-zinc-900/80 flex items-center justify-between text-xs font-semibold text-zinc-200 hover:text-white"
        >
          <span className="flex items-center gap-2">
            <Sparkles className="w-3.5 h-3.5 text-cyan-400" />
            Output Gamma &amp; Quality
          </span>
          {openSections.output ? <ChevronDown className="w-4 h-4 text-zinc-500" /> : <ChevronRight className="w-4 h-4 text-zinc-500" />}
        </button>

        {openSections.output && (
          <div className="p-3.5 space-y-3 text-xs border-t border-zinc-800/80">
            <div>
              <div className="flex justify-between text-[11px] mb-1">
                <span className="text-zinc-400">Output Gamma</span>
                <span className="font-mono text-cyan-400">{params.outputGamma.toFixed(2)}</span>
              </div>
              <input
                type="range"
                min="0.8"
                max="2.2"
                step="0.05"
                value={params.outputGamma}
                onChange={(e) => updateParam('outputGamma', parseFloat(e.target.value))}
                className="w-full h-1 bg-zinc-800 rounded appearance-none"
              />
            </div>

            <div>
              <div className="flex justify-between text-[11px] mb-1">
                <span className="text-zinc-400">Blend With Original (%)</span>
                <span className={`font-mono ${params.blendOriginal === 0 ? 'text-emerald-400' : 'text-amber-400'}`}>
                  {params.blendOriginal}% {params.blendOriginal === 0 && '(Zero Ghosting)'}
                </span>
              </div>
              <input
                type="range"
                min="0"
                max="100"
                value={params.blendOriginal}
                onChange={(e) => updateParam('blendOriginal', parseFloat(e.target.value))}
                className="w-full h-1 bg-zinc-800 rounded appearance-none"
              />
            </div>
          </div>
        )}
      </div>
    </div>
  );
};
