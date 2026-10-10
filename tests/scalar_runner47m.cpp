#include "scalar_suite47m.h"
#include <stdio.h>
int main(){auto r=run_scalar_suite47m();printf("Scalar47M checks=%u failures=%u first_line=%u digest=%08x\n",r.checks,r.failures,r.first_failure_line,r.digest);return r.failures?1:0;}
