#include <windows.h>
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Child process path is missing\n";
    return 1;
  }

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

  PROCESS_INFORMATION pi = {};
  STARTUPINFOA si = {};
  si.cb = sizeof(si);

  std::string cmd = "\"" + std::string(argv[1]) + "\" " + std::to_string(reinterpret_cast< DWORD_PTR >(rd));

  if (!CreateProcessA(argv[1], cmd.data(), NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi))
  {
    std::cerr << GetLastError() << '\n';
    CloseHandle(rd);
    CloseHandle(wr);
    return 1;
  }

  CloseHandle(rd);
  CloseHandle(wr);

  DWORD w = WaitForSingleObject(pi.hProcess, INFINITE);

  if (w == WAIT_FAILED)
  {
    std::cerr << GetLastError() << '\n';
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 1;
  }

  CloseHandle(pi.hThread);
  CloseHandle(pi.hProcess);

  return 0;
}
