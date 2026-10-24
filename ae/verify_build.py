import os,sys,struct

def verify(path):
    data=open(path,'rb').read()
    if data[:2]!=b'MZ': raise SystemExit('[FAIL] Not a PE file')
    pe=struct.unpack_from('<I',data,0x3c)[0]
    if data[pe:pe+4]!=b'PE\0\0': raise SystemExit('[FAIL] PE signature missing')
    nsec=struct.unpack_from('<H',data,pe+6)[0]; optsz=struct.unpack_from('<H',data,pe+20)[0]
    opt=pe+24; magic=struct.unpack_from('<H',data,opt)[0]
    is64=magic==0x20b
    if not is64: raise SystemExit('[FAIL] AEX is not x64')
    sec=opt+optsz
    sections=[]
    for i in range(nsec):
        off=sec+i*40; name=data[off:off+8].rstrip(b'\0').decode('latin1'); vs,va,rs,rp=struct.unpack_from('<IIII',data,off+8); sections.append((name,va,vs,rp,rs))
    rsrc=next((s for s in sections if s[0]=='.rsrc'),None)
    if not rsrc: raise SystemExit('[FAIL] .rsrc section missing')
    blob=data[rsrc[3]:rsrc[3]+rsrc[4]]
    if b'MIB8dnik' not in blob: raise SystemExit('[FAIL] PiPL signature missing')
    if b'EffectMain' not in blob: raise SystemExit('[FAIL] EffectMain missing')
    if b'ResoPrint' not in blob: raise SystemExit('[FAIL] ResoPrint metadata missing')
    print('[OK] PE x64 + PiPL + EffectMain + ResoPrint metadata verified')
    return True

if __name__=='__main__': verify(sys.argv[1] if len(sys.argv)>1 else 'dist/ResoPrint.aex')
