set PATH=%PATH%;%~dp0

:ffmpeg -b 5000 -y -i image\output_W%%06d.bmp -speed 0.01 -qscale 7 simulation.swf
ffmpeg  -r 6 -y -i image\output_W%%06d.bmp -codec msmpeg4v2 -qscale 7 simulation.avi
ffmpeg  -r 6 -y -i image\output_W%%06d.bmp simulation.gif



