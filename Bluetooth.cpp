#include <Arduino.h>
#define CUSTOM_SETTINGS
#define INCLUDE_CAMERA_MODULE
#include <Dabble.h>
#include <Wire.h>

const int buzzerPin = 9;              
float campture_camera_time = 0;
bool start_camera = false;
bool end_camera = false;
bool capture_camera = false;
    unsigned char receive_value[21];
  char converted_char[19];
//check sum and processing received data
bool check_sum(unsigned char c[],int k  )
{
  unsigned char checksum,sum=0;
  for(int i=2;i<k;i++){
    Serial.print(c[i]);
      sum^=c[i];
  }
  sum^=c[0];
  checksum=~sum;
  Serial.print("check sum is ");
  Serial.println(checksum);
  if (checksum ==0){
    return true;
  }
  else{
    return false;
  }
}
void receive_data(){

  float temp;
  Serial.readBytes(receive_value, 20);
  if(check_sum(receive_value,21) == true){
    for (int i = 0 ; i < 19; i++){
      converted_char[i]= receive_value[i+2];
      Serial.print(receive_value[i]);
    }
    Serial.println("");
    temp = atof(converted_char);
    if (receive_value[1] == 0){
      Serial.println("received data");
      start_camera=true;
      end_camera= true;
      receive_value[0] ='z';
    }
    if (receive_value[1] ==1){
      Serial.println("received data");
      capture_camera= true;
      receive_value[0] ='z';
    }

  }
  else {
    Serial.println("checksum failed");
  }
  //reset check sum  variables
  for (int i = 0 ; i < 19; i++){
      converted_char[i]= 0;
      receive_value[i] = 0;
  }
  receive_value[19] = 0;
  receive_value[20] = 0;
  
}
void setup() {
  Serial.begin(9600);
    pinMode(buzzerPin, OUTPUT);
  // put your setup code here, to run once:
    Dabble.begin(9600); 
  //buzzer pin for when taking a photo

}

void loop() {
  
  if ( capture_camera == false &&start_camera == false && end_camera == false&& millis() > campture_camera_time + 15000){
    receive_data();
  }
  if (start_camera == false && end_camera == true &&  millis() > campture_camera_time+ 10000){
    Camera.stopRecording();
    end_camera=false;
    Serial.println("camera ending");
  }
  if (start_camera == true){
    Serial.println(" camera starting");
    
       Camera.startRecording();
        
       Music.play("S1");
       campture_camera_time=millis();
       start_camera = false;
  }
  if (capture_camera == true){
    Serial.println("taking photo !");
    Camera.captureImage();
    tone(buzzerPin, 523, 2000);
    capture_camera=false;
  }
  // put your main code here, to run repeatedly:
}