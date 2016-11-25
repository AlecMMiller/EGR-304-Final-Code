#include <project.h>

#define  CHALLENGE_TIME 10

enum states{
    ACTIVE,
    OFF,
    VICTORY
};

unsigned int count = CHALLENGE_TIME;
uint8_t battery_state = 3;
uint8_t system_state = ACTIVE;
int dumb_placeholder;

typedef struct { // Servo object
    void (*pwmPtr)(uint16_t);
    uint16_t max;
    uint16_t min;
    double range;
} Servo;

typedef struct {
    void (*activatePtr)(uint8);
    
    uint16_t YAW_MAX;
    uint16_t YAW_MIN;
    uint16_t YAW_RESET;
    uint16_t yaw_current;
    Servo yawServo;
    
    uint16_t PITCH_MAX;
    uint16_t PITCH_MIN;
    uint16_t PITCH_RESET;
    uint16_t pitch_current;
    Servo pitchServo;
} Manipulator;

void set_servo(Servo servo, uint8_t angle){ // Set servo to the angle specified
    double setpoint = angle * servo.range / 180 + servo.min;
    servo.pwmPtr(setpoint);
}

void set_current_manipulator(Manipulator manipulator){
    Servo_En_1_Write(0);
    Servo_En_2_Write(0);
    Servo_En_3_Write(0);
    
    set_servo(manipulator.pitchServo,manipulator.pitch_current);
    set_servo(manipulator.yawServo,manipulator.yaw_current);
    
    manipulator.activatePtr(1);
}

void set_battery_light(uint8_t setValue){
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

void go_to_state(uint8_t state){
    system_state = state;
    
    switch(state) {
        case ACTIVE:
            set_battery_light(0b111);
            battery_state = 3;
            system_state = ACTIVE;
            break;
    
        case OFF:
            break;
        
        case VICTORY:
            break;
    }
}

CY_ISR(TIMER_ISR){
    if (system_state == ACTIVE){
        count--;
        if(count == 0){
         count = CHALLENGE_TIME;
         battery_state--;
            
         switch(battery_state){
            case 2:
                set_battery_light(0b011);
                break;
                
            case 1:
                set_battery_light(0b001);
                break;
                
            case 0:
                set_battery_light(0b000);
                system_state = OFF;
                break;
                
           }
        }
    }
}

Servo create_servo(void (pwm_ptr)(uint16_t), uint16_t min, uint16_t max){ // Initializes the servo object
    Servo createdServo;
    createdServo.max = max;
    createdServo.min = min;
    createdServo.range = max - min;
    createdServo.pwmPtr = pwm_ptr;
    return createdServo;
}

Manipulator create_manipulator(void (en_ptr)(uint8),
                                uint16_t yaw_min, uint16_t yaw_max,
                                uint16_t yaw_reset,
                                uint16_t yaw_full, uint16_t yaw_none,
                                uint16_t pitch_min, uint16_t pitch_max,
                                uint16_t pitch_full, uint16_t pitch_none,
                                uint16_t pitch_reset)
{                                    
    
    Manipulator createdManipulator;
    createdManipulator.activatePtr = en_ptr;
                                    
    createdManipulator.PITCH_MAX = pitch_max;
    createdManipulator.PITCH_MIN = pitch_min;
    createdManipulator.PITCH_RESET = pitch_reset;
    createdManipulator.pitch_current = pitch_reset;
    createdManipulator.pitchServo = create_servo(PWM_1_WriteCompare1,pitch_none,pitch_full);
    
    createdManipulator.YAW_MAX = yaw_max;
    createdManipulator.YAW_MIN = yaw_min;
    createdManipulator.YAW_RESET = yaw_reset;
    createdManipulator.yaw_current = yaw_reset;
    createdManipulator.yawServo = create_servo(PWM_1_WriteCompare2,yaw_none,yaw_full);
    
    return createdManipulator;
}

int main()
{
    Countdown_Start();
    timer_interrupt_StartEx(TIMER_ISR);
    CyGlobalIntEnable; /* Enable global interrupts. */
    PWM_1_Enable();
    Servo servoBottom =  create_servo(PWM_1_WriteCompare1,1400,6750);
    Servo servoTop =  create_servo(PWM_1_WriteCompare2,6900,1650);
    set_battery_light(0b111);
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