set dataset=https://github.com/Sanaxen/Tsunami_simulation/releases/download/v0.1.0/tsu_dataset.7z

:bitsadmin /transfer getFile %dataset%  %~dp0tsu_dataset.7z
curl -L %dataset%  -o tsu_dataset.7z

tar -xf tsu_dataset.7z