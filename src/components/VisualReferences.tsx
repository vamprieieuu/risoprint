import React, { useState } from 'react';
import { Eye, ExternalLink, Image as ImageIcon, Sliders, CheckCircle2 } from 'lucide-react';

export const VisualReferences: React.FC = () => {
  const [selectedImg, setSelectedImg] = useState<string>('after');

  const assets = [
    {
      id: 'before',
      title: '01 Before (Source Footage)',
      filename: 'IMG_4003.jpeg',
      path: '/references/01_before/IMG_4003.jpeg',
      description:
        'Original photograph of rearing black horse against a deep burgundy/crimson background (#920C24). Cyan highlights visible along the mane.',
    },
    {
      id: 'after',
      title: '02 After (Visual Target)',
      filename: 'IMG_4010.jpeg',
      path: '/references/02_after/IMG_4010.jpeg',
      description:
        'The definitive target visual output: full-color risograph look with warm ochre/orange textured background, deep subtractive black/navy horse coat, cyan mane highlights, and controlled plate registration chatter.',
    },
    {
      id: 'settings1',
      title: '03 Plugin Settings (Part 1)',
      filename: 'IMG_4011.png',
      path: '/references/03_plugin_settings/IMG_4011.png',
      description:
        'Intensity 100%, Blend Normal, Show Source Off, Paper #FFFFFF, Ink Contrast -0.33, Scale 0.21, Opacity 0.65, Softness 0.50, Contrast 1.58, Ink 1 #50D9F1.',
    },
    {
      id: 'settings2',
      title: '03 Plugin Settings (Part 2)',
      filename: 'IMG_4012.png',
      path: '/references/03_plugin_settings/IMG_4012.png',
      description:
        'Separation 1 0.38, Shift X 1 0.003, Shift Y 1 0.002, Enabled 2 On, Ink 2 #BF7700, Separation 2 0.64, Shift X 2 0.000, Shift Y 2 -0.001, Enabled 3 On, Ink 3 #2900FF.',
    },
    {
      id: 'settings3',
      title: '03 Plugin Settings (Part 3)',
      filename: 'IMG_4013.png',
      path: '/references/03_plugin_settings/IMG_4013.png',
      description:
        'Separation 3 0.39, Shift X 3 0.000, Shift Y 3 -0.003. Confirms precise registration shift coordinates.',
    },
  ];

  const current = assets.find((a) => a.id === selectedImg) || assets[1];

  return (
    <div className="flex-1 flex flex-col gap-6 p-4 lg:p-6 overflow-y-auto max-w-7xl mx-auto w-full">
      {/* Header */}
      <div className="bg-zinc-900 border border-zinc-800 rounded-2xl p-6 shadow-xl flex flex-col md:flex-row md:items-center justify-between gap-4">
        <div>
          <div className="inline-flex items-center gap-1.5 px-3 py-1 rounded-full bg-indigo-950/80 border border-indigo-800 text-indigo-300 text-xs font-semibold mb-2">
            <ImageIcon className="w-3.5 h-3.5" />
            Original Handoff Visual References &amp; Parameters
          </div>
          <h2 className="text-xl font-bold text-white tracking-tight">
            Before, After, and Native UI Parameter Screenshots
          </h2>
          <p className="text-xs text-zinc-400 mt-1 max-w-2xl leading-relaxed">
            All plugin parameters and the visual look were calibrated to match these authoritative
            reference assets from the handoff bundle.
          </p>
        </div>

        <div className="flex items-center gap-2">
          <a
            href="https://nexus.synkit.net/effects/risograph-effect"
            target="_blank"
            rel="noreferrer"
            className="flex items-center gap-1.5 px-3 py-1.5 rounded-xl bg-zinc-800 hover:bg-zinc-700 text-xs font-medium text-zinc-300 border border-zinc-700 transition"
          >
            <ExternalLink className="w-3.5 h-3.5 text-cyan-400" />
            Nexus Synkit URL
          </a>
        </div>
      </div>

      {/* Asset Selector Tabs */}
      <div className="grid grid-cols-2 sm:grid-cols-3 lg:grid-cols-5 gap-3">
        {assets.map((asset) => {
          const isSelected = asset.id === selectedImg;
          return (
            <button
              key={asset.id}
              onClick={() => setSelectedImg(asset.id)}
              className={`p-3 rounded-xl border text-left transition-all flex flex-col gap-2 ${
                isSelected
                  ? 'bg-zinc-800 border-indigo-500 shadow-md shadow-indigo-950/40'
                  : 'bg-zinc-900/60 border-zinc-800 hover:bg-zinc-900 hover:border-zinc-700'
              }`}
            >
              <div className="aspect-[3/4] w-full rounded-lg overflow-hidden bg-black border border-zinc-800">
                <img
                  src={asset.path}
                  alt={asset.title}
                  className="w-full h-full object-cover object-top"
                />
              </div>
              <div>
                <span className="text-[10px] font-mono text-zinc-500 uppercase">{asset.filename}</span>
                <h4 className="text-xs font-bold text-white truncate">{asset.title}</h4>
              </div>
            </button>
          );
        })}
      </div>

      {/* Main Preview Card */}
      <div className="bg-zinc-900 border border-zinc-800 rounded-2xl p-6 flex flex-col lg:flex-row gap-6 shadow-xl items-center lg:items-start">
        {/* Large Image View */}
        <div className="max-w-md w-full rounded-xl overflow-hidden border border-zinc-800 bg-black shadow-2xl flex-shrink-0">
          <img
            src={current.path}
            alt={current.title}
            className="w-full h-auto max-h-[640px] object-contain mx-auto"
          />
        </div>

        {/* Info & Technical Notes */}
        <div className="flex-1 space-y-5 w-full">
          <div>
            <span className="text-xs font-mono text-indigo-400 font-semibold uppercase">
              Selected Reference File
            </span>
            <h3 className="text-xl font-bold text-white mt-1">{current.title}</h3>
            <p className="text-xs text-zinc-300 mt-2 leading-relaxed">{current.description}</p>
          </div>

          {current.id === 'after' && (
            <div className="bg-cyan-950/30 border border-cyan-800/50 rounded-xl p-4 space-y-3">
              <div className="flex items-center gap-2 text-cyan-300 font-bold text-xs uppercase font-mono">
                <CheckCircle2 className="w-4 h-4 text-cyan-400" /> Key Visual Characteristics Ported
              </div>
              <ul className="text-xs text-zinc-300 space-y-2 list-disc list-inside">
                <li>
                  <strong className="text-white">Dark Burgundy Background Translation:</strong> Deep
                  red from `IMG_4003` feeds Plate 2 (Ochre `#BF7700`) due to hue proximity (350° to 37°),
                  creating the vibrant warm orange/red background visible in `IMG_4010`.
                </li>
                <li>
                  <strong className="text-white">Subtractive Multi-Plate Darkness:</strong> Deep horse shadows
                  receive overlapping deposits from Cyan, Ochre, and Blue plates that multiply into rich,
                  textured print blacks rather than flat monochrome.
                </li>
                <li>
                  <strong className="text-white">Authentic Stencil Stipple Grain:</strong> Dual-octave
                  stochastic halftone noise applied across plate density transitions creates true risograph
                  screen texture without pixelation.
                </li>
                <li>
                  <strong className="text-white">Micro Registration Drift:</strong> Controlled plate offsets
                  (Plate 1 `+0.003, +0.002`, Plate 2 `0.000, -0.001`, Plate 3 `0.000, -0.003`) create
                  realistic printing imperfections at silhouette edges.
                </li>
              </ul>
            </div>
          )}

          {(current.id === 'settings1' || current.id === 'settings2' || current.id === 'settings3') && (
            <div className="bg-zinc-950 border border-zinc-800 rounded-xl p-4 space-y-2">
              <div className="flex items-center gap-2 text-zinc-300 font-bold text-xs uppercase font-mono">
                <Sliders className="w-4 h-4 text-cyan-400" /> Ground-Truth Parameters Table
              </div>
              <div className="grid grid-cols-2 sm:grid-cols-3 gap-2 font-mono text-xs">
                <div className="bg-zinc-900 p-2 rounded border border-zinc-800">
                  <span className="text-[10px] text-zinc-500 block">INTENSITY</span>
                  <span className="text-cyan-400 font-bold">100% (1.00)</span>
                </div>
                <div className="bg-zinc-900 p-2 rounded border border-zinc-800">
                  <span className="text-[10px] text-zinc-500 block">BLEND</span>
                  <span className="text-cyan-400 font-bold">Normal (1)</span>
                </div>
                <div className="bg-zinc-900 p-2 rounded border border-zinc-800">
                  <span className="text-[10px] text-zinc-500 block">PAPER</span>
                  <span className="text-cyan-400 font-bold">#FFFFFF</span>
                </div>
                <div className="bg-zinc-900 p-2 rounded border border-zinc-800">
                  <span className="text-[10px] text-zinc-500 block">INK CONTRAST</span>
                  <span className="text-cyan-400 font-bold">-0.33</span>
                </div>
                <div className="bg-zinc-900 p-2 rounded border border-zinc-800">
                  <span className="text-[10px] text-zinc-500 block">SCALE</span>
                  <span className="text-cyan-400 font-bold">0.21</span>
                </div>
                <div className="bg-zinc-900 p-2 rounded border border-zinc-800">
                  <span className="text-[10px] text-zinc-500 block">OPACITY</span>
                  <span className="text-cyan-400 font-bold">0.65</span>
                </div>
                <div className="bg-zinc-900 p-2 rounded border border-zinc-800">
                  <span className="text-[10px] text-zinc-500 block">SOFTNESS</span>
                  <span className="text-cyan-400 font-bold">0.50</span>
                </div>
                <div className="bg-zinc-900 p-2 rounded border border-zinc-800">
                  <span className="text-[10px] text-zinc-500 block">CONTRAST</span>
                  <span className="text-cyan-400 font-bold">1.58</span>
                </div>
                <div className="bg-zinc-900 p-2 rounded border border-zinc-800">
                  <span className="text-[10px] text-zinc-500 block">PLATE 1 CYAN</span>
                  <span className="text-cyan-400 font-bold">#50D9F1 | Sep 0.38</span>
                </div>
                <div className="bg-zinc-900 p-2 rounded border border-zinc-800">
                  <span className="text-[10px] text-zinc-500 block">PLATE 2 OCHRE</span>
                  <span className="text-amber-400 font-bold">#BF7700 | Sep 0.64</span>
                </div>
                <div className="bg-zinc-900 p-2 rounded border border-zinc-800">
                  <span className="text-[10px] text-zinc-500 block">PLATE 3 BLUE</span>
                  <span className="text-blue-400 font-bold">#2900FF | Sep 0.39</span>
                </div>
                <div className="bg-zinc-900 p-2 rounded border border-zinc-800">
                  <span className="text-[10px] text-zinc-500 block">ORIGINAL BLEND</span>
                  <span className="text-emerald-400 font-bold">0.0% (Zero Ghosting)</span>
                </div>
              </div>
            </div>
          )}
        </div>
      </div>
    </div>
  );
};
