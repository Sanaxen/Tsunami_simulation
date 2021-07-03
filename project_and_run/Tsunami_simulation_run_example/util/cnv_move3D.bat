set ImageMagic="C:\Program Files\ImageMagick-6.8.0-Q16"
:set PATH=%PATH%;"D:\work\tu_sim\Tools\ffmpeg-20130212-git-1ac0fa5-win64-static\bin"

:%ldm% -geometry 400%% image3D\*.bmp simulation3D.gif
:%ldm% -geometry 90%% image3D\output_*.bmp simulation3D.gif

:%ImageMagic%\convert.exe  -geometry 90%% image3D\output_*.bmp simulation3D.gif
%ImageMagic%\convert.exe  -geometry 90%% image3D\*.bmp simulation3D.avi

:ffmpeg.exe -y -i image3D\%%03d.bmp -qscale 7 -vcodec wmv1 simulation3D.avi
:ffmpeg.exe -y  -r 6 -i image3D\output_%%06d.bmp  -qscale 6 -b 500k simulation3D_%1.swf
:ffmpeg -r 8 -y -i image3D\output_%%06d.bmp  -codec msmpeg4v2 -qscale 7 simulation3D.avi

ffmpeg -r 8 -y -i image3D\%%03d.bmp  -codec msmpeg4v2 -q 0 simulation3D.avi
:ffmpeg -r 8 -i .\image3D\%%03d.bmp -vcodec libx264  -pix_fmt yuv420p simulation3D.mp4
pause
ffmpeg -r 8 -y -i image\output_%%06d.bmp  -codec msmpeg4v2 -qscale 7 simulation.avi

convert.exe  -geometry 30%% image3D\output_*.bmp simulation3D.gif


type tsunami_template0.html > simulation3D_%1.html
echo Simulation3D Test %1 >> simulation3D_%1.html
type tsunami_template1.html >> simulation3D_%1.html
echo simulation3D_%1.swf >> simulation3D_%1.html
type tsunami_template2.html >> simulation3D_%1.html
