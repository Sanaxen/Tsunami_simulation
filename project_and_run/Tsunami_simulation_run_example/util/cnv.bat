:ffmpeg -b 5000 -y -i image\output_W%%06d.bmp -speed 0.01 -qscale 7 simulation.swf
ffmpeg  -r 6 -y -i image\output_W%%06d.bmp -codec msmpeg4v2 -qscale 7 simulation.avi
ffmpeg  -r 6 -y -i image\output_W%%06d.bmp aaa.gif

type tsunami_template0.html > test.html
echo Test >> test.html
type tsunami_template1.html >> test.html
echo simulation.swf >> test.html
type tsunami_template2.html >> test.html


