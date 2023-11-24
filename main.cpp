#include "mbed.h"
#include <cstdint>
#include <cstdio>
#include <string>
using namespace std;



#define WAIT_TIME_MS 50 
#define NOTE_C4  262    //difine note values for buzzer
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
PwmOut buzzer(PA_15);
const int C_major_scale[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5}; //create array with the required notes (in order)
void play_note(int frequency);

BufferedSerial serial(USBTX, USBRX); //for serial out
DigitalOut redled(PC_0); //difining led, pot and display ports
DigitalOut led1(PA_4);
DigitalOut led2(PB_0);
DigitalOut led3(PC_1);
BusOut leds_bus(PC_0, PC_1, PB_0, PA_4);
DigitalIn button1(PC_10);
DigitalIn button2(PC_11);
DigitalIn button3(PD_2);
BusOut SegDis(PA_11, PA_12, PB_1, PB_15, PB_14, PB_12, PB_11 );
AnalogIn pot1(PA_5);
AnalogIn pot2(PA_6);
AnalogIn pot3(PA_7);
PwmOut red_LED(PB_5);   //PWM output to red led
PwmOut green_LED(PB_3); //PWM output to green led
PwmOut blue_LED(PB_4);  //PWM output to blue led
AnalogIn FSR(PA_1); //Force sensor pin
float LDR_val;


bool check_on = false; //for on off function

float pot_val1 = 0.0f; 
float pot_val2 = 0.0f;
float pot_val3 = 0.0f;


uint8_t pot_select1 = 0; 
uint8_t pot_select2 = 0;
uint8_t pot_select3 = 0; 

void Washer_on();  
void Washer_off();
void cycle_select(bool cycle_check, float& pot_val, uint8_t& pot_select, AnalogIn& pot,PwmOut&  Multi_led);
void cycle_selected();
void Temp_selected();
void Length_selected();
void Multi_Led_init();
float frequency = 100; 
void Multi_Led_flash();
void Cycle_start();
void Cycle_perameters();
int seconds;
int secondsUpdated = 0;
int sectimer = 0;


int main()
{
    serial.set_baud(115200);
printf("welcome\n");
    while(1){
    if(button1.read() == 1){
        check_on = !check_on; //checks if button one is pressed for on/off functionality
        ThisThread::sleep_for(1s);
    if(check_on){
            redled.write(1); 
            
            Washer_on(); // start up sequence with leds and buzzer effects and initilises all components
            serial.write("Washing Machine On\n", strlen("Washing Machine On\n"));
            cycle_select(check_on, pot_val1, pot_select1, pot1, green_LED ); // for each pot, the value is sent to the function which determines which option has been selected and led to confirm choice
            cycle_select(check_on, pot_val2, pot_select2, pot2, red_LED ); //repeat to read pot 2
            cycle_select(check_on, pot_val3, pot_select3, pot3, blue_LED ); //repeated for pot 3 values
            SegDis.write(0b111111);
            cycle_selected(); // this funtion takes pot 1 value that has been selected and tells the user which cycle they have selected
            Temp_selected(); // takes the pot 2 value and displayes the selected temp
            Length_selected(); // takes the pot 3 vale relating to the size of load selected
            string temp_seconds = to_string(seconds) + "\n";
            serial.write("Time set is ", strlen("Time set is "));
            serial.write(temp_seconds.c_str(), strlen(temp_seconds.c_str()));

            
            thread_sleep_for(500);
            Cycle_start(); //takes the value of seconds calculated and starts the timer for the wash with ending sequence
            printf("Start another cycle?\n");
            
            



        }
    else{ //sets everything to zero as the off funtion
            leds_bus.write(0);
            SegDis.write(0);
            red_LED.write(0);
            green_LED.write(0);
            blue_LED.write(0);
            pot_val1 = 0;
            pot_val2 = 0;
            pot_val3 = 0;
        }
        thread_sleep_for(500);
        }
    }
}



void Washer_on(){
while(1){

for(int i = 4; i < 8; i++){         //iterate through the C_major_scale array
                play_note(C_major_scale[i]);    //pass the note at position C_major_scale[i] to function
            }

            buzzer.pulsewidth_us(0);            //turn off buzzer
                
            

                    led1.write(1); //seqence for leds when turned on
            ThisThread::sleep_for(500ms);
            led2.write(1);
            ThisThread::sleep_for(500ms);
            led3.write(1);
            ThisThread::sleep_for(500ms);
            redled.write(1);
            ThisThread::sleep_for(500ms);
            leds_bus.write(0);
            ThisThread::sleep_for(250ms);
            leds_bus.write(15);
            ThisThread::sleep_for(250ms);
            leds_bus.write(0);
            ThisThread::sleep_for(250ms);
            leds_bus.write(15);
            ThisThread::sleep_for(250ms);
            SegDis.write(0b111111);
            leds_bus.write(0);
            break;

                }
}


