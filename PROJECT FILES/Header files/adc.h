#include "types.h"

void InitADC(void);
void ReadADC(u32 chno,f32* eAR,u32 *AdcDval);
void DisplayADC(float *eAR);
