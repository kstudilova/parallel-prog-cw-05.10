#include <windows.h>
#include <iostream>
#include <string>

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
  return 0;
}
