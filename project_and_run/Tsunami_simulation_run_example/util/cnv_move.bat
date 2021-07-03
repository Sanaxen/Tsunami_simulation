set ldm="C:\Program Files\ImageMagick-6.8.0-Q16\convert.exe"
set ldm=convert.exe

%ldm% -geometry 10%% image\output*.bmp aaa.gif
:%ldm% -geometry 99%% render\render_output*.bmp render.gif

