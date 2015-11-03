#include "api_robot.h" /* Robot control API */
#include <stdlib.h>

/* main function */
void _start(void){
    srand(time(0));

    while(1){
        if(( read_sonar(4) > 1200 ) || ( read_sonar(3) > 1200 )){
            int motor1 = rand() % 256;
            int motor2 = rand() % 256;
            set_speed_motos(motor1,motor2);
        }
    }

}

