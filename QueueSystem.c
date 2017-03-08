#include <stdio.h>
#include "QueueSystem.h"
#include "elev.h"

int OrderTable[3][4] = {
			//          First   Second  Third   Fourth
			/* Up  */   {0       ,0      ,0      ,0},
			/* Down */  {0       ,0      ,0      ,0},
			/* Panel */ {0       ,0      ,0      ,0}
};

int qs_add_order_in_OrderTable(int button, int floor){
	OrderTable[button][floor] = 1;
	return 0;
}

int qs_get_direction(int CurrentFloor, int direction){
	
	if (CurrentFloor == 3){
		return DOWN;
	}
	else if (CurrentFloor == 0){
		return UP;
	}
	else if (direction == DOWN){
		for(int button = 0; button < 3; button++){
			for(int floor = 0; floor < CurrentFloor; floor++){
				if(OrderTable[button][floor] == 1)
				return DOWN;
			}
		}
		return UP;
	}
	else if (direction == UP){
		for(int button = 0; button < 3; button++){
			for(int floor = CurrentFloor + 1; floor < 4; floor++){
				if(OrderTable[button][floor] == 1)
				return UP;
			}
		}
		return DOWN;
	}
	else{
		return direction;
	}
}

int qs_should_elevator_stop_at_floor(int CurrentFloor, int direction){

	if (OrderTable[BUTTON_COMMAND][CurrentFloor] == 1){
		return 1;
	}		
	else if (OrderTable[direction][CurrentFloor] == 1 ){
		return 1;
	}
 	else if (CurrentFloor == 3 && OrderTable[BUTTON_CALL_DOWN][3] == 1 ){
 		return 1;
 	}
 	else if (CurrentFloor == 0 && OrderTable[BUTTON_CALL_UP][0] == 1 ){
 		return 1;
 	}
 	else if (direction == UP){
 		for(int button = 0; button < 3; button++){
	 		for(int floor = CurrentFloor + 1; floor < 4; floor++){
	 			if(OrderTable[button][floor] == 1){
	 				return 0;
	 			}
	 		}
	 	}
 		if(OrderTable[BUTTON_CALL_DOWN][CurrentFloor] == 1){
 			return 1;
 		}
 	}
 	else if (direction == DOWN){
 		for(int button = 0; button < 3; button++){
 			for(int floor = 0; floor < CurrentFloor; floor++){
 				if(OrderTable[button][floor] == 1){
 					return 0;
 				}
 			}
 		}
 		if(OrderTable[BUTTON_CALL_UP][CurrentFloor] == 1){
 			return 1;
 		}
 	}
 	return 0;
}

int qs_clear_order_at_floor_in_OrderTable(int CurrentFloor){
	for(int button = 0; button < 3; button++){
		OrderTable[button][CurrentFloor] = 0;	
	}
	return 0;
}

int qs_no_orders_in_OrderTable(void){

	for(int button = 0; button < 3; button++){
		for(int floor = 0; floor < 4; floor++){
			if(OrderTable[button][floor] == 1){
				return 0;
			}
		}
	}
	return 1;
}

void qs_clear_all_orders_in_OrderTable(void){

   	for(int button = 0; button < 3; button++){
		for(int floor = 0; floor < 4; floor++){
           	OrderTable[button][floor] = 0;
       	}
   	}	
}




