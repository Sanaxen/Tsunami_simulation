set ldmpath=..\bin

set ALPHA_BLEND_VALUE=0.6

:set MOVE_STEP=80
set PERSPECTIVE=1
set COLORMAP=wave_colormap7.bmp
set COLORMAP=wave_colormap6a.bmp
:set COLORMAP=wave_colormap0.bmp
:set COLORMAP=wave_colormap9.bmp
set COLORMAP=wave_colormap11.bmp

%ldmpath%\TSUNAMI_Viewr3D.exe . -offscreen 1

:..\bin\reindex.exe image3D
:ffmpeg -b 5000 -y -i image\output_%%06d.bmp -speed 0.01 -qscale 7 simulation3D.swf

:"C:\Program Files\ImageMagick-6.8.0-Q16\convert.exe"  -geometry 90%% image\output_0*.bmp simulation3D.gif