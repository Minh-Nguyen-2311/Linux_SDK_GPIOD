#include "stdio.h"
#include "math.h"
#include "gpiod.h"

int servo_driver_init(const char *chipname, unsigned int line_num);
void servo_driver_set_angle(float angle);
void servo_driver_set_direction(float speed);
void servo_driver_stop(void);
