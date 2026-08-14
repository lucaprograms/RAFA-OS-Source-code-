#ifndef RESOURCE_H
#define RESOURCE_H

#include <stdint.h>

// Returns CPU brand string (48 chars + null)
void get_cpu_brand(char* out);

// Resource monitor application (graphics mode)
int monitorapp(uint32_t* fb, uint32_t pitch);

#endif

