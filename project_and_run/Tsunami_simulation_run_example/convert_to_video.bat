..\bin\ffmpeg -r 2 -y -i image3D/output_%%06d.bmp -vcodec libx264 -pix_fmt yuv420p -vf "scale=trunc(iw/2)*2:trunc(ih/2)*2" sim_.mp4
..\bin\ffmpeg -i sim_.mp4 -filter_complex "[0:v] fps=10,scale=640:-1,split [a][b];[a] palettegen [p];[b][p] paletteuse" sim_.gif