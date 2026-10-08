#include <windows.h>
#include <iostream>
#include <string>
#include <cstddef>
#include <algorithm>

DWORD send(DWORD& err, HANDLE wr, const char* b, DWORD k)
{
  DWORD r = 0;
  DWORD h = 0;

  while (r < k)
  {
    if (!WriteFile(wr, b + r, k - r, &h, NULL))
    {
      err = GetLastError();
      break;
    }

    if (h == 0)
    {
      err = ERROR_WRITE_FAULT;
      break;
    }
    r += h;
  }
  return r;
}

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

  DWORD err = 0;
  size_t pos = 0;

  while (pos < msg.size())
  {
    DWORD k = static_cast< DWORD >(std::min< size_t >(4096, msg.size() - pos));

    if (send(err, wr, msg.data() + pos, k) != k)
    {
      std::cerr << err << '\n';
      CloseHandle(wr);
      WaitForSingleObject(pi.hProcess, INFINITE);
      CloseHandle(pi.hThread);
      CloseHandle(pi.hProcess);
      return 1;
    }
    pos += k;
  }

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
