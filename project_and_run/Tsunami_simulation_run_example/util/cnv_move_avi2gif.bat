set ldm=convert.exe


:%ldm% -loop 0 -delay 1 -geometry 35%% %1 %1.gif
:%ldm%  -geometry 35%% %1 %1.gif
:%ldm%  -geometry 35%% image3D\output_*.bmp %1_img.gif

%ldm% -delay 50 -geometry 50%% C:\Users\neutral\Desktop\yamato\レンダラ\パストレーシング\test\image\*.bmp xxx.gif