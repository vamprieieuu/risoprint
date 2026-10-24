#!/usr/bin/env python3
import sys,struct,os

def ps(s):
    b=bytes([len(s)])+s.encode('latin1'); return b+b'\0'*((-len(b))%4)

def gen(r_src,rc_out,bin_out):
    props=[('dnik',b'TKFe'),('eman',ps('risoprint')),('gtac',ps('risoprint')),('4668',b'EffectMain\0\0'),('RVPe',struct.pack('<I',2)),('RVSe',struct.pack('<HH',13,28)),('REVe',bytes.fromhex('01060800')),('FNIe',b'\0\0'),('OLGe',struct.pack('<I',0x02008004)),('2LGe',struct.pack('<I',0x08001400)),('ANMe',ps('risoprint')),('LFea',struct.pack('<I',8)),('LRUe',ps('https://nexus.synkit.net/effects/risograph-effect'))]
    d=bytearray(struct.pack('<IHH',1,0,len(props))+b'\0\0')
    for k,v in props:
        d+=b'MIB8'+k.encode()+struct.pack('<II',0,len(v))+v+b'\0'*((-len(v))%4)
    os.makedirs(os.path.dirname(os.path.abspath(bin_out)),exist_ok=True); open(bin_out,'wb').write(d)
    rc=os.path.abspath(rc_out).replace(os.sep,'/')
    os.makedirs(os.path.dirname(rc),exist_ok=True); open(rc_out,'w',encoding='utf-8').write(f'LANGUAGE 9, 1\n16000 PIPL "{os.path.abspath(bin_out).replace(os.sep,"/")}"\n')
    print(f'Generated risoprint PiPL: {len(d)} bytes')
if __name__=='__main__':
    if len(sys.argv)!=4: raise SystemExit('Usage: pipl_gen.py <r> <rc> <bin>')
    gen(*sys.argv[1:])
