#include <Windows.h>

BOOL IsFileAlreadyOpen(char *filename)
   {
      HFILE theFile = HFILE_ERROR;
      DWORD lastErr  = NO_ERROR;

      // �t�@�C����r���I�[�v�����܂��B
      theFile = _lopen(filename, OF_READ | OF_SHARE_EXCLUSIVE);

      if (theFile == HFILE_ERROR)
         // ���̃G���[��ۑ����܂��B
         lastErr = GetLastError();
      else
         // �����I�[�v�������ꍇ�́A�t�@�C�����N���[�Y���܂��B
         _lclose(theFile);

      // ���L�ᔽ������ꍇ�� TRUE ��Ԃ��܂��B
      return ((theFile == HFILE_ERROR) &&
              (lastErr == ERROR_SHARING_VIOLATION));
   }
