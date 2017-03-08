

int OrderTable[3][4];


typedef enum tag_direction {
		UP   = 0,
		DOWN = 1
} direction_t ;


int qs_add_order_in_OrderTable(int button, int floor);


int qs_get_direction(int CurrentFloor, int direction);


int qs_should_elevator_stop_at_floor(int CurrentFloor, int direction);


int qs_clear_order_at_floor_in_OrderTable(int CurrentFloor);


int qs_no_orders_in_OrderTable(void);


void qs_clear_all_orders_in_OrderTable(void);