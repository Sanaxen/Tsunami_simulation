:set ldm="C:\Program Files\ImageMagick-6.9.1-Q16\convert.exe"

cd .\google_earth
:goto end

mogrify.exe -format png -transparent black output*.bmp
:mogrify.exe -fill "#0058FD80" -opaque "#0058FD00" *.png

:mogrify.exe -format png output*.bmp
type header.txt > test.kml
for %%x in (image*) do type %%x >> test.kml
type footer.txt >> test.kml

:end
mogrify.exe -format png -transparent black runup*.bmp
mogrify.exe -format png -transparent black chkpoint_data*.bmp
:del /Q *.bmp
cd ..
