#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <error.h>

typedef struct {
	unsigned long start;
	unsigned long end;
	char startip[16];
	char endip[16];
	char country[20];
	char province[20];
	char detail[20];
} iploc;

int ipcmp(const void* a, const void* b)
{
	iploc* va = (iploc*)a;
	iploc* vb = (iploc*) b;
	if (va->start > vb->end)
		return 1;
	else if (va->end < vb->start)
		return -1;
	else
		return 0;
}

#define IPCNT 11000
#define SHMKEY 0x00119527
#define SHMFILENAME "/dev/shm/ip2loc"
