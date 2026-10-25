import React from 'react';
import { RisoParams, BLEND_MODES, DEFAULT_PARAMS } from '../engine/risoprintEngine';
import { RotateCcw, Sliders, Palette, Layers, Sparkles, Move } from 'lucide-react';

interface ControlPanelProps {
  params: RisoParams;
  onChange: (params: RisoParams) => void;
  renderMode: 'composite' | 'paper' | 'plate1' | 'plate2' | 'plate3' | 'bugged_simulation';
  setRenderMode: (mode: 'composite' | 'paper' | 'plate1' | 'plate2' | 'plate3' | 'bugged_simulation') => void;
}

export const ControlPanel: React.FC<ControlPanelProps> = ({
  params,
  onChange,
  renderMode,
  setRenderMode,
}) => {
  const updateParam = <K extends keyof RisoParams>(key: K, val: RisoParams[K]) => {
    onChange({ ...params, [key]: val });
  };

  const updateInk = (idx: 0 | 1 | 2, updates: Partial<RisoParams['inks'][0]>) => {
    const nextInks = [...params.inks] as [RisoParams['inks'][0], RisoParams['inks'][1], RisoParams['inks'][2]];
    nextInks[idx] = { ...nextInks[idx], ...updates };
    onChange({ ...params, inks: nextInks });
  };

  const resetDefaults = () => {
    onChange({ ...DEFAULT_PARAMS });
    setRenderMode('composite');
  };

  const applyPreset = (name: string) => {
    if (name === 'reference') {
      onChange({ ...DEFAULT_PARAMS });
    } else if (name === 'punchy') {
      onChange({
        ...DEFAULT_PARAMS,
        contrast: 1.85,
        opacity: 0.78,
        scale: 0.18,
        inks: [
          { enabled: true, color: '#00E5FF', separation: 0.45, shiftX: 0.004, shiftY: 0.002 },
          { enabled: true, color: '#FF5722', separation: 0.72, shiftX: 0.0, shiftY: -0.002 },
          { enabled: true, color: '#1A237E', separation: 0.42, shiftX: -0.002, shiftY: -0.004 },
        ],
      });
    } else if (name === 'vintage') {
      onChange({
        ...DEFAULT_PARAMS,
        paper: '#FBF5E6',
        scale: 0.28,
        softness: 0.65,
        inkContrast: -0.2,
        inks: [
          { enabled: true, color: '#00A896', separation: 0.35, shiftX: 0.002, shiftY: 0.001 },
          { enabled: true, color: '#D47200', separation: 0.60, shiftX: -0.001, shiftY: 0.0 },
          { enabled: true, color: '#5B1E31', separation: 0.35, shiftX: 0.0, shiftY: -0.002 },
        ],
      });
    }
  };

  return (
    <div className="w-full lg:w-96 flex-shrink-0 bg-zinc-900/90 border border-zinc-800 rounded-2xl p-4 flex flex-col gap-5 overflow-y-auto max-h-[calc(100vh-5rem)]">
      {/* Top Header & Reset */}
      <div className="flex items-center justify-between pb-3 border-b border-zinc-800">
        <div className="flex items-center gap-2">
          <Sliders className="w-4 h-4 text-cyan-400" />
          <h2 className="text-sm font-bold text-white tracking-wide uppercase font-mono">
            Effect Controls
          </h2>
        </div>

        <button
          onClick={resetDefaults}
          className="flex items-center gap-1.5 text-xs text-zinc-400 hover:text-white px-2.5 py-1 rounded-lg bg-zinc-800 hover:bg-zinc-700 transition"
          title="Reset all settings to native reference defaults (matching After Effects settings screenshots)"
        >
          <RotateCcw className="w-3 h-3 text-cyan-400" />
          Reset Defaults
        </button>
      </div>

      {/* Render Mode & Plate Isolation */}
      <div className="space-y-2">
        <label className="text-xs font-semibold text-zinc-300 flex items-center justify-between">
          <span className="flex items-center gap-1.5">
            <Layers className="w-3.5 h-3.5 text-indigo-400" />
            Inspection &amp; Stencil Mode
          </span>
          <span className="text-[10px] text-zinc-500 font-mono">SmartRender</span>
        </label>
        <div className="grid grid-cols-3 gap-1.5">
          <button
            onClick={() => setRenderMode('composite')}
            className={`px-2 py-1.5 rounded-lg text-xs font-medium border transition ${
              renderMode === 'composite'
                ? 'bg-cyan-950 text-cyan-200 border-cyan-700 shadow-sm'
                : 'bg-zinc-800/80 text-zinc-400 border-zinc-700/60 hover:bg-zinc-800 hover:text-white'
            }`}
          >
            3 Plates (All)
          </button>
          <button
            onClick={() => setRenderMode('plate1')}
            className={`px-2 py-1.5 rounded-lg text-xs font-medium border transition ${
              renderMode === 'plate1'
                ? 'bg-cyan-950 text-cyan-200 border-cyan-700'
                : 'bg-zinc-800/80 text-zinc-400 border-zinc-700/60 hover:bg-zinc-800 hover:text-white'
            }`}
          >
            Plate 1 (Cyan)
          </button>
          <button
            onClick={() => setRenderMode('plate2')}
            className={`px-2 py-1.5 rounded-lg text-xs font-medium border transition ${
              renderMode === 'plate2'
                ? 'bg-amber-950 text-amber-200 border-amber-700'
                : 'bg-zinc-800/80 text-zinc-400 border-zinc-700/60 hover:bg-zinc-800 hover:text-white'
            }`}
          >
            Plate 2 (Ochre)
          </button>
          <button
            onClick={() => setRenderMode('plate3')}
            className={`px-2 py-1.5 rounded-lg text-xs font-medium border transition ${
              renderMode === 'plate3'
                ? 'bg-blue-950 text-blue-200 border-blue-700'
                : 'bg-zinc-800/80 text-zinc-400 border-zinc-700/60 hover:bg-zinc-800 hover:text-white'
            }`}
          >
            Plate 3 (Blue)
          </button>
          <button
            onClick={() => setRenderMode('paper')}
            className={`px-2 py-1.5 rounded-lg text-xs font-medium border transition ${
              renderMode === 'paper'
                ? 'bg-zinc-700 text-white border-zinc-500'
                : 'bg-zinc-800/80 text-zinc-400 border-zinc-700/60 hover:bg-zinc-800 hover:text-white'
            }`}
          >
            Paper Stipple
          </button>
          <button
            onClick={() => setRenderMode('bugged_simulation')}
            className={`px-2 py-1.5 rounded-lg text-xs font-semibold border transition ${
              renderMode === 'bugged_simulation'
                ? 'bg-rose-950 text-rose-300 border-rose-700 animate-pulse'
                : 'bg-zinc-800/80 text-rose-400/80 border-rose-950/60 hover:bg-rose-950/40 hover:text-rose-300'
            }`}
            title="Demonstrates the 4 symptoms caused by the broken parameter index mismatch"
          >
            Simulate Bug
          </button>
        </div>
      </div>

      {/* Preset Buttons */}
      <div className="flex items-center gap-1.5 overflow-x-auto pb-1">
        <span className="text-[11px] font-semibold text-zinc-400 flex items-center gap-1">
          <Sparkles className="w-3 h-3 text-cyan-400" /> Presets:
        </span>
        <button
          onClick={() => applyPreset('reference')}
          className="text-[11px] px-2 py-0.5 rounded-md bg-zinc-800 hover:bg-zinc-700 text-zinc-300 whitespace-nowrap border border-zinc-700/50"
        >
          Reference Target
        </button>
        <button
          onClick={() => applyPreset('punchy')}
          className="text-[11px] px-2 py-0.5 rounded-md bg-zinc-800 hover:bg-zinc-700 text-zinc-300 whitespace-nowrap border border-zinc-700/50"
        >
          High Contrast
        </button>
        <button
          onClick={() => applyPreset('vintage')}
          className="text-[11px] px-2 py-0.5 rounded-md bg-zinc-800 hover:bg-zinc-700 text-zinc-300 whitespace-nowrap border border-zinc-700/50"
        >
          Warm Newsprint
        </button>
      </div>

      {/* Section 1: Master Controls */}
      <div className="space-y-3 pt-2 border-t border-zinc-800/80">
        <h3 className="text-xs font-bold text-zinc-400 tracking-wider uppercase font-mono">
          Master Settings
        </h3>

        {/* Intensity */}
        <div className="space-y-1">
          <div className="flex justify-between text-xs">
            <span className="text-zinc-300">Intensity</span>
            <span className="font-mono text-cyan-400">{params.intensity}%</span>
          </div>
          <input
            type="range"
            min={0}
            max={100}
            step={1}
            value={params.intensity}
            onChange={(e) => updateParam('intensity', Number(e.target.value))}
            className="w-full h-1.5 bg-zinc-800 rounded-lg cursor-pointer"
          />
        </div>

        {/* Blend Mode */}
        <div className="space-y-1">
          <label className="text-xs text-zinc-300">Blend Mode</label>
          <select
            value={params.blend}
            onChange={(e) => updateParam('blend', Number(e.target.value))}
            className="w-full text-xs bg-zinc-950 border border-zinc-700 rounded-lg px-2.5 py-1.5 text-zinc-200 focus:outline-none focus:border-cyan-500"
          >
            {BLEND_MODES.map((mode) => (
              <option key={mode.id} value={mode.id}>
                {mode.name}
              </option>
            ))}
          </select>
        </div>

        {/* Show Source Toggle */}
        <div className="flex items-center justify-between py-1">
          <span className="text-xs text-zinc-300">Show Source</span>
          <button
            onClick={() => updateParam('showSource', !params.showSource)}
            className={`w-11 h-6 rounded-full transition-colors relative p-0.5 border ${
              params.showSource
                ? 'bg-cyan-600 border-cyan-500'
                : 'bg-zinc-800 border-zinc-700'
            }`}
          >
            <div
              className={`w-4 h-4 rounded-full bg-white transition-transform ${
                params.showSource ? 'translate-x-5' : 'translate-x-0.5'
              }`}
            />
          </button>
        </div>

        {/* Paper Color */}
        <div className="flex items-center justify-between py-1">
          <span className="text-xs text-zinc-300 flex items-center gap-1.5">
            <Palette className="w-3.5 h-3.5 text-zinc-400" />
            Paper Color
          </span>
          <div className="flex items-center gap-2">
            <input
              type="color"
              value={params.paper}
              onChange={(e) => updateParam('paper', e.target.value)}
              className="w-6 h-6 rounded border border-zinc-700 cursor-pointer bg-transparent"
            />
            <span className="font-mono text-xs text-zinc-400 uppercase">{params.paper}</span>
          </div>
        </div>
      </div>

      {/* Section 2: Texture & Print Response */}
      <div className="space-y-3 pt-2 border-t border-zinc-800/80">
        <h3 className="text-xs font-bold text-zinc-400 tracking-wider uppercase font-mono">
          Print Texture &amp; Halftone
        </h3>

        {/* Ink Contrast */}
        <div className="space-y-1">
          <div className="flex justify-between text-xs">
            <span className="text-zinc-300">Ink Contrast</span>
            <span className="font-mono text-cyan-400">{params.inkContrast.toFixed(2)}</span>
          </div>
          <input
            type="range"
            min={-1}
            max={1}
            step={0.01}
            value={params.inkContrast}
            onChange={(e) => updateParam('inkContrast', Number(e.target.value))}
            className="w-full h-1.5 bg-zinc-800 rounded-lg cursor-pointer"
          />
        </div>

        {/* Scale (Grain size) */}
        <div className="space-y-1">
          <div className="flex justify-between text-xs">
            <span className="text-zinc-300">Scale (Grain Size)</span>
            <span className="font-mono text-cyan-400">{params.scale.toFixed(2)}</span>
          </div>
          <input
            type="range"
            min={0.03}
            max={1.0}
            step={0.01}
            value={params.scale}
            onChange={(e) => updateParam('scale', Number(e.target.value))}
            className="w-full h-1.5 bg-zinc-800 rounded-lg cursor-pointer"
          />
        </div>

        {/* Opacity */}
        <div className="space-y-1">
          <div className="flex justify-between text-xs">
            <span className="text-zinc-300">Opacity (Ink Laydown)</span>
            <span className="font-mono text-cyan-400">{params.opacity.toFixed(2)}</span>
          </div>
          <input
            type="range"
            min={0}
            max={1}
            step={0.01}
            value={params.opacity}
            onChange={(e) => updateParam('opacity', Number(e.target.value))}
            className="w-full h-1.5 bg-zinc-800 rounded-lg cursor-pointer"
          />
        </div>

        {/* Softness */}
        <div className="space-y-1">
          <div className="flex justify-between text-xs">
            <span className="text-zinc-300">Softness</span>
            <span className="font-mono text-cyan-400">{params.softness.toFixed(2)}</span>
          </div>
          <input
            type="range"
            min={0}
            max={1}
            step={0.01}
            value={params.softness}
            onChange={(e) => updateParam('softness', Number(e.target.value))}
            className="w-full h-1.5 bg-zinc-800 rounded-lg cursor-pointer"
          />
        </div>

        {/* Contrast */}
        <div className="space-y-1">
          <div className="flex justify-between text-xs">
            <span className="text-zinc-300">Contrast (Reference: 1.58)</span>
            <span className="font-mono text-cyan-400">{params.contrast.toFixed(2)}</span>
          </div>
          <input
            type="range"
            min={0.25}
            max={3.0}
            step={0.01}
            value={params.contrast}
            onChange={(e) => updateParam('contrast', Number(e.target.value))}
            className="w-full h-1.5 bg-zinc-800 rounded-lg cursor-pointer"
          />
        </div>

        {/* Blend With Original */}
        <div className="space-y-1">
          <div className="flex justify-between text-xs">
            <span className="text-zinc-300">Blend With Original (%)</span>
            <span className="font-mono text-cyan-400">{params.blendOriginal}%</span>
          </div>
          <input
            type="range"
            min={0}
            max={100}
            step={1}
            value={params.blendOriginal}
            onChange={(e) => updateParam('blendOriginal', Number(e.target.value))}
            className="w-full h-1.5 bg-zinc-800 rounded-lg cursor-pointer"
          />
          <p className="text-[10px] text-zinc-500">
            Set to 0% by default to prevent unwanted source ghosting/double exposure.
          </p>
        </div>
      </div>

      {/* Section 3: 3 Ink Plates */}
      <div className="space-y-3 pt-2 border-t border-zinc-800/80">
        <h3 className="text-xs font-bold text-zinc-400 tracking-wider uppercase font-mono">
          Ink Plates &amp; Registration
        </h3>

        {[0, 1, 2].map((idx) => {
          const ink = params.inks[idx];
          const plateName = idx === 0 ? 'Cyan Plate 1' : idx === 1 ? 'Ochre Plate 2' : 'Blue Plate 3';
          return (
            <div
              key={idx}
              className="bg-zinc-950/70 border border-zinc-800/80 rounded-xl p-3 space-y-2.5"
            >
              <div className="flex items-center justify-between">
                <div className="flex items-center gap-2">
                  <input
                    type="checkbox"
                    checked={ink.enabled}
                    onChange={(e) => updateInk(idx as 0 | 1 | 2, { enabled: e.target.checked })}
                    className="rounded bg-zinc-800 border-zinc-700 text-cyan-500 focus:ring-0 cursor-pointer"
                  />
                  <span className="text-xs font-semibold text-zinc-200">{plateName}</span>
                </div>

                <div className="flex items-center gap-2">
                  <input
                    type="color"
                    value={ink.color}
                    onChange={(e) => updateInk(idx as 0 | 1 | 2, { color: e.target.value })}
                    className="w-5 h-5 rounded border border-zinc-700 cursor-pointer bg-transparent"
                  />
                  <span className="font-mono text-[11px] text-zinc-400 uppercase">{ink.color}</span>
                </div>
              </div>

              {/* Separation */}
              <div className="space-y-1">
                <div className="flex justify-between text-[11px]">
                  <span className="text-zinc-400">Separation</span>
                  <span className="font-mono text-cyan-400">{ink.separation.toFixed(2)}</span>
                </div>
                <input
                  type="range"
                  min={0}
                  max={1}
                  step={0.01}
                  value={ink.separation}
                  onChange={(e) => updateInk(idx as 0 | 1 | 2, { separation: Number(e.target.value) })}
                  className="w-full h-1 bg-zinc-800 rounded-lg cursor-pointer"
                />
              </div>

              {/* Registration Shifts */}
              <div className="grid grid-cols-2 gap-2 pt-1 border-t border-zinc-800/50">
                <div className="space-y-1">
                  <div className="flex justify-between text-[10px]">
                    <span className="text-zinc-400 flex items-center gap-1">
                      <Move className="w-2.5 h-2.5 text-zinc-500" /> Shift X
                    </span>
                    <span className="font-mono text-zinc-300">{ink.shiftX.toFixed(3)}</span>
                  </div>
                  <input
                    type="range"
                    min={-0.02}
                    max={0.02}
                    step={0.001}
                    value={ink.shiftX}
                    onChange={(e) => updateInk(idx as 0 | 1 | 2, { shiftX: Number(e.target.value) })}
                    className="w-full h-1 bg-zinc-800 rounded-lg cursor-pointer"
                  />
                </div>

                <div className="space-y-1">
                  <div className="flex justify-between text-[10px]">
                    <span className="text-zinc-400 flex items-center gap-1">
                      <Move className="w-2.5 h-2.5 text-zinc-500" /> Shift Y
                    </span>
                    <span className="font-mono text-zinc-300">{ink.shiftY.toFixed(3)}</span>
                  </div>
                  <input
                    type="range"
                    min={-0.02}
                    max={0.02}
                    step={0.001}
                    value={ink.shiftY}
                    onChange={(e) => updateInk(idx as 0 | 1 | 2, { shiftY: Number(e.target.value) })}
                    className="w-full h-1 bg-zinc-800 rounded-lg cursor-pointer"
                  />
                </div>
              </div>
            </div>
          );
        })}
      </div>
    </div>
  );
};
