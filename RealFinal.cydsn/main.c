#include <project.h>
#include <stdio.h>
#include <math.h>
#include "main.h"

#define  CHALLENGE_TIME 100

enum states{
    ACTIVE,
    OFF,
    VICTORY
};

unsigned int count = CHALLENGE_TIME;
uint8_t battery_state = 3;
uint8_t system_state = ACTIVE;
uint8_t last_key_state = 0;
int dumb_placeholder;
char* sentString = "this test";
char* inString = "";

typedef struct { // Servo object
    void (*pwmPtr)(uint16_t);
    uint16_t max;
    uint16_t min;
    double range;
} Servo;

typedef struct {
    void (*activatePtr)(uint8);
    
    uint16_t ANGLE_MAX;
    uint16_t ANGLE_MIN;
    uint16_t ANGLE_RESET;
    uint16_t angle_current;
    Servo Servo;
} Manipulator;

Manipulator CurrentManipulator;

void set_servo(Servo servo, uint8_t angle){ // Set servo to the angle specified
    double setpoint = angle * servo.range / 180 + servo.min;
    servo.pwmPtr(setpoint);
}

void set_current_manipulator(Manipulator manipulator){
    CurrentManipulator = manipulator;
    Servo_En_1_Write(0);
    Servo_En_2_Write(0);
    Servo_En_3_Write(0);
    
    set_servo(manipulator.Servo,manipulator.angle_current);
    
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
            UART_UartPutString("System active\n");
            set_battery_light(0b111);
            battery_state = 3;
            system_state = ACTIVE;
            break;
    
        case OFF:
            UART_UartPutString("System inactive\n");
            sentString = "System down\r";
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
                go_to_state(OFF);
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
                                uint16_t angle_min, uint16_t angle_max,
                                uint16_t per_full, uint16_t per_none,
                                uint16_t angle_reset)
{                                    
    
    Manipulator createdManipulator;
    createdManipulator.activatePtr = en_ptr;
                                    
    createdManipulator.ANGLE_MIN = angle_min;
    createdManipulator.ANGLE_MAX = angle_max;
    createdManipulator.ANGLE_RESET = angle_reset;
    createdManipulator.angle_current = angle_reset;
    createdManipulator.Servo = create_servo(PWM_1_WriteCompare,per_none,per_full);
    
    return createdManipulator;
}

void SetDesiredAngle(uint8 angle){
    if (angle > CurrentManipulator.ANGLE_MAX){
        angle = CurrentManipulator.ANGLE_MAX;
    } else if (angle < CurrentManipulator.ANGLE_MIN) {
        angle = CurrentManipulator.ANGLE_MIN;
    }
    
    CurrentManipulator.angle_current = angle;
}

void UpdatePositions(){
    if ((inString[0] == '1' || inString[0] == '2' || inString[0] == '3') && inString[1] == ' '){
            uint8 length = strlen(inString) - 1;
            uint8 i = 0;
            uint8 isValid = 1;
            char debug[80];
            double placeVal = 0;
            int holdingVar = 0;
            for(i = length; i > 2; i--){
                if(!isdigit(inString[i])){
                    isValid = 0;
                    i = 3;
                } else {
                    holdingVar += (inString[i] - '0') * (int) pow(10, placeVal);
                    placeVal++;
                }
            }
            
            if(inString[2] == '-') {
                holdingVar *= -1;
            } else if (isdigit(inString[2])){
                holdingVar += (inString[2] - '0') * (int) pow(10, placeVal); 
            }
            
            if(isValid){
                SetDesiredAngle(holdingVar);
//                sentString = "Is number\r";
//                sprintf(debug, "Length: %d\n", holdingVar);
//                UART_UartPutString(debug);
            }
            //SetDesiredAngle(CurrentManipulator.angle_current + holdingVar);
            //sprintf(sentString, "test");
        } else {
            UART_UartPutString(inString);    
        }
        inString = "";
}

