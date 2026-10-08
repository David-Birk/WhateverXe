#pragma once
#include <Headers/kern_util.hpp>
// Legacy spellings are migration aliases, not CPU admission.
inline bool xeCheckKernelArgument(const char *name) {
 if(checkKernelArgument(name))return true;
 if(!strcmp(name,"-adlnconfigstage"))return checkKernelArgument("-n97configstage");
 if(!strcmp(name,"-adlnctxpublish"))return checkKernelArgument("-n97ctxpublish");
 if(!strcmp(name,"-display13"))return checkKernelArgument("-n97display13");
 if(!strcmp(name,"-adlndisplaystage"))return checkKernelArgument("-n97displaystage");
 if(!strcmp(name,"-display13trace"))return checkKernelArgument("-n97displaytrace");
 if(!strcmp(name,"-adlneventstage"))return checkKernelArgument("-n97eventstage");
 if(!strcmp(name,"-adlngoldenraw"))return checkKernelArgument("-n97goldenraw");
 if(!strcmp(name,"-adlngoldensource"))return checkKernelArgument("-n97goldensource");
 if(!strcmp(name,"-adlngpu"))return checkKernelArgument("-n97gpu");
 if(!strcmp(name,"-adlngt"))return checkKernelArgument("-n97gt");
 if(!strcmp(name,"-adlninitialnopocs"))return checkKernelArgument("-n97initialnopocs");
 if(!strcmp(name,"-adlnkernelctx"))return checkKernelArgument("-n97kernelctx");
 if(!strcmp(name,"-adlnkernelearly"))return checkKernelArgument("-n97kernelearly");
 if(!strcmp(name,"-adlnkernelgl"))return checkKernelArgument("-n97kernelgl");
 if(!strcmp(name,"-adlnkernelidlegap"))return checkKernelArgument("-n97kernelidlegap");
 if(!strcmp(name,"-adlnkernelnewctx"))return checkKernelArgument("-n97kernelnewctx");
 if(!strcmp(name,"-adlnkernelrenderwake"))return checkKernelArgument("-n97kernelrenderwake");
 if(!strcmp(name,"-adlnkernelreuse"))return checkKernelArgument("-n97kernelreuse");
 if(!strcmp(name,"-adlnkernelwake"))return checkKernelArgument("-n97kernelwake");
 if(!strcmp(name,"-adlnloadonly"))return checkKernelArgument("-n97loadonly");
 if(!strcmp(name,"-adlnmemorystage"))return checkKernelArgument("-n97memorystage");
 if(!strcmp(name,"-adlnnative"))return checkKernelArgument("-n97native");
 if(!strcmp(name,"-adlnparententry"))return checkKernelArgument("-n97parententry");
 if(!strcmp(name,"-adlnparentonly"))return checkKernelArgument("-n97parentonly");
 if(!strcmp(name,"-adlnprobe"))return checkKernelArgument("-n97probe");
 if(!strcmp(name,"-adlnqtrace"))return checkKernelArgument("-n97qtrace");
 if(!strcmp(name,"-adlnrcsnopocs"))return checkKernelArgument("-n97rcsnopocs");
 if(!strcmp(name,"-adlnrcsprofile"))return checkKernelArgument("-n97rcsprofile");
 if(!strcmp(name,"-adlnscanout"))return checkKernelArgument("-n97scanout");
 if(!strcmp(name,"-adlntaskstage"))return checkKernelArgument("-n97taskstage");
 if(!strcmp(name,"adln-qtrace"))return checkKernelArgument("n97-qtrace");
 return false;
}
