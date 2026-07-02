set PATH=%PATH%;%~dp0

:ffmpeg -b 5000 -y -i image\output_W%%06d.bmp -speed 0.01 -qscale 7 simulation.swf
:ffmpeg  -r 6 -y -i image\output_W%%06d.bmp -codec msmpeg4v2 -qscale 7 simulation.avi
:ffmpeg  -r 6 -y -i image\output_W%%06d.bmp simulation.gif

ffmpeg  -y  -start_number 0 -i image\output_W%%06d.bmp -vf "pad=ceil(iw/2)*2:ceil(ih/2)*2" -c:v libx264 -pix_fmt yuv420p simulation.mp4
ffmpeg -y -r 5 -i simulation.mp4 -filter_complex "[0:v] fps=20,scale=608:-1,split [a][b];[a] palettegen [p];[b][p] paletteuse" simulation.gif

