GLUI��Visual Studio 2010�Ŏg���@���̂Q
�R���s���[�^

���(GLUI��VisualStudio 2010�Ŏg��)�Ƃ͕ʂ�PC��GLUI�����Ă݂��B

Windows Vista 32bit VisualStudio 2010�ł���B

��������glui-2.36.zip���_�E�����[�h�����B

������𓀂���glui-2.36\src\msvc\glui.sln��VisualStudio2010�ŊJ���B

�ϊ��E�B�U�[�h�Ŏ����I�ɕϊ����J�n�B

�ϊ���A�v���W�F�N�g_glui library���r���h����Ɖ��̂悤�ȃG���[���ł��B


1>c:\users\xxx\desktop\glui-2.36\src\include\gl\glui.h(1718): error C2252: �e���v���[�g�̖����I�ȃC���X�^���X���͖��O��ԃX�R�[�v�ł̂ݔ������܂�
1>c:\users\xxx\desktop\glui-2.36\src\include\gl\glui.h(1719): error C2252: �e���v���[�g�̖����I�ȃC���X�^���X���͖��O��ԃX�R�[�v�ł̂ݔ������܂�


����ɂ��Ă͐�l�����łɉ������Ă���B

Installing GLUI using VC++ on windows

�܂��A�ȉ��̃G���[�ӏ���GLUIAPI GLUI_CommandLine�̊O�փJ�b�g�A���h�y�[�X�g�ňړ�����B


    #ifdef _MSC_VER
    // Explicit template instantiation needed for dll
    template class GLUIAPI std::allocator<GLUI_String>;
    template class GLUIAPI std::vector<GLUI_String, std::allocator<GLUI_String> >;
    #endif


�����āA#include <iterator> �� #include <cstdlib>�@�̉��ɒǉ�����B

�Ăуr���h���Ă݂�ƃr���h�͐�����glui32.lib���������ꂽ�B

