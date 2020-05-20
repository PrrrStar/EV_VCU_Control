/**
 * Author: Jason White
 *
 * Description:
 * Reads joystick/gamepad events and displays them.
 *
 * Compile:
 * gcc joystick.c -o joystick
 *
 * Run:
 * ./joystick [/dev/input/jsX]
 *
 * See also:
 * https://www.kernel.org/doc/Documentation/input/joystick-api.txt
 */
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <stdbool.h>


/**
 * Reads a joystick event from the joystick device.
 *
 * Returns 0 on success. Otherwise -1 is returned.
 */
int read_event(int fd, struct js_event *event)
{
    ssize_t bytes;

    bytes = read(fd, event, sizeof(*event));

    if (bytes == sizeof(*event))
        return 0;

    /* Error, could not read full event. */
    return -1;
}

/**
 * Returns the number of axes on the controller or 0 if an error occurs.
 */
size_t get_axis_count(int fd)
{
    __u8 axes;

    if (ioctl(fd, JSIOCGAXES, &axes) == -1)
        return 0;

    return axes;
}

/**
 * Returns the number of buttons on the controller or 0 if an error occurs.
 */
size_t get_button_count(int fd)
{
    __u8 buttons;
    if (ioctl(fd, JSIOCGBUTTONS, &buttons) == -1)
        return 0;

    return buttons;
}

/**
 * Current state of an axis.
 */
struct axis_state {
    short x, y;
};

/**
 * Keeps track of the current axis state.
 *
 * NOTE: This function assumes that axes are numbered starting from 0, and that
 * the X axis is an even number, and the Y axis is an odd number. However, this
 * is usually a safe assumption.
 *
 * Returns the axis that the event indicated.
 */
size_t get_axis_state(struct js_event *event, struct axis_state axes[3])
{
    size_t axis = event->number / 2;

    if (axis < 3)
    {
        if (event->number % 2 == 0)
            axes[axis].x = event->value;
        else
            axes[axis].y = event->value;
    }

    return axis;
}


int canSend(char *port, char *data);

int main(int argc, char *argv[]) {


	const char *can_device = "vcan0";
    const char *device = "/dev/input/js0";

    const char *forward = "200#88.00.5A.00.00.00.00.00";
    const char *backward = "200#84.00.5A.00.00.00.00.00";

    const char *leftLamp = "200#82.00.5A.00.00.00.00.00";
    const char *rightLamp = "200#81.00.5A.00.00.00.00.00";
    
    const char *brake = "200#84.00.5A.00.00.00.00.00";
    const char *parking = "200#80.00.5A.00.00.00.00.00";

    const char *d_left = "200#80.00.40.00.00.00.00.00";
    const char *d_right = "200#80.00.74.00.00.00.00";

    const char *manual = "200#80.00.5A.00.00.00.00.00";
    
    int js;
	struct js_event event;
	struct axis_state axes[3] = {0};
	size_t axis;

	bool bw_switch = false;

	//default (Forward) : 90 degree
	int angle = 90;

	js = open(device, O_RDONLY);

	if (js == -1)
		perror("Could not open joystick");

	while (read_event(js, &event) == 0) {
	        switch (event.type) {

			case JS_EVENT_BUTTON:
            //    printf("Button %u %s\n", event.number, event.value ? "pressed" : "released");


				if (event.number == 0){
					printf("Green : A Button");
				}

				if (event.number == 1){
					printf("Brake\n");
				    printf(brake);

                    printf("\n");
                    canSend(can_device,brake);

				}

				if (event.number == 2){					
                    printf("Parking Mode\n");
				    printf(parking);

                    printf("\n");
                    canSend(can_device,parking);

				}

				if (event.number == 3){
					printf("Yellow : Y Button");
				}

				if (event.number == 4){
					printf("Left Lamp\n");
                    printf(leftLamp);
                    
                    printf("\n");
				    canSend(can_device,leftLamp);

                }

				if (event.number == 5){
					printf("Rigth Lamp\n");
				    printf(rightLamp);

                    printf("\n");
                    canSend(can_device,rightLamp);
				}

				if (event.number == 6){
					printf("Foward");
				
                				
				}

				if (event.number == 7){
					printf("Remote ON",event.value);
                    
	
				}
				printf("\n");
			
			break;


			case JS_EVENT_AXIS:
				axis = get_axis_state(&event, axes);
				if (axis < 3){
                	//printf("Axis %zu at (%6d, %6d)\n", axis, axes[axis].x, axes[axis].y);
					if (axis==0&&axes[axis].x == 0&&axes[axis].y == -32767){
						printf("↑ : Forward\n");
                        printf(forward);			
						
                        printf("\n");
                        canSend(can_device,forward);
				
					}
					if (axis==0&&axes[axis].x == 0&&axes[axis].y == 32767){
						printf("↓ : backward\n");			
						printf(backward);
						
                        printf("\n");
                        canSend(can_device,backward);
				
					}
					if (axis==0&&axes[axis].x == -32767&&axes[axis].y == -2){
						printf("← : Steering Left\n");
                        printf(d_left);
                        
					    printf("\n");
                        canSend(can_device,d_left);

        			}
					if (axis==0&&axes[axis].x == 32767&&axes[axis].y == -2){
						printf("→ : Steering Right\n");
                        printf(d_right);			
						
                        printf("\n");
                        canSend(can_device,d_right);

					}
		
				}
                	break;

			default:
                /* Ignore init events. */
		break;
		}
        
	fflush(stdout);
	}

	close(js);
	return 0;
}
