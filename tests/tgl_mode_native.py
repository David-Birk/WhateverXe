#!/usr/bin/env python3
"""Replay qualified native TGL setPortMode with the production result repair."""
from pathlib import Path
import argparse,hashlib,re,subprocess,tempfile
p=argparse.ArgumentParser(description=__doc__);p.add_argument('donor',type=Path);p.add_argument('--serialization',action='store_true');p.add_argument('--source',type=Path);a=p.parse_args()
if hashlib.sha256(a.donor.read_bytes()).hexdigest()!='c004fc1ac9ee4157e2e12736f40fa6639bde025a279326beeb9dd48c9b445731':p.error('unqualified donor SHA256')
r=Path(__file__).resolve().parents[1];s=(a.source or r/'WhateverXe/kern_xe.cpp').read_text()
# Verify the production admission bytes, not just the fixture's ready flag.
donor=a.donor.read_bytes()
for name,offset in [('modeEntry',0x5fcb8),('modeFailure',0x5ffeb),('modeReturn',0x6010e)]:
 values=re.search(r'const uint8_t '+name+r'\[\] \{([^}]+)\}',s).group(1)
 expected=bytes(int(v.strip(),0) for v in values.split(','))
 assert donor[offset:offset+len(expected)]==expected,name
body=re.search(r'uint32_t XE::wrapTGLSetPortMode\(.*?\n\}',s,re.S).group()
f=(r/'tests'/('tgl_mode_serial_fixture.cpp' if a.serialization else 'tgl_mode_native_fixture.cpp')).read_text().replace('// PRODUCTION_WRAPPER',body)
with tempfile.TemporaryDirectory(prefix='tgl-mode-') as d:
 d=Path(d);(d/'check.cpp').write_text(f)
 subprocess.run(['clang++','-arch','x86_64','-std=c++14','-O1','-g','-pthread','-I',str(r/'WhateverXe'),str(d/'check.cpp'),'-o',str(d/'check')],check=True)
 subprocess.run([str(d/'check'),str(a.donor.resolve())],check=True)
