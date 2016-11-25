#include <project.h>

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
    CyGlobalIntEnable; /* Enable global interrupts. */
    PWM_1_Enable();
    struct Servo servoBottom =  create_servo(PWM_1_WriteCompare1,1400,6750);
    struct Servo servoTop =  create_servo(PWM_1_WriteCompare2,6900,1650);
    for(;;)
    {
        CyDelay(5000);
        set_servo(servoBottom, 45);
        set_servo(servoTop, 45);
    }
}
