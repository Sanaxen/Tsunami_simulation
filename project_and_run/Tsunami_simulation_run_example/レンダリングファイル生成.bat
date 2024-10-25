obj2pov.exe "obj\elevation_data.obj" elevation_data.tmp elevation_data
obj2pov.exe "obj\water_depth_data.obj" water_depth_data.tmp water_depth_data
obj2pov.exe "obj\output_000000.obj" 0_tmp 000000
type elevation_data.tmp > tmp
type water_depth_data.tmp >> tmp
type 0_tmp >> tmp
del 0_tmp
type mesh_model__begin.txt > pov\000000_main.pov
type tmp >> pov\000000_main.pov
type mesh_model__end.txt >> pov\000000_main.pov
del tmp
obj2pov.exe "obj\output_000006.obj" 0_tmp 000006
type elevation_data.tmp > tmp
type water_depth_data.tmp >> tmp
type 0_tmp >> tmp
del 0_tmp
type mesh_model__begin.txt > pov\000006_main.pov
type tmp >> pov\000006_main.pov
type mesh_model__end.txt >> pov\000006_main.pov
del tmp
echo call povray2.bat 000000_main.pov > レンダリング実行.bat
echo call povray2.bat 000006_main.pov >> レンダリング実行.bat
