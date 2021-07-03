GLUIをVisual Studio 2010で使う　その２
コンピュータ

昨日(GLUIをVisualStudio 2010で使う)とは別のPCにGLUIを入れてみた。

Windows Vista 32bit VisualStudio 2010である。

ここからglui-2.36.zipをダウンロードした。

これを解凍してglui-2.36\src\msvc\glui.slnをVisualStudio2010で開く。

変換ウィザードで自動的に変換が開始。

変換後、プロジェクト_glui libraryをビルドすると下のようなエラーがでた。


1>c:\users\xxx\desktop\glui-2.36\src\include\gl\glui.h(1718): error C2252: テンプレートの明示的なインスタンス化は名前空間スコープでのみ発生します
1>c:\users\xxx\desktop\glui-2.36\src\include\gl\glui.h(1719): error C2252: テンプレートの明示的なインスタンス化は名前空間スコープでのみ発生します


これについては先人がすでに解決している。

Installing GLUI using VC++ on windows

まず、以下のエラー箇所をGLUIAPI GLUI_CommandLineの外へカットアンドペーストで移動する。


    #ifdef _MSC_VER
    // Explicit template instantiation needed for dll
    template class GLUIAPI std::allocator<GLUI_String>;
    template class GLUIAPI std::vector<GLUI_String, std::allocator<GLUI_String> >;
    #endif


そして、#include <iterator> を #include <cstdlib>　の下に追加する。

再びビルドしてみるとビルドは成功しglui32.libが生成された。

