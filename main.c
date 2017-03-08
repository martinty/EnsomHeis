#include <stdio.h>
#include <unistd.h>
#include "elev.h"
#include "StateMachine.h"
#include "timer.h"

int main() {

    // Initialize hardware
    if (!elev_init()) {
        printf("Unable to initialize elevator hardware!\n");
        return 1;
    }

    sm_startup();

    static double Seconds  =  3;
    static int FloorNumber = -1;

    while (1) {

		sm_stop_button_pushed();
		
		if (sm_check_order_buttons_and_add_orders_to_queue()){
			sm_order_button_is_pushed();
			sm_order_lights_update();
		}

		FloorNumber = elev_get_floor_sensor_signal();
		if (FloorNumber != -1){
			elev_set_floor_indicator(FloorNumber);
			sm_elevator_has_arrived_at_floor(FloorNumber);
		}
			
		if (t_check_if_timer_is_finished(Seconds)){
			sm_close_door();
		}

		if(FloorNumber == -1){
			sm_close_door();
		}

		if(elev_get_obstruction_signal()){
			light_show();
		}

		usleep(128);
	}
	return 0;
}

    