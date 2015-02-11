#include <convolve.h>

void convolve(int32_t Signal[/* SignalLen */], int SignalLen,
              int32_t Kernel[/* KernelLen */], int KernelLen,
              int32_t Result[/* SignalLen + KernelLen - 1 */])
{
  int n;

  for (n = 0; n < SignalLen + KernelLen - 1; n++)
  {
    int kmin, kmax, k;

    Result[n] = 0;

    kmin = (n >= KernelLen - 1) ? n - (KernelLen - 1) : 0;
    kmax = (n < SignalLen - 1) ? n : SignalLen - 1;

    for (k = kmin; k <= kmax; k++)
    {
      Result[n] += Signal[k] * Kernel[n - k];
    }
  }
}
