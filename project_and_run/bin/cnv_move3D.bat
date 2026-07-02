set PATH=%PATH%;%~dp0

:ffmpeg.exe -y  -r 6 -i image3D\output_%%06d.bmp  -qscale 6 -b 500k simulation3D_%1.swf
:ffmpeg -r 8 -y -i image3D\output_%%06d.bmp  -codec msmpeg4v2 -qscale 7 simulation3D.avi
:ffmpeg -r 8 -i image3D\output_%%06d.bmp -vcodec libx264  -pix_fmt yuv420p simulation3D.mp4

:ffmpeg -r 8 -y -i image3D\output_%%06d.bmp  -codec msmpeg4v2 -q 0 simulation3D.avi

ffmpeg  -r 8 -y  -start_number 0 -i image3D\output_%%06d.bmp -vf "pad=ceil(iw/2)*2:ceil(ih/2)*2" -c:v libx264 -pix_fmt yuv420p simulation3D.mp4
ffmpeg -y -r 5 -i simulation3D.mp4 -filter_complex "[0:v] fps=20,scale=608:-1,split [a][b];[a] palettegen [p];[b][p] paletteuse" simulation3D.gif