int main()
{
    #ifdef LOW_POWER_MODE    
        CYBLE_LP_MODE_T         lpMode;
        CYBLE_BLESS_STATE_T     blessState;
    #endif
    
    CYBLE_API_RESULT_T      bleApiResult;
    
    CyGlobalIntEnable;
    
    /* Start UART and BLE component and display project information */
    UART_Start();   
    bleApiResult = CyBle_Start(AppCallBack); 
    
    if(bleApiResult == CYBLE_ERROR_OK)
    {
        #ifdef PRINT_MESSAGE_LOG
            UART_UartPutString("\n\r************************************************************");
            UART_UartPutString("\n\r***************** BLE UART example project *****************");
            UART_UartPutString("\n\r************************************************************\n\r");
            UART_UartPutString("\n\rDevice role \t: PERIPHERAL");
            
            #ifdef LOW_POWER_MODE
                UART_UartPutString("\n\rLow Power Mode \t: ENABLED");
            #else
                UART_UartPutString("\n\rLow Power Mode \t: DISABLED");
            #endif
            
            #ifdef FLOW_CONTROL
                UART_UartPutString("\n\rFlow Control \t: ENABLED");  
            #else
                UART_UartPutString("\n\rFlow Control \t: DISABLED");
            #endif
            
        #endif
    }
    else
    {
        #ifdef PRINT_MESSAGE_LOG   
            UART_UartPutString("\n\r\t\tCyBle stack initilization FAILED!!! \n\r ");
        #endif
        
        /* Enter infinite loop */
        while(1);
    }
    
    CyBle_ProcessEvents();
    
    /***************************************************************************
    * Main polling loop
    ***************************************************************************/
    Countdown_Start();
    timer_interrupt_StartEx(TIMER_ISR);
    CyGlobalIntEnable; /* Enable global interrupts. */
    PWM_1_Enable();
    set_battery_light(0b111);
    
    Manipulator Servo1 = create_manipulator(Servo_En_1_Write,
                                0, 180,
                                6750, 1400,
                                90);
    
    Manipulator Servo2 = create_manipulator(Servo_En_2_Write,
                                0, 180,
                                6750, 1400,
                                90);
    
    Manipulator Servo3 = create_manipulator(Servo_En_3_Write,
                                0, 180,
                                6750, 1400,
                                90);
    
    CurrentManipulator = Servo1;
    
    for(;;)
    {
        #ifdef LOW_POWER_MODE
            
            if((CyBle_GetState() != CYBLE_STATE_INITIALIZING) && (CyBle_GetState() != CYBLE_STATE_DISCONNECTED))
            {
                /* Enter DeepSleep mode between connection intervals */
                
                lpMode = CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);
                CyGlobalIntDisable;
                blessState = CyBle_GetBleSsState();

                if(lpMode == CYBLE_BLESS_DEEPSLEEP) 
                {   
                    if((blessState == CYBLE_BLESS_STATE_ECO_ON || blessState == CYBLE_BLESS_STATE_DEEPSLEEP) && \
                            (UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) == 0u)
                    {
                        #ifdef FLOW_CONTROL
                        EnableUartRxInt();
                        #endif
                        
                        CySysPmSleep();
                        
                        #ifdef FLOW_CONTROL
                        DisableUartRxInt();
                        #endif
                    }
                }
                else
                {
                    if((blessState != CYBLE_BLESS_STATE_EVENT_CLOSE) && \
                            (UART_SpiUartGetTxBufferSize() + UART_GET_TX_FIFO_SR_VALID) == 0u)
                    {
                        #ifdef FLOW_CONTROL
                        EnableUartRxInt();
                        #endif
                        
                        CySysPmSleep();
                        
                        #ifdef FLOW_CONTROL
                        DisableUartRxInt();
                        #endif
                    }
                }
                CyGlobalIntEnable;
                
            }
        #else
            HandleLeds();
        #endif
        
        /*******************************************************************
        *  Process all pending BLE events in the stack
        *******************************************************************/
        HandleBleProcessing();
        CyBle_ProcessEvents();
        
        switch(system_state){
            case ACTIVE:
                UpdatePositions();
                set_servo(CurrentManipulator.Servo, CurrentManipulator.angle_current);
                //Servo_En_2_Write(0);
                //Servo_En_1_Write(1);
                //CyDelay(10);
                //dumb_placeholder++;
                //set_servo(servoBottom,dumb_placeholder);
                //set_servo(servoTop,dumb_placeholder);
                break;
            case OFF:
                //if K
                //set_servo(servoBottom,0);
                //set_servo(servoTop,0);
                break;
        }
    }
}