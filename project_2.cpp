#include "mbed.h"
#include "TCS3472_I2C.h"
#include "TextLCD.h"
#include "Keypad.h"
 #include "StepperMotorUni.h"
 #include "Servo.h"
#include "HCSR04.h"
#include<math.h>
#define NUMCOL 3
PwmOut led1(PTA12);
PwmOut led2(PTD4);
PwmOut iff(PTD3);

HCSR04 HCSR04(PTC9,PTC8);
Servo myservo(PTA13);
StepperMotorUni motor( PTB0, PTB1, PTB2, PTB3 );
TCS3472_I2C rgb_sensor( PTE0, PTE1);
TextLCD my_lcd(PTE20, PTE21, PTE22, PTE23, PTE29, PTE30, TextLCD::LCD16x2);
Keypad kpad(PTE5, PTE4,PTE3, PTE2, PTB11,PTB10,PTB9,PTB8);

void add_color(int stored_color[][4],int index,int r,int g,int b,int c);
int object_recognition(int total_object,int stored_object[],int sensor_data[]);
int main() {
    start:
    
    int rgb_readings[4],index = 0,i = 0,j,object_number,selected_object,initial_pos,released = 1,separated = 0,total = 0;
    double avg_error = 0;
    char key,inputArray[16],charr;
    
    rgb_sensor.enablePowerAndRGBC();
    rgb_sensor.setIntegrationTime( 100 );
    
    my_lcd.printf("Object");
    my_lcd.locate(0,1);
    my_lcd.printf("Separation");
    wait(3);
    my_lcd.cls();
    my_lcd.printf("How many");
    my_lcd.locate(0,1);
    my_lcd.printf("objects?");
    wait(2);
    my_lcd.cls();
    my_lcd.printf("Total and");
    my_lcd.locate(0,1);
    my_lcd.printf("Selected:");
    
      while(i<16) //turns 16 times to get the distance value from the user
    {   here2:
        key = kpad.ReadKey(); //read the current key pressed
        if(key == '\0')
        released = 1; //set the flag when all keys are released
        if (key == '*'){
            my_lcd.cls();
            }
        if((key != '\0') && (released == 1))  //if a key is pressed AND previous key was released
        {if (key == '#'){
            goto got;
        }
        else if (key == 'D')
            my_lcd.printf(" ");
        else{
            my_lcd.printf("%c", key);
            }
        
        inputArray[i] = key;
        
        i++;
        released = 0; //clear the flag to indicate that key is still pressed
        goto here2;
        }
    }
    got:
    sscanf(inputArray,"%d %c %d", &object_number,&charr,&selected_object); //dig1 stores the range of the object entered by the user
    my_lcd.cls();
    my_lcd.printf("Scan objects");
    int color_arr[object_number][4];
     
    store:
    key = kpad.ReadKey(); //read the current key pressed
    if (key == '*'){
       my_lcd.cls();
        goto start;}
    else if(key == 'A')
     {  my_lcd.cls();
        my_lcd.printf("%d. scanned",index+1);
        rgb_sensor.getAllColors(rgb_readings);
        add_color(color_arr,index,rgb_readings[0],rgb_readings[1],rgb_readings[2],rgb_readings[3]);
        index = index +1;
        wait(1);
        goto store;
     }
     else 
     { if(index == object_number)
        {
            my_lcd.cls();
            my_lcd.printf("%d object stored",object_number);
            goto object_to_separate;
        }
     goto store;
     }
     
    object_to_separate:
    initial_pos = myservo.read();
    
    while((HCSR04.distance(1) > 10))
    {    motor.set_pps(500 );
         motor.move_steps( -200 );
         wait(0.4);
    }
    wait(2);
    rgb_sensor.getAllColors(rgb_readings);
    total += 1;
    
    int found = 0,error[object_number];
    for(i = 0;i <object_number;i++)
    {   for(j= 0;j <=3;j++)
        {   avg_error += abs(rgb_readings[j] - color_arr[i][j]);
        }
        avg_error /= 4.0;
        error[i] = avg_error;
        if (error[found] > error[i])
            found = i;
        avg_error  = 0;
    }  
    
    
    if (found+1 != selected_object){
        myservo.position(initial_pos-80);
        wait(1);
        myservo.position(initial_pos+7);}
    else
        separated += 1;
    my_lcd.cls();
    my_lcd.printf("Separated: %d",separated);
    my_lcd.locate(0,1);
    my_lcd.printf("Total objects: %d",total);
    
    motor.move_steps( -800 );
     wait(1.6);
    
    goto object_to_separate;       
}

void add_color(int stored_color[][4],int index,int r,int g,int b,int c)
{   stored_color[index][0] = r;
    stored_color[index][1] = g;
    stored_color[index][2] = b;
    stored_color[index][3] = c;
}

int object_recognition(int total_object,int stored_object[][4],int sensor_data[])
{   int found = 0,error[total_object],i,j,avg_error;
    for(i = 0;i <total_object;i++)
    {   for(j= 0;j <=3;j++)
        {   avg_error += abs(sensor_data[j] - stored_object[i][j]);
        }
        avg_error /= 4.0;
        error[i] = avg_error;
        if (error[found] > error[i])
            found = i;
        avg_error  = 0;
    }  
    return(found);
  }




