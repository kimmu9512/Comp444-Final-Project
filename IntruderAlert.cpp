#include <Arduino.h>
//#define CUSTOM_SETTINGS
//#define INCLUDE_CAMERA_MODULE
//#include <Dabble.h>
#include <Wire.h>                                                             
//global variables used in the program
const int trig = 13;
const int echo = 12;
const int trig1 = 10;
const int echo1 = 9; 
float distance;
float distance1;
float distance_prime;
float distance_prime1;
bool intruder;
float time;
  unsigned char send_value[21];
  char converted_char[19];
/****************** ULTRA SONIC SENSOR MODULE  ********************/
float getDistance(const int myTrig, const int myEcho)
{
  float echoTime;                   //variable to store the time it takes for a ping to bounce off an object
  float calculatedDistance;         //variable to store the distance calculated from the echo time

  //send out an ultrasonic pulse that's 10ms long
  digitalWrite(myTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(myTrig, LOW);

  echoTime = pulseIn(myEcho, HIGH);      //use the pulsein command to see how long it takes for the
                                          //pulse to bounce back to the sensor

  calculatedDistance = echoTime / 148.0;  //calculate the distance of the object that reflected the pulse (half the bounce time multiplied by the speed of sound)

  return calculatedDistance;              //send back the distance that was calculated
}
unsigned char get_checksum(unsigned char b[],int k)
 {
    unsigned char addition_checksum =0;
    unsigned char sum =0;
    for(int i=0;i<k;i++){
      	sum^=b[i];
       
    }
    addition_checksum=~sum;
    return addition_checksum;
}
void send_data(float my_data, int sensor){

  dtostrf(my_data, 6,2,converted_char);
  for( int i = 0 ; i < 19;i++){
    send_value[i+2] = (unsigned char) converted_char[i];
  }
  //add check sum value to before sending it .
  send_value[0] = get_checksum(converted_char,19);
  //start recording value 
  if(sensor == 0){

    send_value[1] = 0;
  }
  //capture camera
  else if( sensor ==1){

  send_value[1] = 1;
  }
  Serial.write(send_value,20);
  delay(100);
}

// reactive control for Ultrasonic sensor 
void check_ultra_sensor(){
    distance_prime = getDistance(trig,echo);
  distance_prime1 = getDistance(trig1,echo1);
  int camera_option = -1;
  //capturing rate of distance difference every fifth of a second
  if ((float)millis() - time >= 200) {
    /*
    Serial.print("distance prime is :");
    Serial.println(distance_prime);
    Serial.print("distance prime 1 is : ");
    Serial.println(distance_prime1);
    */
    //if both sensors change it means a person is definitely entering.
      if (distance_prime - distance > 5 && distance_prime1 -distance1 > 5) {
      camera_option = 0;
    }
    //if only one sensor changes
    else if(distance_prime - distance >5){
        camera_option = 1;
        time = millis();
        //check if other sensor reads as well for a second
        while (time + 1000 >millis()){
          distance_prime1 = getDistance(trig1,echo1);
          if (distance_prime1-distance1 > 5){
            camera_option =0;
            break;
          }
        }
     } 
     else if (distance_prime1-distance1 > 5){
       camera_option =1;
        time = millis();
        //check if other sensor reads as well for a second
        while (time + 1000 >millis()){
          distance_prime = getDistance(trig,echo);
          if (distance_prime-distance > 5){
            camera_option =0;
            break;
          }
        }
     }
      //within 2 seconds if we notice a change in the other sensor 
          //both sensors activated, meaning for sure someone came in
  if (camera_option != -1){
    /*
    Serial.println("camera option is: ");
    Serial.println(camera_option);
    */
    send_data(1.23,camera_option);
    delay(10000);
  }

  time = millis();
  distance = getDistance(trig,echo);
  distance1= getDistance(trig1,echo1);
  }

}

void setup() {
  Serial.begin(9600); 

  //Dabble.begin(9600); 
  //ULTRA SONIC SENSOR 
  pinMode(trig, OUTPUT);   
  pinMode(echo, INPUT);
  pinMode(trig1,OUTPUT);
  pinMode(echo1, INPUT);
  //capture current time and distance of the sesnor.
  time = millis();

  distance = getDistance(trig,echo);
  distance1= getDistance(trig1,echo1);
  // when robot starts  there is no intruder yet (obivously).
  intruder = false;
  

  //DABBLE 

  // put your setup code here, to run once:
}

void loop() {  

    check_ultra_sensor();
}
  
  