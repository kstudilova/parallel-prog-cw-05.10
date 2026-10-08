#include <windows.h>
#include <iostream>
#include <string>

int main()
{
  std::string msg;
  std::getline(std::cin, msg);

  HANDLE rd, wr;

  SECURITY_ATTRIBUTES sa = {};
  sa.nLength = sizeof(sa);
  sa.bInheritHandle = FALSE;

  if (!CreatePipe(&rd, &wr, &sa, 256))
  {
    std::cerr << GetLastError() << '\n';
    return 1;
  }

  if (!SetHandleInformation(rd, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT))
  {
    std::cerr << GetLastError() << '\n';
    CloseHandle(rd);
    CloseHandle(wr);
    return 1;
  }

  CloseHandle(rd);
  CloseHandle(wr);
}
