:copy ..\bin\*.exe Release /v /y
del /Q 処理結果ステータス.log
del /Q .\google_earth\*.*

call init.bat
call clear.bat
del /Q chkpoint_data.bmp
call 安政東海地震v2.bat
call autoTestSub.bat 安政東海地震v2 遡上考慮あり
:call autoTestSub.bat 安政東海地震v2 遡上考慮無し

call init.bat
call clear.bat
del /Q chkpoint_data.bmp
call 安政東海地震v2.bat
call 伊勢湾台風.bat
call autoTestSub.bat 伊勢湾台風 遡上考慮あり
:call autoTestSub.bat 伊勢湾台風 遡上考慮無し

call init.bat
call clear.bat
del /Q chkpoint_data.bmp
call "台風201330号(HAIYAN).bat"
call autoTestSub.bat 台風201330号(HAIYAN) 遡上考慮あり 1
:call autoTestSub.bat 台風201330号(HAIYAN) 遡上考慮無し


call init.bat
call clear.bat
del /Q chkpoint_data.bmp
call 東北太平洋沖v2-2.bat
call 東北大モデルVer1.2.bat
call autoTestSub.bat 東北太平洋沖v2-2_東北大モデルVer1.2 遡上考慮あり
:call autoTestSub.bat 東北太平洋沖v2-2_東北大モデルVer1.2 遡上考慮無し


call init.bat
call clear.bat
del /Q chkpoint_data.bmp
call 宝永地震v2.bat
call autoTestSub.bat 宝永地震v2 遡上考慮あり
:call autoTestSub.bat 宝永地震v2 遡上考慮無し


:A
call init.bat
call clear.bat
del /Q chkpoint_data.bmp
call 東北太平洋沖v2-2.bat
call autoTestSub.bat 東北太平洋沖v2-2 遡上考慮あり
:call autoTestSub.bat 東北太平洋沖v2-2 遡上考慮無し


call init.bat
call clear.bat
del /Q chkpoint_data.bmp
call 東北太平洋沖v2-2.bat
call 佐竹モデルVer4.6.bat
call autoTestSub.bat 東北太平洋沖v2-2_佐竹モデルVer4.6 遡上考慮あり
:call autoTestSub.bat 東北太平洋沖v2-2_佐竹モデルVer4.6 遡上考慮無し


call init.bat
call clear.bat
del /Q chkpoint_data.bmp
call 宝永地震v2.bat
call Furumura_2011.bat
call autoTestSub.bat Furumura_2011 遡上考慮あり
:call autoTestSub.bat Furumura_2011 遡上考慮無し







:end


