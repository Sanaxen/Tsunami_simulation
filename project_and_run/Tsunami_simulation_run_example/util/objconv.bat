dir obj /B | sed -e "s/^/call OBJConvFormat.bat /" | sed -e "s/\.obj//g" > x.bat
