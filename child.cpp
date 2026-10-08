#include <windows.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <exception>

DWORD recv(DWORD& err, HANDLE rd, char* b, DWORD k)
{
  DWORD r = 0;
  DWORD h = 0;

  while (r < k)
  {
    if (!ReadFile(rd, b + r, k - r, &h, NULL))
    {
      err = GetLastError();
      break;
    }

    if (h == 0)
    {
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
    std::cerr << "Read handle is missing\n";
    return 1;
  }

  HANDLE rd;
  try
  {
    rd = reinterpret_cast< HANDLE >(std::stoull(argv[1]));
  }
  catch (const std::exception&)
  {
    std::cerr << "Invalid read handle\n";
    return 1;
  }

  char msg[4096];
  DWORD err = 0;
  DWORD k = sizeof(msg) - 1;

  while (true)
  {
    DWORD n = recv(err, rd, msg, k);

    if (n > 0)
    {
      msg[n] = '\0';
      printf("%s", msg);
    }

    if (err == ERROR_BROKEN_PIPE)
    {
      break;
    }

    if (err != 0)
    {
      std::cerr << err << '\n';
      CloseHandle(rd);
      return 1;
    }

    if (n == 0)
    {
      break;
    }
  }

  std::cout << '\n';
  CloseHandle(rd);
  return 0;
}
