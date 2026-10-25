import React, { useState } from 'react';
import confetti from 'canvas-confetti';
import { Header } from './components/Header';
import { ControlPanel } from './components/ControlPanel';
import { ViewportViewer } from './components/ViewportViewer';
import { DiagnosticInspector } from './components/DiagnosticInspector';
import { VisualReferences } from './components/VisualReferences';
import { SourceCodeViewer } from './components/SourceCodeViewer';
import { DEFAULT_PARAMS, RisoParams, RenderMode } from './engine/risoprintEngine';
import { downloadFixedRepositoryZip } from './utils/zipExporter';
import { CheckCircle2, AlertCircle } from 'lucide-react';

export default function App() {
  const [activeTab, setActiveTab] = useState<'studio' | 'diagnostics' | 'references' | 'code'>('studio');
  const [params, setParams] = useState<RisoParams>({ ...DEFAULT_PARAMS });
  const [renderMode, setRenderMode] = useState<RenderMode>('composite');
  const [isDownloading, setIsDownloading] = useState<boolean>(false);
  const [toastMessage, setToastMessage] = useState<string | null>(null);

  const showToast = (msg: string) => {
    setToastMessage(msg);
    setTimeout(() => setToastMessage(null), 4000);
  };

  const handleDownloadZip = async () => {
    try {
      setIsDownloading(true);
      await downloadFixedRepositoryZip();
      confetti({
        particleCount: 60,
        spread: 70,
        origin: { y: 0.1, x: 0.85 },
        colors: ['#50D9F1', '#BF7700', '#2900FF', '#FFFFFF'],
      });
      showToast('Fixed repository packaged and downloaded successfully!');
    } catch (err: any) {
      showToast(`Error packaging ZIP: ${err?.message || 'Download failed'}`);
    } finally {
      setIsDownloading(false);
    }
  };

  return (
    <div className="min-h-screen flex flex-col bg-zinc-950 text-zinc-100 font-sans selection:bg-cyan-500/30">
      {/* App Header */}
      <Header
        activeTab={activeTab}
        setActiveTab={setActiveTab}
        onDownloadZip={handleDownloadZip}
        isDownloading={isDownloading}
      />

      {/* Toast Notification */}
      {toastMessage && (
        <div className="fixed bottom-5 right-5 z-50 flex items-center gap-2 bg-zinc-900 border border-zinc-700 text-white text-xs font-medium px-4 py-3 rounded-xl shadow-2xl animate-fade-in backdrop-blur">
          <CheckCircle2 className="w-4 h-4 text-emerald-400" />
          <span>{toastMessage}</span>
        </div>
      )}

      {/* Main Content Area */}
      <main className="flex-1 flex flex-col min-w-0">
        {activeTab === 'studio' && (
          <div className="flex-1 flex flex-col lg:flex-row gap-4 p-4 lg:p-6 overflow-hidden max-w-[1920px] mx-auto w-full">
            {/* Left/Main: Canvas Viewport */}
            <ViewportViewer params={params} renderMode={renderMode} />

            {/* Right: Controls Panel */}
            <ControlPanel
              params={params}
              onChange={setParams}
              renderMode={renderMode}
              setRenderMode={setRenderMode}
            />
          </div>
        )}

        {activeTab === 'diagnostics' && <DiagnosticInspector />}

        {activeTab === 'references' && <VisualReferences />}

        {activeTab === 'code' && <SourceCodeViewer />}
      </main>
    </div>
  );
}
