import React from 'react';
import { Download, CheckCircle2, Layers, Cpu, Image as ImageIcon, Code2, Sparkles } from 'lucide-react';

interface HeaderProps {
  activeTab: 'studio' | 'diagnostics' | 'references' | 'code';
  setActiveTab: (tab: 'studio' | 'diagnostics' | 'references' | 'code') => void;
  onDownloadZip: () => void;
  isDownloading: boolean;
}

export const Header: React.FC<HeaderProps> = ({
  activeTab,
  setActiveTab,
  onDownloadZip,
  isDownloading,
}) => {
  return (
    <header className="border-b border-zinc-800 bg-zinc-900/90 backdrop-blur sticky top-0 z-50 px-4 lg:px-6 py-3">
      <div className="flex flex-col md:flex-row md:items-center justify-between gap-3">
        {/* Brand & Status */}
        <div className="flex items-center gap-3">
          <div className="relative flex items-center justify-center w-10 h-10 rounded-xl bg-zinc-800 border border-zinc-700 shadow-inner">
            <span className="w-3 h-3 rounded-full bg-[#50D9F1] absolute -top-0.5 -left-0.5 opacity-90 shadow-[0_0_8px_#50D9F1]" />
            <span className="w-3 h-3 rounded-full bg-[#BF7700] absolute -bottom-0.5 -right-0.5 opacity-90 shadow-[0_0_8px_#BF7700]" />
            <span className="w-3 h-3 rounded-full bg-[#2900FF] absolute top-1.5 right-1 opacity-90 shadow-[0_0_8px_#2900FF]" />
            <Layers className="w-5 h-5 text-zinc-200 relative z-10" />
          </div>

          <div>
            <div className="flex items-center gap-2">
              <h1 className="text-lg font-bold tracking-tight text-white flex items-center gap-2 font-mono">
                risoprint
                <span className="text-xs px-2 py-0.5 rounded-full font-sans font-semibold bg-cyan-950 text-cyan-400 border border-cyan-800">
                  AE .aex x64
                </span>
              </h1>
              <span className="hidden sm:inline-flex items-center gap-1 text-[11px] font-medium text-emerald-400 bg-emerald-950/60 border border-emerald-800/80 px-2 py-0.5 rounded-md">
                <CheckCircle2 className="w-3 h-3 text-emerald-400" />
                Root Causes Fixed
              </span>
            </div>
            <p className="text-xs text-zinc-400">
              Native 3-Plate Risograph After Effects Effect Engine &amp; Test Suite
            </p>
          </div>
        </div>

        {/* Navigation Tabs */}
        <div className="flex items-center gap-1 bg-zinc-950/80 p-1 rounded-xl border border-zinc-800/80 self-start md:self-auto overflow-x-auto max-w-full">
          <button
            onClick={() => setActiveTab('studio')}
            className={`flex items-center gap-1.5 px-3 py-1.5 rounded-lg text-xs font-medium transition-all whitespace-nowrap ${
              activeTab === 'studio'
                ? 'bg-zinc-800 text-white shadow-sm border border-zinc-700/60'
                : 'text-zinc-400 hover:text-zinc-200 hover:bg-zinc-900'
            }`}
          >
            <Sparkles className="w-3.5 h-3.5 text-cyan-400" />
            Interactive Studio
          </button>

          <button
            onClick={() => setActiveTab('diagnostics')}
            className={`flex items-center gap-1.5 px-3 py-1.5 rounded-lg text-xs font-medium transition-all whitespace-nowrap ${
              activeTab === 'diagnostics'
                ? 'bg-zinc-800 text-white shadow-sm border border-zinc-700/60'
                : 'text-zinc-400 hover:text-zinc-200 hover:bg-zinc-900'
            }`}
          >
            <Cpu className="w-3.5 h-3.5 text-amber-400" />
            Root Causes &amp; Audit
          </button>

          <button
            onClick={() => setActiveTab('references')}
            className={`flex items-center gap-1.5 px-3 py-1.5 rounded-lg text-xs font-medium transition-all whitespace-nowrap ${
              activeTab === 'references'
                ? 'bg-zinc-800 text-white shadow-sm border border-zinc-700/60'
                : 'text-zinc-400 hover:text-zinc-200 hover:bg-zinc-900'
            }`}
          >
            <ImageIcon className="w-3.5 h-3.5 text-indigo-400" />
            Visual References
          </button>

          <button
            onClick={() => setActiveTab('code')}
            className={`flex items-center gap-1.5 px-3 py-1.5 rounded-lg text-xs font-medium transition-all whitespace-nowrap ${
              activeTab === 'code'
                ? 'bg-zinc-800 text-white shadow-sm border border-zinc-700/60'
                : 'text-zinc-400 hover:text-zinc-200 hover:bg-zinc-900'
            }`}
          >
            <Code2 className="w-3.5 h-3.5 text-emerald-400" />
            Source Files &amp; CMake
          </button>
        </div>

        {/* Action Button: Download Fixed Repo ZIP */}
        <div className="flex items-center gap-2">
          <button
            onClick={onDownloadZip}
            disabled={isDownloading}
            className="flex items-center gap-2 px-3.5 py-1.5 rounded-xl bg-gradient-to-r from-cyan-600 to-blue-600 hover:from-cyan-500 hover:to-blue-500 text-white text-xs font-semibold shadow-md shadow-cyan-950 transition-all active:scale-95 disabled:opacity-50"
            title="Download the complete updated repository with all C++ and CMake fixes as a ZIP file"
          >
            <Download className={`w-3.5 h-3.5 ${isDownloading ? 'animate-bounce' : ''}`} />
            {isDownloading ? 'Packaging ZIP...' : 'Download Fixed Repo (.zip)'}
          </button>
        </div>
      </div>
    </header>
  );
};
