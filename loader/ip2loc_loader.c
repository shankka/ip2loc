#include "ip2loc.h"

int main(int argc, char* argv[])
{
	iploc loc[IPCNT];
	char* loadsrc;
	FILE* fp;
	key_t key;
	int p=0;
	int shm_id;
	iploc* shm;
	memset(loc, 0, sizeof(iploc)*IPCNT);

	if (argc != 2)
	{
		printf("Usage:%s src\n", argv[0]);
		return 1;
	}

	loadsrc = argv[1];
	printf("Load from %s\n", loadsrc);

	fp = fopen(loadsrc, "r");
	if (!fp)
	{
		printf("Open file[%s] failed!\n", loadsrc);
		return 2;
	}

	while (!feof(fp) && p<IPCNT)
	{
		unsigned long start=0;
		unsigned long end=0;
		char startip[16]={0};
		char endip[16]={0};
		char country[20]={0};
		char province[20]={0};
		char detail[20]={0};
		int ret;
		char row[1024+1]={0};
		fgets(row, 1024, fp);
		ret = sscanf(row, "%lu\t%lu\t%s\t%s\t%s\t%s\t\t%s\n", &start, &end,
				startip, endip, country, province, detail);

		if (ret == -1 || start == 0 || end == 0)
			continue;

		loc[p].start = start;
		loc[p].end = end;
		memcpy(loc[p].startip, startip, sizeof(loc[p].startip));
		memcpy(loc[p].endip, endip, sizeof(loc[p].endip));
		memcpy(loc[p].country, country, sizeof(loc[p].country));
		memcpy(loc[p].province, province, sizeof(loc[p].province));
		memcpy(loc[p].detail, detail, sizeof(loc[p].detail));
		p++;
	}

	qsort(loc, IPCNT, sizeof(iploc), ipcmp);

	printf("Loaded rows num : %d\n", p);

	//shm_id = shmget(SHMKEY, sizeof(iploc)*IPCNT, IPC_CREAT|0666);
	key = ftok(SHMFILENAME, 0);
	if (key == -1)
	{
		perror("Ftok error");
		return 2;
	}

	shm_id = shmget(key, sizeof(iploc)*IPCNT, IPC_CREAT|0666);
	if (shm_id == -1)
	{
		perror("Create shared memory failed!");
		return 3;
	}

	shm = (iploc*)shmat(shm_id, NULL, 0);
	if (shm == (char*)-1)
	{
		perror("Failed!");
		return 4;
	}
	memcpy(shm, loc, sizeof(iploc)*IPCNT);
	shmdt(shm);
	return 0;
}
