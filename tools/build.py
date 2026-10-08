#!/usr/bin/env python3
"""Build the standalone x86_64 Lilu plugin and donor symbol provider."""
import argparse,hashlib,json,plistlib,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def run(cmd): subprocess.run([str(x) for x in cmd],check=True)
def main():
 p=argparse.ArgumentParser(description=__doc__)
 p.add_argument('--sdk',type=Path,default=ROOT/'MacKernelSDK')
 p.add_argument('--lilu',type=Path,default=ROOT/'Lilu.kext')
 p.add_argument('--output',type=Path,default=ROOT/'build')
 p.add_argument('--release',action='store_true')
 a=p.parse_args();out=a.output.resolve();out.mkdir(parents=True,exist_ok=True)
 sdk=a.sdk.resolve();lilu=a.lilu.resolve()/'Contents/Resources'
 base=['xcrun','clang++','-arch','x86_64','-std=c++17',('-O2' if a.release else '-O0'),'-g','-mkernel','-fapple-kext','-fno-exceptions','-fno-rtti','-fno-stack-protector','-fno-builtin','-fno-asynchronous-unwind-tables','-mno-red-zone','-nostdinc','-DKERNEL','-DKERNEL_PRIVATE','-D__APPLE__','-D__MACH__','-I'+str(sdk/'Headers'),'-I'+str(lilu),'-I'+str(ROOT/'WhateverXe'),'-mmacosx-version-min=10.15','-Wall','-Wextra','-Wno-unused-parameter','-Wno-deprecated-declarations','-Wno-vla','-Wno-ossharedptr-misuse']
 manifest={}
 for name,version,sources in [('WhateverXe','0.1.0',sorted((ROOT/'WhateverXe').glob('*.cpp'))+[lilu/'Library/plugin_start.cpp']),('AppleIntelTGLCompat','0.4.0',[ROOT/'AppleIntelTGLCompat/AppleIntelTGLCompat.cpp'])]:
  bundle=out/(name+'.kext')/'Contents';(bundle/'MacOS').mkdir(parents=True,exist_ok=True);objdir=out/name;objdir.mkdir(exist_ok=True)
  objects=[]
  for source in sources:
   obj=objdir/(source.stem+'.o');cmd=base+['-DPRODUCT_NAME='+name,'-DMODULE_VERSION='+version]
   if not a.release:cmd+=['-DDEBUG=1']
   run(cmd+['-c',source,'-o',obj]);objects.append(obj)
  exe=bundle/'MacOS'/name
  run(['xcrun','ld','-arch','x86_64','-kext','-undefined','dynamic_lookup','-no_fixup_chains','-macos_version_min','10.15','-o',exe,*objects,sdk/'Library/x86_64/libkmod.a'])
  libs={x:'10.0.0' for x in ['com.apple.kpi.bsd','com.apple.kpi.dsep','com.apple.kpi.iokit','com.apple.kpi.libkern','com.apple.kpi.mach','com.apple.kpi.unsupported']};libs['com.apple.iokit.IOPCIFamily']='1.1'
  ident='org.b00t0x.'+name
  info=dict(CFBundleIdentifier=ident,CFBundleExecutable=name,CFBundleName=name,CFBundlePackageType='KEXT',CFBundleVersion=version,CFBundleShortVersionString=version,OSBundleCompatibleVersion=version,OSBundleRequired='Root',OSBundleLibraries=libs)
  if name=='WhateverXe':
   libs['as.vit9696.Lilu']='1.2.0'
   info['IOKitPersonalities']={ident:dict(CFBundleIdentifier=ident,IOClass=name,IOMatchCategory=name,IOProviderClass='IOResources',IOResourceMatch='IOKit')}
  (bundle/'Info.plist').write_bytes(plistlib.dumps(info))
  symbols=subprocess.check_output(['xcrun','nm',str(exe)],text=True)
  assert ' _kmod_info' in symbols, 'missing kext lifecycle metadata'
  if name=='WhateverXe':
   assert ' __realmain' in symbols and ' __start' in symbols
   unresolved=subprocess.check_output(['xcrun','nm','-u',str(exe)],text=True)
   assert not any(x in unresolved for x in ['__ZN2XE','__ZN6XeHost','__ZN11DVMTCalcFix']), 'unresolved local implementation'
  manifest[name]={'identifier':ident,'sha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'source_sha256':{str(s.relative_to(ROOT)) if s.is_relative_to(ROOT) else str(s):hashlib.sha256(s.read_bytes()).hexdigest() for s in sources}}
 manifest['headers']={str(p.relative_to(ROOT)):hashlib.sha256(p.read_bytes()).hexdigest() for d in ['WhateverXe','AppleIntelTGLCompat'] for p in sorted((ROOT/d).glob('*.hpp'))}
 (out/'build-manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
 print(json.dumps({n:v['sha256'] for n,v in manifest.items() if n!='headers'},indent=2))
if __name__=='__main__':main()
