import React, { useState, useEffect } from 'react';
import { FileCode, Copy, Check, Download } from 'lucide-react';

interface FileItem {
  id: string;
  name: string;
  path: string;
  lang: string;
  url: string;
}

export const SourceCodeViewer: React.FC = () => {
  const files: FileItem[] = [
    {
      id: 'ae_cpp',
      name: 'risoprint_ae.cpp',
      path: 'ae/risoprint_ae.cpp',
      lang: 'cpp',
      url: '/ae/risoprint_ae.cpp',
    },
    {
      id: 'core_cpp',
      name: 'risoprint_core.cpp',
      path: 'core/risoprint_core.cpp',
      lang: 'cpp',
      url: '/core/risoprint_core.cpp',
    },
    {
      id: 'core_h',
      name: 'risoprint_core.h',
      path: 'core/risoprint_core.h',
      lang: 'cpp',
      url: '/core/risoprint_core.h',
    },
    {
      id: 'cmake',
      name: 'CMakeLists.txt',
      path: 'CMakeLists.txt',
      lang: 'cmake',
      url: '/CMakeLists.txt',
    },
    {
      id: 'changelog',
      name: 'CHANGELOG.md',
      path: 'CHANGELOG.md',
      lang: 'markdown',
      url: '/CHANGELOG.md',
    },
    {
      id: 'workflow',
      name: 'build.yml (MSVC CI)',
      path: '.github/workflows/build.yml',
      lang: 'yaml',
      url: '/.github/workflows/build.yml',
    },
  ];

  const [activeFileId, setActiveFileId] = useState<string>('ae_cpp');
  const [content, setContent] = useState<string>('Loading file...');
  const [copied, setCopied] = useState<boolean>(false);

  const activeFile = files.find((f) => f.id === activeFileId) || files[0];

  useEffect(() => {
    let isCurrent = true;
    fetch(activeFile.url)
      .then((r) => {
        if (!r.ok) throw new Error(`HTTP ${r.status}`);
        return r.text();
      })
      .then((txt) => {
        if (isCurrent) setContent(txt);
      })
      .catch((err) => {
        if (isCurrent) setContent(`// Error loading ${activeFile.path}: ${err.message}`);
      });

    return () => {
      isCurrent = false;
    };
  }, [activeFile]);

  const copyToClipboard = () => {
    navigator.clipboard.writeText(content);
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  const downloadSingleFile = () => {
    const blob = new Blob([content], { type: 'text/plain' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = activeFile.name;
    a.click();
    URL.revokeObjectURL(url);
  };

  return (
    <div className="flex-1 flex flex-col p-4 lg:p-6 overflow-hidden max-w-7xl mx-auto w-full">
      {/* Top File Selector Tabs */}
      <div className="flex flex-col sm:flex-row sm:items-center justify-between pb-3 gap-3 border-b border-zinc-800">
        <div className="flex items-center gap-1.5 overflow-x-auto pb-1 max-w-full">
          {files.map((file) => {
            const isSelected = file.id === activeFileId;
            return (
              <button
                key={file.id}
                onClick={() => setActiveFileId(file.id)}
                className={`flex items-center gap-2 px-3 py-1.5 rounded-lg text-xs font-mono transition whitespace-nowrap ${
                  isSelected
                    ? 'bg-zinc-800 text-cyan-400 border border-zinc-700 shadow-sm'
                    : 'text-zinc-400 hover:text-zinc-200 hover:bg-zinc-900'
                }`}
              >
                <FileCode className="w-3.5 h-3.5" />
                {file.name}
              </button>
            );
          })}
        </div>

        {/* Action Buttons */}
        <div className="flex items-center gap-2 flex-shrink-0">
          <button
            onClick={copyToClipboard}
            className="flex items-center gap-1.5 px-3 py-1.5 rounded-lg bg-zinc-800 hover:bg-zinc-700 text-xs font-medium text-zinc-300 border border-zinc-700 transition"
          >
            {copied ? <Check className="w-3.5 h-3.5 text-emerald-400" /> : <Copy className="w-3.5 h-3.5" />}
            {copied ? 'Copied!' : 'Copy Code'}
          </button>

          <button
            onClick={downloadSingleFile}
            className="flex items-center gap-1.5 px-3 py-1.5 rounded-lg bg-zinc-800 hover:bg-zinc-700 text-xs font-medium text-zinc-300 border border-zinc-700 transition"
          >
            <Download className="w-3.5 h-3.5" />
            Save File
          </button>
        </div>
      </div>

      {/* Code Editor Style View */}
      <div className="flex-1 mt-4 border border-zinc-800 rounded-xl bg-zinc-950 overflow-hidden flex flex-col shadow-2xl">
        <div className="bg-zinc-900/80 px-4 py-2 border-b border-zinc-800 flex items-center justify-between text-xs font-mono text-zinc-400">
          <span>{activeFile.path}</span>
          <span className="uppercase text-[10px] text-zinc-500">{activeFile.lang}</span>
        </div>

        <div className="flex-1 p-4 overflow-auto font-mono text-xs text-zinc-300 leading-relaxed select-text">
          <pre className="whitespace-pre">
            <code>{content}</code>
          </pre>
        </div>
      </div>
    </div>
  );
};
