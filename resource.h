#ifndef RESOURCE_H
#define RESOURCE_H

#include <stdint.h>

// Returns CPU brand string (48 chars + null)
void get_cpu_brand(char* out);

const CHAR8* DetectVirtualizerType(void);

#endif