void play_note(int frequency){
    buzzer.period_us((float) 1000000.0f/ (float) frequency);    //set the period of the pwm signal (in us)
    buzzer.pulsewidth_us(buzzer.read_period_us()/2);            //set pulse width of the pwm to 1/2 the period
    ThisThread::sleep_for(500ms);                               //play sound for 500ms
}



void cycle_select(bool cycle_check, float& pot_val, uint8_t& pot_select, AnalogIn& pot,PwmOut&  Multi_led){

   
   
    while(cycle_check){

        if(button1.read() == 1)
        {
        
             check_on = !check_on; //checks for button one presses to turn off and on
            
            thread_sleep_for(500);
        }
        else
        {
            pot_val = pot.read() * 3.3; //takes the pot values and splits it into 4 and displays a range of 0-4 depending on the cycle being selected
            thread_sleep_for(500);
            if(pot_val < 0.4){
                SegDis.write(0x3F);
            }
            else if(pot_val > 0.4 && pot_val < 1.0){
                SegDis.write(0x06);
            }
            else if(pot_val >= 1.01 && pot_val < 1.8){
                SegDis.write(0x5B);
            }
            else if(pot_val >= 1.81 && pot_val < 2.6){
                SegDis.write(0x4F);
            }
            else{
                SegDis.write(0xE6);
            }
            if(button2.read() == 1){
                if(SegDis.read() != 0x3F) //makes sure the value is not zero
                { 
                    pot_select = SegDis.read(); 
                    Multi_led.write(1);
                    cycle_check = !cycle_check; //lights led and breaks the loop
                }   
                    thread_sleep_for(500);
                }
                
        }
            
           
            }
 
}

void cycle_selected(){
    while(check_on){
        if(button1.read() == 1)
        {
        
            check_on = !check_on; //checks for on/off
            thread_sleep_for(500);
        }
        else{
            if(pot_select1 == 0x06){ //reads the hex value that was displayed  when selecting and states the selection
                serial.write("The Wash Type Selected:\n", strlen("The Wash Type Selected:\n")); 
                serial.write("Cotton\n", strlen("Cotton\n"));
                seconds = 45;
                break;
            }
            else if(pot_select1 == 0x5B){ //repeates for all 4 possible choices
                serial.write("The Wash Type Selected:\n", strlen("The Wash Type Selected:\n"));
                serial.write("Synthetics\n", strlen("Synthetics\n"));
                seconds = 30;
                break;
            }
            else if(pot_select1 == 0x4F){
                serial.write("The Wash Type Selected:\n", strlen("The Wash Type Selected:\n"));
                serial.write("Delicates\n", strlen("Delicates\n"));
                seconds = 20;
                break;
            }
            else if(pot_select1 == 0xE6){
                serial.write("The Wash Type Selected:\n", strlen("The Wash Type Selected:\n"));
                serial.write("Sports\n", strlen("Sports\n"));
                seconds = 35;
                break;
            }


        }

    }
    
}


void Temp_selected(){
    while(check_on){
        if(button1.read() == 1)
        {
        
        check_on = !check_on;
        thread_sleep_for(500);
        }
        else{
            if(pot_select2 == 0x06){ //reads the second pots values and states the temp selected from one of four hex values displayed
                serial.write("The Wash Temp Selected:\n", strlen("The Wash Temp Selected:\n"));
                serial.write("30\n", strlen("30\n"));
                break;
            }
            else if(pot_select2 == 0x5B){
                serial.write("The Wash Temp Selected:\n", strlen("The Wash Temp Selected:\n"));
                serial.write("40\n", strlen("40\n"));
                break;
            }
            else if(pot_select2 == 0x4F){
                serial.write("The Wash Temp Selected:\n", strlen("The Wash Temp Selected:\n"));
                serial.write("50\n", strlen("50\n"));
                break;
            }
            else if(pot_select2 == 0xE6){
                serial.write("The Wash Temp Selected:\n", strlen("The Wash Temp Selected:\n"));
                serial.write("60\n", strlen("60\n"));
                break;
            }


        }

    }
    
}


void Length_selected(){
    while(check_on){
        if(button1.read() == 1)
        {
            check_on = !check_on;
            thread_sleep_for(500);
        }
        else{
            if(pot_select3 == 0x06){ //reads from the third pot values and states the chosen load size from the hex value that was displayed
                serial.write("The Wash Load Size Selected:\n", strlen("The Wash Load Size Selected:\n"));
                serial.write("Small\n", strlen("Small\n"));
                seconds = seconds*0.3;

                break;
            }
            else if(pot_select3 == 0x5B){
                serial.write("The Wash Load Size Selected:\n", strlen("The Wash Load Size Selected:\n"));
                serial.write("Medium\n", strlen("Medium\n"));
                seconds = seconds*0.6;
                break;
            }
            else if(pot_select3 == 0x4F){
                serial.write("The Wash Load Size Selected:\n", strlen("The Wash Load Size Selected:\n"));
                serial.write("Large\n", strlen("Large\n"));
                seconds = seconds*0.8;
                break;
            }
            else if(pot_select3 == 0xE6){
                serial.write("The Wash Load Size Selected:\n", strlen("The Wash Load Size Selected:\n"));
                serial.write("Full\n", strlen("Full\n"));
                break;
            }


        }

    }
    
}


