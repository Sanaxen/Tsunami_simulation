'---------------------------------------------------------------------------
' フォントインストールスクリプト
' 　このスクリプトと同じ場所にあるttfファイルを
' 　fontsフォルダにインストールする
'---------------------------------------------------------------------------
Option Explicit
dim FSO, srcFolder, dstFolder, File, SH, strFontsPath

'Fontsフォルダのパス取得
strFontsPath = CreateObject("WScript.Shell").SpecialFolders("Fonts")
set FSO = CreateObject("Scripting.FileSystemObject")
set srcFolder = FSO.GetFile(WScript.scriptfullname).ParentFolder
set SH = CreateObject("Shell.Application")
set dstFolder = SH.Namespace(strFontsPath)

dstFolder.CopyHere "7barSPBd.TTF"
WriteLine "7barSPBd.TTF" 




