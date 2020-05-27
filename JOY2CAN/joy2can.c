#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#include <unistd.h>
#include <linux/joystick.h>


int read_event(int fd, struct js_event *event){
    ssize_t bytes;

    bytes = read(fd, event, sizeof(*event));

    if (bytes == sizeof(*event))
        return 0;

    return -1;
}

size_t get_axis_count(int fd){

    __u8 axes;

    if (ioctl(fd, JSIOCGAXES, &axes) == -1)
        return 0;

    return axes;
}


size_t get_button_count(int fd){

    __u8 buttons;
    if (ioctl(fd, JSIOCGBUTTONS, &buttons) == -1)
        return 0;

    return buttons;
}

struct axis_state {
    short x, y;
};


size_t get_axis_state(struct js_event *event, struct axis_state axes[3]){
    size_t axis = event->number / 2;

    if (axis < 3){
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
	
	const char *can_id = "200";
	char *data = malloc(sizeof(char)*20);
	
	
    	const char *manual = "80.00.5A.00.00.00.00.00";

	int forward = 0x88;
	int backward = 0x84;
	
	
    	int direction = 0x5a;


    	const char *leftLamp = "82";
    	const char *rightLamp = "81";
    
    	const char *brake = "84";
    	const char *parking = "80";

    		
    	int js;
	struct js_event event;
	struct axis_state axes[3] = {0};
	size_t axis;

	bool bw_switch = false;


	js = open(device, O_RDONLY);

	if (js == -1)
		perror("Could not open joystick");

	while (read_event(js, &event) == 0) {
	            
        	switch (event.type) {

			case JS_EVENT_BUTTON:
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

				printf("\n");
			
			break;


			case JS_EVENT_AXIS:
				axis = get_axis_state(&event, axes);
				if (axis < 3){
		     			if (axis==0&&axes[axis].x == 0&&axes[axis].y == -32767){
						printf("↑ : Forward\n");
			        		printf(forward);			
							
			        		printf("\n");
			        		canSend(can_device,data);
					
					}
					if (axis==0&&axes[axis].x == 0&&axes[axis].y == 32767){
						printf("↓ : backward\n");			
						printf(backward);
							
			        		printf("\n");
			        		canSend(can_device,data);
					
					}
					if (axis==0&&axes[axis].x == -32767&&axes[axis].y == -2){
						printf("← : Steering Left\n");
		        			direction -= 0x01;
		        			sprintf(data,"%s#%x.00.%x.00.00.00.00.00",can_id,forward,direction);
		        			
						printf("%s\n",data);
		        			canSend(can_device,data);

					}
					if (axis==0&&axes[axis].x == 32767&&axes[axis].y == -2){
						printf("← : Steering Left\n");
		        			direction += 0x01;
		        			sprintf(data,"%s#%x.00.%x.00.00.00.00.00",can_id,forward,direction);
		        			
						printf("%s\n",data);
		        			canSend(can_device,data);
		        		}
		
				}
        		break;


			default:
		}
		
		
        
	fflush(stdout);
	}
	

	close(js);
	return 0;
}
