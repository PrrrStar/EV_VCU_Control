#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include "lib.h"

int canSend(char *port, char *data)
{
	int s; /* can raw socket */ 
	int required_mtu;
	int mtu;
	int enable_canfd = 1;
	struct sockaddr_can addr;
	struct canfd_frame frame;
	struct ifreq ifr;

	/* parse CAN frame */
	required_mtu = parse_canframe(data, &frame);
	if (!required_mtu){
		printf("\nWrong CAN-frame format!\n\n");
		return 1;
	}

	/* open socket */
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("socket");
		return 1;
	}

	strncpy(ifr.ifr_name, port, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
	if (!ifr.ifr_ifindex) {
		perror("if_nametoindex");
		return 1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (required_mtu > CAN_MTU) {

		/* check if the frame fits into the CAN netdevice */
		if (ioctl(s, SIOCGIFMTU, &ifr) < 0) {
			perror("SIOCGIFMTU");
			return 1;
		}
		mtu = ifr.ifr_mtu;

		if (mtu != CANFD_MTU) {
			printf("CAN interface is not CAN FD capable\n");
			return 1;
		}

		/* interface is ok - try to switch the socket into CAN FD mode */
		if (setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES,
			       &enable_canfd, sizeof(enable_canfd))){
			printf("error when enabling CAN FD support\n");
			return 1;
		}

		/* ensure discrete CAN FD length values 0..8, 12, 16, 20, 24, 32, 64 */
		frame.len = can_dlc2len(can_len2dlc(frame.len));
	}

	/* disable default receive filter on this RAW socket */
	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		return 1;
	}

	/* send frame */
	if (write(s, &frame, required_mtu) != required_mtu) {
		perror("write");
		return 1;
	}

	close(s);

	return 0;
}

int open_port(const char *port)
{
    struct ifreq ifr;
    struct sockaddr_can addr;
    soc = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(soc < 0) return -1;
    addr.can_family = AF_CAN;
    strcpy (ifr.ifr_name, port);
    if(ioctl(soc, SIOCGIFINDEX, &ifr) < 0)
        return -1;
    addr.can_ifindex = ifr.ifr_ifindex;
    fcntl(soc, F_SETFL, O_NONBLOCK);
    if(bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        return -1;
    return 0;
}

static int read_can_port = 1;
char buffer_disp[50] = "";


int read_port()
{
    struct can_frame frame_rd;
    int recvbytes = 0;
    int i;
    get_canid = 0;
    get_candlc = 0;
    if(read_can_port == 0) return -1;
    struct timeval timeout = {1, 0};
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(soc, &readSet);
    if(select((soc+1), &readSet, NULL, NULL, &timeout) >=0)
    {
        if(FD_ISSET(soc, &readSet))
        {
            recvbytes = read(soc, &frame_rd,  sizeof(struct can_frame));
            if(recvbytes)
            {
                get_canid = frame_rd.can_id;
                get_candlc = frame_rd.can_dlc;
                memset(get_candata, 0, 20);
                for(i = 0; i < get_candlc; i++)
                    get_candata[i] = frame_rd.data[i];
            }
        }
     }
  }
  return 0;
}

int close_port()
{
    close(soc);
    return 0;
}
