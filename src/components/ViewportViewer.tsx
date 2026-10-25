import React, { useRef, useEffect, useState, useCallback } from 'react';
import { RisoParams, renderRisograph, RenderMode } from '../engine/risoprintEngine';
import {
  Upload,
  Download,
  SplitSquareVertical,
  Columns,
  Eye,
  ZoomIn,
  ZoomOut,
  Maximize2,
  RefreshCw,
  AlertTriangle,
  CheckCircle2,
} from 'lucide-react';

interface ViewportViewerProps {
  params: RisoParams;
  renderMode: RenderMode;
}

export const ViewportViewer: React.FC<ViewportViewerProps> = ({ params, renderMode }) => {
  const canvasRef = useRef<HTMLCanvasElement | null>(null);
  const wipeCanvasRef = useRef<HTMLCanvasElement | null>(null);
  const originalImgRef = useRef<HTMLImageElement | null>(null);
  const targetImgRef = useRef<HTMLImageElement | null>(null);

  const [imageSource, setImageSource] = useState<'default_horse' | 'custom'>('default_horse');
  const [customImgUrl, setCustomImgUrl] = useState<string | null>(null);

  const [viewMode, setViewMode] = useState<'wipe' | 'sideBySide' | 'renderedOnly'>('wipe');
  const [wipePosition, setWipePosition] = useState<number>(50); // 0..100%
  const [wipeCompareTarget, setWipeCompareTarget] = useState<'original' | 'referenceTarget'>('original');

  const [zoom, setZoom] = useState<number>(1);
  const [renderTimeMs, setRenderTimeMs] = useState<number>(0);
  const [isProcessing, setIsProcessing] = useState<boolean>(false);
  const [resolution, setResolution] = useState<{ w: number; h: number }>({ w: 600, h: 800 });

  // Load images
  useEffect(() => {
    const orig = new Image();
    orig.crossOrigin = 'anonymous';
    orig.src = customImgUrl || '/references/01_before/IMG_4003.jpeg';
    orig.onload = () => {
      originalImgRef.current = orig;
      // Use reasonable canvas resolution for smooth real-time response
      const maxDim = 800;
      let w = orig.naturalWidth || 600;
      let h = orig.naturalHeight || 800;
      if (w > maxDim || h > maxDim) {
        if (w > h) {
          h = Math.round((h * maxDim) / w);
          w = maxDim;
        } else {
          w = Math.round((w * maxDim) / h);
          h = maxDim;
        }
      }
      setResolution({ w, h });
      triggerRender();
    };

    const target = new Image();
    target.crossOrigin = 'anonymous';
    target.src = '/references/02_after/IMG_4010.jpeg';
    target.onload = () => {
      targetImgRef.current = target;
    };
  }, [customImgUrl]);

  // Main render function
  const triggerRender = useCallback(() => {
    if (!originalImgRef.current || !canvasRef.current) return;
    const img = originalImgRef.current;
    const canvas = canvasRef.current;
    const { w, h } = resolution;

    canvas.width = w;
    canvas.height = h;
    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    setIsProcessing(true);
    const t0 = performance.now();

    // Draw original scaled
    const offscreen = document.createElement('canvas');
    offscreen.width = w;
    offscreen.height = h;
    const offCtx = offscreen.getContext('2d');
    if (!offCtx) return;
    offCtx.drawImage(img, 0, 0, w, h);

    const srcImageData = offCtx.getImageData(0, 0, w, h);

    // Run engine
    const resultImageData = renderRisograph(srcImageData, params, 0, renderMode);
    ctx.putImageData(resultImageData, 0, 0);

    const t1 = performance.now();
    setRenderTimeMs(Math.round(t1 - t0));
    setIsProcessing(false);
  }, [params, renderMode, resolution]);

  // Trigger render when params or resolution changes
  useEffect(() => {
    const timer = setTimeout(() => {
      triggerRender();
    }, 15);
    return () => clearTimeout(timer);
  }, [triggerRender]);

  // Handle custom upload
  const handleUpload = (e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0];
    if (file) {
      const url = URL.createObjectURL(file);
      setCustomImgUrl(url);
      setImageSource('custom');
    }
  };

  // Download rendered image
  const handleDownloadRender = () => {
    if (!canvasRef.current) return;
    const a = document.createElement('a');
    a.href = canvasRef.current.toDataURL('image/png');
    a.download = `risoprint-${renderMode}-${Date.now()}.png`;
    a.click();
  };

  return (
    <div className="flex-1 flex flex-col min-w-0 bg-zinc-950 rounded-2xl border border-zinc-800 overflow-hidden shadow-2xl">
      {/* Top Controls Toolbar */}
      <div className="border-b border-zinc-800 bg-zinc-900/60 p-3 flex flex-wrap items-center justify-between gap-3">
        {/* Left: View mode toggles */}
        <div className="flex items-center gap-1 bg-zinc-950 p-1 rounded-xl border border-zinc-800">
          <button
            onClick={() => setViewMode('wipe')}
            className={`flex items-center gap-1.5 px-2.5 py-1 text-xs font-medium rounded-lg transition ${
              viewMode === 'wipe'
                ? 'bg-zinc-800 text-cyan-400 border border-zinc-700/80 shadow-sm'
                : 'text-zinc-400 hover:text-zinc-200'
            }`}
            title="Split-screen slider wipe comparison"
          >
            <SplitSquareVertical className="w-3.5 h-3.5" />
            Wipe Compare
          </button>

          <button
            onClick={() => setViewMode('sideBySide')}
            className={`flex items-center gap-1.5 px-2.5 py-1 text-xs font-medium rounded-lg transition ${
              viewMode === 'sideBySide'
                ? 'bg-zinc-800 text-cyan-400 border border-zinc-700/80 shadow-sm'
                : 'text-zinc-400 hover:text-zinc-200'
            }`}
            title="Side by side comparison"
          >
            <Columns className="w-3.5 h-3.5" />
            Side-by-Side
          </button>

          <button
            onClick={() => setViewMode('renderedOnly')}
            className={`flex items-center gap-1.5 px-2.5 py-1 text-xs font-medium rounded-lg transition ${
              viewMode === 'renderedOnly'
                ? 'bg-zinc-800 text-cyan-400 border border-zinc-700/80 shadow-sm'
                : 'text-zinc-400 hover:text-zinc-200'
            }`}
            title="Risoprint render only"
          >
            <Eye className="w-3.5 h-3.5" />
            Single View
          </button>
        </div>

        {/* Compare target (Original vs Reference After Target) */}
        {viewMode !== 'renderedOnly' && (
          <div className="flex items-center gap-2 text-xs">
            <span className="text-zinc-400 font-medium">Compare with:</span>
            <div className="flex items-center bg-zinc-950 rounded-lg p-0.5 border border-zinc-800">
              <button
                onClick={() => setWipeCompareTarget('original')}
                className={`px-2 py-0.5 rounded text-[11px] font-semibold transition ${
                  wipeCompareTarget === 'original'
                    ? 'bg-zinc-800 text-white'
                    : 'text-zinc-400 hover:text-zinc-200'
                }`}
              >
                Original (IMG_4003)
              </button>
              <button
                onClick={() => setWipeCompareTarget('referenceTarget')}
                className={`px-2 py-0.5 rounded text-[11px] font-semibold transition ${
                  wipeCompareTarget === 'referenceTarget'
                    ? 'bg-cyan-950 text-cyan-300 border border-cyan-800'
                    : 'text-zinc-400 hover:text-zinc-200'
                }`}
              >
                Target Ref (IMG_4010)
              </button>
            </div>
          </div>
        )}

        {/* Right Actions: Upload custom image, export render, zoom */}
        <div className="flex items-center gap-2">
          {imageSource === 'custom' && (
            <button
              onClick={() => {
                setImageSource('default_horse');
                setCustomImgUrl(null);
              }}
              className="text-xs px-2.5 py-1 rounded-lg bg-zinc-800 text-zinc-300 hover:bg-zinc-700 flex items-center gap-1"
            >
              <RefreshCw className="w-3 h-3 text-cyan-400" />
              Reset to Horse
            </button>
          )}

          <label className="flex items-center gap-1.5 px-2.5 py-1 rounded-lg bg-zinc-800 hover:bg-zinc-700 text-xs font-medium text-zinc-200 cursor-pointer border border-zinc-700 transition">
            <Upload className="w-3.5 h-3.5 text-zinc-400" />
            Upload Image
            <input type="file" accept="image/*" onChange={handleUpload} className="hidden" />
          </label>

          <button
            onClick={handleDownloadRender}
            className="flex items-center gap-1.5 px-2.5 py-1 rounded-lg bg-cyan-950 hover:bg-cyan-900 text-xs font-semibold text-cyan-300 border border-cyan-800 transition"
            title="Download high-resolution PNG render"
          >
            <Download className="w-3.5 h-3.5" />
            Save PNG
          </button>
        </div>
      </div>

      {/* Main Viewport Stage */}
      <div className="relative flex-1 bg-[radial-gradient(#18181b_1px,transparent_1px)] [background-size:16px_16px] flex items-center justify-center p-4 overflow-auto min-h-[460px]">
        {/* Render mode pill indicator */}
        <div className="absolute top-4 left-4 z-20 flex items-center gap-2">
          {renderMode === 'bugged_simulation' ? (
            <div className="flex items-center gap-1.5 bg-rose-950/90 text-rose-300 text-xs px-3 py-1 rounded-full border border-rose-800 shadow-lg backdrop-blur">
              <AlertTriangle className="w-3.5 h-3.5 text-rose-400 animate-bounce" />
              <span>Simulating Bugged State (Offset Index &amp; Displaced Plates)</span>
            </div>
          ) : (
            <div className="flex items-center gap-1.5 bg-zinc-900/90 text-zinc-200 text-xs px-3 py-1 rounded-full border border-zinc-700/80 shadow-lg backdrop-blur">
              <CheckCircle2 className="w-3.5 h-3.5 text-emerald-400" />
              <span className="font-mono capitalize">
                {renderMode === 'composite' ? '3-Plate Composite' : renderMode}
              </span>
            </div>
          )}
        </div>

        {/* View mode 1: Wipe Slider */}
        {viewMode === 'wipe' && (
          <div
            className="relative border border-zinc-800 rounded-xl overflow-hidden shadow-2xl bg-black"
            style={{
              width: resolution.w * zoom,
              height: resolution.h * zoom,
              maxWidth: '100%',
            }}
          >
            {/* Background Layer: Original or Reference Target */}
            <img
              src={
                wipeCompareTarget === 'original'
                  ? customImgUrl || '/references/01_before/IMG_4003.jpeg'
                  : '/references/02_after/IMG_4010.jpeg'
              }
              alt="Compare reference"
              className="absolute inset-0 w-full h-full object-contain pointer-events-none select-none"
            />

            {/* Foreground Layer: Risoprint Render (Clipped by wipe slider) */}
            <div
              className="absolute inset-0 overflow-hidden"
              style={{
                clipPath: `polygon(${wipePosition}% 0, 100% 0, 100% 100%, ${wipePosition}% 100%)`,
              }}
            >
              <canvas
                ref={canvasRef}
                className="w-full h-full object-contain select-none pointer-events-none"
              />
            </div>

            {/* Wipe Divider Line */}
            <div
              className="absolute top-0 bottom-0 w-0.5 bg-white shadow-[0_0_10px_rgba(255,255,255,0.8)] z-10 pointer-events-none"
              style={{ left: `${wipePosition}%` }}
            >
              <div className="absolute top-1/2 -translate-y-1/2 -translate-x-1/2 w-7 h-7 bg-white text-zinc-950 rounded-full flex items-center justify-center text-[10px] font-bold shadow-lg border-2 border-zinc-900">
                ⬄
              </div>
            </div>

            {/* Wipe Drag Slider */}
            <input
              type="range"
              min={0}
              max={100}
              value={wipePosition}
              onChange={(e) => setWipePosition(Number(e.target.value))}
              className="absolute inset-0 w-full h-full opacity-0 cursor-ew-resize z-20"
            />

            {/* Labels */}
            <div className="absolute bottom-3 left-3 bg-zinc-900/80 backdrop-blur px-2 py-0.5 rounded text-[11px] font-mono text-zinc-300 z-10 pointer-events-none">
              {wipeCompareTarget === 'original' ? 'Original (IMG_4003)' : 'Target Ref (IMG_4010)'}
            </div>
            <div className="absolute bottom-3 right-3 bg-zinc-900/80 backdrop-blur px-2 py-0.5 rounded text-[11px] font-mono text-cyan-400 z-10 pointer-events-none">
              Risoprint Render
            </div>
          </div>
        )}

        {/* View mode 2: Side-by-Side */}
        {viewMode === 'sideBySide' && (
          <div className="flex flex-col md:flex-row items-center gap-4 max-w-full">
            <div className="space-y-1">
              <div className="text-xs font-mono text-zinc-400 flex items-center justify-between">
                <span>
                  {wipeCompareTarget === 'original'
                    ? 'Original Footage (IMG_4003)'
                    : 'Target Visual Reference (IMG_4010)'}
                </span>
                <span className="text-[10px] text-zinc-600">INPUT / REFERENCE</span>
              </div>
              <div
                className="border border-zinc-800 rounded-xl overflow-hidden shadow-xl bg-black"
                style={{
                  width: (resolution.w / 2) * zoom,
                  height: (resolution.h / 2) * zoom,
                }}
              >
                <img
                  src={
                    wipeCompareTarget === 'original'
                      ? customImgUrl || '/references/01_before/IMG_4003.jpeg'
                      : '/references/02_after/IMG_4010.jpeg'
                  }
                  alt="Reference input"
                  className="w-full h-full object-contain"
                />
              </div>
            </div>

            <div className="space-y-1">
              <div className="text-xs font-mono text-cyan-400 flex items-center justify-between">
                <span>risoprint Render (After Effects Native Engine)</span>
                <span className="text-[10px] text-cyan-600">OUTPUT WORLD</span>
              </div>
              <div
                className="border border-cyan-800/60 rounded-xl overflow-hidden shadow-xl bg-black"
                style={{
                  width: (resolution.w / 2) * zoom,
                  height: (resolution.h / 2) * zoom,
                }}
              >
                <canvas ref={canvasRef} className="w-full h-full object-contain" />
              </div>
            </div>
          </div>
        )}

        {/* View mode 3: Rendered Only */}
        {viewMode === 'renderedOnly' && (
          <div
            className="border border-zinc-800 rounded-xl overflow-hidden shadow-2xl bg-black"
            style={{
              width: resolution.w * zoom,
              height: resolution.h * zoom,
              maxWidth: '100%',
            }}
          >
            <canvas ref={canvasRef} className="w-full h-full object-contain" />
          </div>
        )}
      </div>

      {/* Bottom Status Bar */}
      <div className="border-t border-zinc-800/80 bg-zinc-900/80 px-4 py-2 flex flex-wrap items-center justify-between text-xs text-zinc-400">
        <div className="flex items-center gap-4">
          <span className="flex items-center gap-1.5">
            <span className="w-2 h-2 rounded-full bg-emerald-500 animate-pulse" />
            <span className="font-mono">{resolution.w} &times; {resolution.h} px</span>
          </span>

          <span className="font-mono text-zinc-400">
            Render: <strong className="text-zinc-200">{renderTimeMs} ms</strong>
          </span>

          <span className="hidden sm:inline font-mono text-zinc-400">
            Plates active: <strong className="text-cyan-400">{[params.cyanPlateEn, params.magentaPlateEn, params.yellowPlateEn, params.blackPlateEn].filter(Boolean).length}/4</strong>
          </span>
        </div>

        {/* Zoom controls */}
        <div className="flex items-center gap-2">
          <button
            onClick={() => setZoom((z) => Math.max(0.5, z - 0.1))}
            className="p-1 rounded bg-zinc-800 hover:bg-zinc-700 text-zinc-300"
            title="Zoom out"
          >
            <ZoomOut className="w-3.5 h-3.5" />
          </button>
          <span className="font-mono text-[11px] w-12 text-center">
            {Math.round(zoom * 100)}%
          </span>
          <button
            onClick={() => setZoom((z) => Math.min(2.0, z + 0.1))}
            className="p-1 rounded bg-zinc-800 hover:bg-zinc-700 text-zinc-300"
            title="Zoom in"
          >
            <ZoomIn className="w-3.5 h-3.5" />
          </button>
          <button
            onClick={() => setZoom(1)}
            className="p-1 rounded bg-zinc-800 hover:bg-zinc-700 text-zinc-300 text-[11px] font-mono px-1.5"
            title="Reset 100%"
          >
            1:1
          </button>
        </div>
      </div>
    </div>
  );
};
