#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#define NAME	"EV_VCU"			

void usage(void)
{
	fprintf(stderr, "usage: %s <candev>\n", NAME);

	exit(-1);
}

/*
 * Main
 */

int main(int argc, char *argv[])
{
	int s;
	int n;
	int n1;
	int n2;
	char *ifname;
	struct sockaddr_can addr;
	struct ifreq ifr;
	socklen_t len = sizeof(addr);
	struct can_frame frame;
	int ret;

	/* Check command line */
	if (argc < 1)
		usage();
	ifname = argv[1];
	
	/* Open the PF_CAN socket */
	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (s < 0) {
		perror("Error while opening socket");
		exit(-1);
	}

	/* Find the CAN device */
	strcpy(ifr.ifr_name, ifname);
	ret = ioctl(s, SIOCGIFINDEX, &ifr);
        if (ret < 0) {
		perror("ioctl");
		exit(-1);
	}
	printf("%s: %s at index %d\n", NAME, ifname, ifr.ifr_ifindex);
	
	/* Bind the socket */
	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0) {
		perror("bind");
		exit(-1);
	}


	/* Fill the frame data */
	frame.can_id  = 0x200;
	frame.can_dlc = 8;
	frame.data[0] = 0x80;
	frame.data[1] = 0x00;
	frame.data[2] = 0x5A;
	frame.data[3] = 0x00;
	frame.data[4] = 0x00;
	frame.data[5] = 0x00;
	frame.data[6] = 0x00;
	frame.data[7] = 0x00;
	/* Send the frame */
	
	n = write(s, &frame, sizeof(struct can_frame));
	if (n < 0) {
		perror("write");
		exit(-1);
	}

	printf("ID\t: %x\n",frame.can_id);
	printf("DLC\t: %d\n",frame.can_dlc);
	printf("Data\t: ");
	
	int j;
	for(j = 0; j<sizeof(frame.data); j++){

		printf("%x ",frame.data[j]);
	}
	printf("\n");
	printf("\n%s: wrote %d bytes\n", NAME, n);

	n2 = sendto(s, &frame, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, sizeof(addr));
	
	if (n2 < 0) {
		perror("sendto");
		exit(-1);
	}
	
	printf("ID\t: %x\n",frame.can_id);
	printf("DLC\t: %d\n",frame.can_dlc);
	printf("Data\t: ");
	int k;
	for(k = 0; k<sizeof(frame.data); k++){

		printf("%x ",frame.data[k]);
	}
	printf("\n");
	printf("\n%s: sendto %d bytes\n", NAME, n2);
	
	n1 = recvfrom(s, &frame, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, &len);

	if (n1 < 0) {
		perror("recvfrom");
		exit(-1);
	}
	printf("\n%s: recvfrom %d bytes\n", NAME, n1);

	printf("ID\t: %x\n",frame.can_id);
	printf("DLC\t: %d\n",frame.can_dlc);
	printf("Data\t: ");
	int i;
	for(i = 0; i<sizeof(frame.data); i++){

		printf("%x ",frame.data[i]);
	}
	printf("\n%s: wrote %d bytes\n", NAME, n);
	printf("\n\n%s: wrote %d bytes", NAME, n);
	printf("\n%s: recvfrom %d bytes", NAME, n1);
	printf("\n%s: sendto %d bytes\n", NAME, n2);
	
	return 0;
}

