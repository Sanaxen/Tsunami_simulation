set PATH=%PATH%;%~dp0

:ffmpeg.exe -y  -r 6 -i image3D\output_%%06d.bmp  -qscale 6 -b 500k simulation3D_%1.swf
ffmpeg -r 8 -y -i image3D\output_%%06d.bmp  -codec msmpeg4v2 -qscale 7 simulation3D.avi
ffmpeg -r 8 -i image3D\output_%%06d.bmp -vcodec libx264  -pix_fmt yuv420p simulation3D.mp4

:ffmpeg -r 8 -y -i image3D\output_%%06d.bmp  -codec msmpeg4v2 -q 0 simulation3D.avi
