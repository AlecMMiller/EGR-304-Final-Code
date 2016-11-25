#include <project.h>

#define  CHALLENGE_TIME 10

enum states{
    ACTIVE,
    OFF,
    VICTORY
};

int count = CHALLENGE_TIME;
int battery_state = 3;
int system_state = ACTIVE;
int dumb_placeholder;

void batteryLight(uint8_t setValue){
    if(setValue & 0b100)
        LED_1_Write(1);
    else
        LED_1_Write(0);
        
    if(setValue & 0b10)
        LED_2_Write(1);
    else
        LED_2_Write(0);
        
    if(setValue & 0b1)
        LED_3_Write(1);
    else
        LED_3_Write(0);
}

CY_ISR(TIMER_ISR){
    count--;
    if(count == 0){
     count = CHALLENGE_TIME;
     battery_state--;
     switch(battery_state){
        case 2:
            batteryLight(0b011);
            break;
            
        case 1:
            batteryLight(0b001);
            break;
            
        case 0:
            batteryLight(0b000);
            system_state = OFF;
            break;
            
        case -1:
            batteryLight(0b111);
            battery_state = 3;
            system_state = ACTIVE;
       }
    }
}

typedef struct Servo { // Servo object
    void (*pwmPtr)(uint16_t);
    uint16_t max;
    uint16_t min;
    double range;
};

struct Servo create_servo(void (pwm_ptr)(uint16_t), uint16_t min, uint16_t max){ // Initializes the servo object
    struct Servo createdServo;
    createdServo.max = max;
    createdServo.min = min;
    createdServo.range = max - min;
    createdServo.pwmPtr = pwm_ptr;
    return createdServo;
}

void set_servo(struct Servo servo, uint8_t angle){ // Set servo to the angle specified
    double setpoint = angle * servo.range / 180 + servo.min;
    servo.pwmPtr(setpoint);
}

int main()
{
    Countdown_Start();
    timer_interrupt_StartEx(TIMER_ISR);
    CyGlobalIntEnable; /* Enable global interrupts. */
    PWM_1_Enable();
    struct Servo servoBottom =  create_servo(PWM_1_WriteCompare1,1400,6750);
    struct Servo servoTop =  create_servo(PWM_1_WriteCompare2,6900,1650);
    batteryLight(0b111);
    for(;;)
    {
        switch(system_state){
            case ACTIVE:
                Servo_En_2_Write(0);
                Servo_En_1_Write(1);
                CyDelay(10);
                dumb_placeholder++;
                set_servo(servoBottom,dumb_placeholder);
                set_servo(servoTop,dumb_placeholder);
                break;
            case OFF:
                set_servo(servoBottom,0);
                set_servo(servoTop,0);
        }
    }
}