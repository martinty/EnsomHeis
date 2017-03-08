#include <stdio.h>
#include <unistd.h>
#include "elev.h"
#include "StateMachine.h"
#include "QueueSystem.h"
#include "timer.h"


typedef enum tag_StateMachine{
        Idle,
        Running, 
        DoorOpen, 
        Emergency,
        IdleBetweenFloors
} StateMachine_t;

static StateMachine_t state = Idle;
static direction_t direction = UP;

static int ArrivedFloor = -1;
static int CurrentFloor = -1;
static int LastOrderedFloor = -1;


void sm_startup(void){
	while (elev_get_floor_sensor_signal() == -1){
    	elev_set_motor_direction(DIRN_UP);
    }
    CurrentFloor = elev_get_floor_sensor_signal();
    elev_set_floor_indicator(CurrentFloor);
   	elev_set_motor_direction(DIRN_STOP);
}

void sm_stop_button_pushed(void){

	static int EmergencyOn = 0;

	switch(state){    

		case Emergency:
			if (!(elev_get_stop_signal())){
				elev_set_stop_lamp(0);
				if (elev_get_floor_sensor_signal() != -1){
					t_timer_start();
					state = DoorOpen;
				}
				else{
					state = IdleBetweenFloors;
					ArrivedFloor = -1;
				}
				EmergencyOn = 0;
			}

		default:
			if (elev_get_stop_signal()){
				if (EmergencyOn == 0){
					elev_set_motor_direction(DIRN_STOP);
					elev_set_stop_lamp(1);
					qs_clear_all_orders_in_OrderTable();
					sm_order_lights_update();
					if (elev_get_floor_sensor_signal() != -1){
						elev_set_door_open_lamp(1);
					}
					EmergencyOn = 1;
					state = Emergency;
				}
			}    
	}
}

int sm_check_order_buttons_and_add_orders_to_queue(void){

	int IsQueueTrue = 0;
	
	switch(state){

		case Emergency:
			return IsQueueTrue;
		
		default:
			for(int button = 0; button < 3; button++){
				for(int floor = 0; floor < 4; floor++){
					if(elev_get_button_signal(button, floor) == 1){
						if (OrderTable[button][floor] != 1){
							qs_add_order_in_OrderTable(button, floor);
							LastOrderedFloor = floor;
							IsQueueTrue = 1;
						}
					}
				}
			}
			return IsQueueTrue;
	}
}

void sm_order_button_is_pushed(void){

	switch(state){    
		
		case Idle:
			if (CurrentFloor != LastOrderedFloor){
				direction = qs_get_direction(CurrentFloor, direction);	
				if (direction == UP){
					elev_set_motor_direction(DIRN_UP);
				} 
				else if (direction == DOWN){
					elev_set_motor_direction(DIRN_DOWN);
				}
				state = Running;
			}	
			else {
				if (elev_get_floor_sensor_signal() == CurrentFloor){
					elev_set_door_open_lamp(1);
					t_timer_start();
					qs_clear_order_at_floor_in_OrderTable(CurrentFloor);
					state = DoorOpen;
				}
				else{
					elev_set_motor_direction(DIRN_UP);
					state = Running;
				}
			}
			break;

		case IdleBetweenFloors:
			state = Running;
			if (direction == UP){
				for(int floor = 0; floor <= CurrentFloor; floor++)
					if (floor == LastOrderedFloor){
						elev_set_motor_direction(DIRN_DOWN);
						direction = DOWN;
						CurrentFloor = CurrentFloor + 1;
						break;
					}
				if(direction == UP){
					elev_set_motor_direction(DIRN_UP);
				}
			}
			else if (direction == DOWN){
				for(int floor = CurrentFloor; floor < 4; floor++)
					if (floor == LastOrderedFloor){
						elev_set_motor_direction(DIRN_UP);
						direction = UP;
						CurrentFloor = CurrentFloor - 1;
						break;
					}
				if(direction == DOWN){
					elev_set_motor_direction(DIRN_DOWN);
				}
			}
			break;

		case DoorOpen:
			qs_clear_order_at_floor_in_OrderTable(CurrentFloor);
			if( LastOrderedFloor == CurrentFloor){
				t_timer_start();
			}
			break;

		default:
			break;
	}			
}

int sm_elevator_has_arrived_at_floor(int FloorNumber){

	switch(state){

		case Running:
			ArrivedFloor = FloorNumber;
			if (ArrivedFloor != CurrentFloor) {
				CurrentFloor = ArrivedFloor;
	//		elev_set_floor_indicator(CurrentFloor);
				if (qs_should_elevator_stop_at_floor(CurrentFloor, direction) == 1){
					elev_set_motor_direction(DIRN_STOP);
					elev_set_door_open_lamp(1);
					t_timer_start();
					qs_clear_order_at_floor_in_OrderTable(CurrentFloor);
					state = DoorOpen;
				}	
				else if (CurrentFloor == 0 || CurrentFloor == 3){
					elev_set_motor_direction(DIRN_STOP);
					if (qs_no_orders_in_OrderTable()){
						state = Idle;
					}
					else{
						if (CurrentFloor == 0){
							elev_set_motor_direction(DIRN_UP);
						}
						else{
							elev_set_motor_direction(DIRN_DOWN);
						}
						state = Running;

					}
				}
				sm_order_lights_update();
			}
			break;
		
		default:
			break;
	}
	return 0;
}

void sm_close_door(void){

	switch(state){    
		
		case DoorOpen:
			elev_set_door_open_lamp(0);
			if (qs_no_orders_in_OrderTable() == 1){
				state = Idle;
			}
			else{
				direction = qs_get_direction(CurrentFloor, direction);
				if (direction == UP){
					elev_set_motor_direction(DIRN_UP);
				}
				else if (direction == DOWN){
					elev_set_motor_direction(DIRN_DOWN);
				}
				state = Running;
			}
			break;

		default:
			break;
	}
}

void sm_order_lights_update(void){

	for(int button = 0; button < 3; button++){
		for(int floor = 0; floor < 4; floor++){
			if(OrderTable[button][floor] == 1){
				elev_set_button_lamp(button, floor, 1);
			}
			else{
				elev_set_button_lamp(button, floor, 0);
			}
		}
	}
}

void light_show(void){
	elev_set_motor_direction(DIRN_STOP);
	qs_clear_all_orders_in_OrderTable();
	sm_order_lights_update();
	while(elev_get_obstruction_signal()){
		for( int f = 0; f < 4; f++){
			elev_set_floor_indicator(f);
			for (int b = 0; b < 3; b++){
				elev_set_button_lamp(b, f, 1);
			}
			usleep(200000);
		}

		for( int f = 3; f > -1; f--){
			elev_set_floor_indicator(f);
			for (int b = 0; b < 3; b++){
				elev_set_button_lamp(b, f, 0);
			}
			usleep(200000);
		}
	}
	elev_set_floor_indicator(CurrentFloor);
}