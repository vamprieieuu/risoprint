import JSZip from 'jszip';

export async function downloadFixedRepositoryZip(): Promise<void> {
  // First attempt: direct download of the pre-built repository zip
  try {
    const res = await fetch('/risoprint-ae-fixed-repository.zip');
    if (res.ok) {
      const blob = await res.blob();
      const url = URL.createObjectURL(blob);
      const a = document.createElement('a');
      a.href = url;
      a.download = 'risoprint-ae-fixed-repository.zip';
      document.body.appendChild(a);
      a.click();
      document.body.removeChild(a);
      URL.revokeObjectURL(url);
      return;
    }
  } catch {
    // Fall back to client-side packaging
  }

  // Client-side fallback packaging via JSZip
  const zip = new JSZip();

  const files = [
    'CMakeLists.txt',
    'README.md',
    'CHANGELOG.md',
    '.gitignore',
    '.gitattributes',
    '.github/workflows/build.yml',
    'handoff/IMPLEMENTATION_BRIEF.md',
    'core/risoprint_core.h',
    'core/risoprint_core.cpp',
    'ae/risoprint_ae.cpp',
    'ae/risoprint.def',
    'ae/risoprint_PiPL.r',
    'ae/risoprint_Version.rc',
    'ae/pipl_gen.py',
    'ae/verify_build.py',
    'tests/verify_plugin_code.py',
  ];

  for (const path of files) {
    try {
      const r = await fetch(`/${path}`);
      if (r.ok) {
        zip.file(path, await r.text());
      }
    } catch {
      // Ignore missing
    }
  }

  const binaryAssets = [
    'references/01_before/IMG_4003.jpeg',
    'references/02_after/IMG_4010.jpeg',
    'references/03_plugin_settings/IMG_4011.png',
    'references/03_plugin_settings/IMG_4012.png',
    'references/03_plugin_settings/IMG_4013.png',
  ];

  for (const bpath of binaryAssets) {
    try {
      const r = await fetch(`/${bpath}`);
      if (r.ok) {
        zip.file(bpath, await r.blob());
      }
    } catch {
      // Ignore
    }
  }

  const content = await zip.generateAsync({ type: 'blob' });
  const url = URL.createObjectURL(content);
  const a = document.createElement('a');
  a.href = url;
  a.download = 'risoprint-ae-fixed-repository.zip';
  document.body.appendChild(a);
  a.click();
  document.body.removeChild(a);
  URL.revokeObjectURL(url);
}
