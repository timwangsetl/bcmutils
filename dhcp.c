#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "bcmutils.h"

int bcm_dhcpsnoop_trustport_set(int skfd, int trustports)
{
    uint32 data[8];

    memset(data, '\0', sizeof(data));
    data[0] = trustports;


    if (ioctl(skfd, DHCP_SNOOP_TRUST, &data) < 0) {
        printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
        return -1;    
    }
    return 0;
}

int bcm_dhcpv6_trustport_set(int skfd, int trustports)
{
	uint32 data[8];

	memset(data, '\0', sizeof(data));
    data[0] = trustports;


	if (ioctl(skfd, DHCPv6_SNOOP_TRUST, &data) < 0) {
		printf("[%s:%d]: %s\n", __FUNCTION__, __LINE__, strerror(errno));
		return -1;    
	}
	return 0;
}

