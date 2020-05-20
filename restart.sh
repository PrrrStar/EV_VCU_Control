ifconfig can0 down
ifconfig can1 down

modprobe can
modprobe can_raw
modprobe mttcan
ip link set can0 type can bitrate 500000 dbitrate 2000000 berr-reporting on fd on

ip link set can1 type can bitrate 500000 dbitrate 2000000 berr-reporting on fd on

ip link set up can0
ip link set up can1