void Multi_Led_init(){
    red_LED.write(0.0);                 //initialise leds with inital PWM value to 0.0 = 0%
    green_LED.write(0.0); 
    blue_LED.write(0.0);
        
    red_LED.period(1/frequency);        //set the period of the wave form as 1/100Hz
    green_LED.period(1/frequency); 
    blue_LED.period(1/frequency);

}


void Multi_Led_flash(){ //for pulsing multi led
    while (check_on)
    {
      if(button1.read() == 1)
        {
        
        check_on = !check_on;
        thread_sleep_for(500);
        }
      else for(int i = 0; i < 4; i++){
        for(float PWM = 0.00; PWM <= 1.00; PWM += 0.02){   //cycle up from 0% duty cycle to 100% in increments of 2% 
                red_LED.write(PWM);                        //write PWM duty cycle to LED
                thread_sleep_for(WAIT_TIME_MS);
        }
        red_LED.write(0.0);                                //Set the red LED to 0 after the for loop completes
        thread_sleep_for(500);                            // Sleep for 1 s to make the transition between colours more obvious

        for(float PWM = 0.00; PWM <= 1.00; PWM += 0.02){   //cycle up from 0% duty cycle to 100% in increments of 2% 
                green_LED.write(PWM);                      //write PWM duty cycle to LED
                thread_sleep_for(WAIT_TIME_MS);
        }
        green_LED.write(0.0);                               //Set the green LED to 0 after the for loop completes
        thread_sleep_for(500);                             // Sleep for 1 s to make the transition between colours more obvious
 
         for(float PWM = 0.00; PWM <= 1.00; PWM += 0.02){   //cycle up from 0% duty cycle to 100% in increments of 2% 
                blue_LED.write(PWM);                      //write PWM duty cycle to LED
                thread_sleep_for(WAIT_TIME_MS);
        }
        blue_LED.write(0.0);                               //Set the green LED to 0 after the for loop completes
        thread_sleep_for(500); }
    
       }   
}


void Cycle_start(){
    red_LED.write(0.0);                
    green_LED.write(0.0); 
    blue_LED.write(0.0);
    
   


    auto start_time = std::chrono::system_clock::now();
    auto end_time = start_time + std::chrono::seconds(seconds);

    while (button3.read() != 1 && std::chrono::system_clock::now() < end_time) {
        int remaining_time = std::chrono::duration_cast<std::chrono::seconds>(end_time - std::chrono::system_clock::now()).count();
        char buffer[50];
         int length = sprintf(buffer, "Time remaining: %d seconds\n", remaining_time);
        serial.write(buffer, length);
       thread_sleep_for(500);
       thread_sleep_for(500);
       green_LED.write(1.0f);
       LDR_val= FSR.read();
       
    
    }

    if (button3.read() == 1) {
        serial.write("Timer stopped by user\n", strlen("Timer stopped by user\n")); //if button 3 pressed during the timer it stops the cycle
        green_LED.write(1);
        redled.write(1);
        play_note(392);
        buzzer.pulsewidth_us(0);  
    } 
    else if (FSR.read() > 0.5){
        serial.write("Timer stopped by user\n", strlen("Timer stopped by user\n")); //if FSR senses a value over 0.5 the timer stops
        green_LED.write(1);
        redled.write(1);
        play_note(392);
        buzzer.pulsewidth_us(0);  

    }
    else {
        serial.write("Wash Cycle Complete\n", strlen("Wash Cycle Complete\n")); //final sequence of leds and lights when finished
            play_note(523);
        play_note(494);
        play_note(440);
        play_note(392);
        buzzer.pulsewidth_us(0);  
        redled.write(1);
        ThisThread::sleep_for(500ms);
        led3.write(1);
        ThisThread::sleep_for(500ms);
        led2.write(1);
        ThisThread::sleep_for(500ms);
        led1.write(1);
        ThisThread::sleep_for(500ms);
        leds_bus.write(0);
        ThisThread::sleep_for(250ms);
        leds_bus.write(15);
        ThisThread::sleep_for(250ms);
        leds_bus.write(0);
        ThisThread::sleep_for(250ms);
        leds_bus.write(15);
        ThisThread::sleep_for(250ms);
        leds_bus.write(0);
        red_LED.write(0);
         
        
    
}








}