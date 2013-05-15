#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ip2loc.h"

unsigned long ip2long(char*);

int main(int argc, char* argv[])
{
	char* ip;
	unsigned long addr;
	iploc * search;
	key_t key;
	iploc loc = {0, 0, {0}, {0}, {0}, {0}, {0}};
	int shm_id;
	iploc* shm;

	if (argc != 2)
	{
		printf("Usage:%s ip\n", argv[0]);
		return 0;
	}

	ip = argv[1];
	addr = ip2long(ip);
	printf("%s: %lu\n",ip, addr);
	loc.start = addr;
	loc.end = addr;

	//shm_id = shmget(SHMKEY, sizeof(iploc)*IPCNT, IPC_CREAT);
	key = ftok(SHMFILENAME, 0);
	if (key == -1)
	{
		perror("Ftok error");
		return 2;
	}

	shm_id = shmget(key, sizeof(iploc)*IPCNT, IPC_CREAT);
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

	search = (iploc*)bsearch(&loc, shm, IPCNT, sizeof(iploc), ipcmp);
	if (search == NULL)
	{
		printf("Not found!\n");
	}
	else
	{
		printf("%s, %s, %s", search->country, search->province, search->detail);
	}
	return 0;
}

unsigned long ip2long(char* ip)
{
	struct in_addr addr;
	int ret = inet_aton(ip, &addr);
	if (ret == 0)
		return 0;

	return (unsigned long) ntohl(addr.s_addr);
}
