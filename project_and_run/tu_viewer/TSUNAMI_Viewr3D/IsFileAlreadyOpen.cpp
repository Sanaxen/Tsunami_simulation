#include <Windows.h>

BOOL IsFileAlreadyOpen(char *filename)
   {
      HFILE theFile = HFILE_ERROR;
      DWORD lastErr  = NO_ERROR;

      // ファイルを排他オープンします。
      theFile = _lopen(filename, OF_READ | OF_SHARE_EXCLUSIVE);

      if (theFile == HFILE_ERROR)
         // 今のエラーを保存します。
         lastErr = GetLastError();
      else
         // 無事オープンした場合は、ファイルをクローズします。
         _lclose(theFile);

      // 共有違反がある場合は TRUE を返します。
      return ((theFile == HFILE_ERROR) &&
              (lastErr == ERROR_SHARING_VIOLATION));
   }
