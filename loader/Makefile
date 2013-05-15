all: ip2loc_loader ip2loc

ip2loc_loader: ip2loc_loader.c
        gcc -Wall -o ip2loc_loader ip2loc_loader.c

ip2loc: ip2loc.c
        gcc -Wall -o ip2loc ip2loc.c

clean: 
        rm ip2loc_loader ip2loc