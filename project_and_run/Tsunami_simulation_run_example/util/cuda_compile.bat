nvcc -c DC3D.cu -Xcompiler "/GS /Qpar /GL /W3 /Gy /Zc:wchar_t /Zi /Gm- /Ox /Ob2 /Zc:inline  /WX- /Zc:forScope /arch:AVX2 /Gd /Oi /MT /openmp /EHsc /nologo  /Ot"

nvcc -c fault_deformation.cu -Xcompiler "/GS /Qpar /GL /W3 /Gy /Zc:wchar_t /Zi /Gm- /Ox /Ob2 /Zc:inline  /WX- /Zc:forScope /arch:AVX2 /Gd /Oi /MT /openmp /EHsc /nologo  /Ot"

