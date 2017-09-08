// Author: James Clare
// Year: 2017
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//The purpose of this program is to control the pitch of a small speaker, along with allowing for the pitch and duration of each played note to be recorded and played back.
//The device requires careful memory managment to store the recorded notes as the arduinos have limited Random Access Memory and memory leaks must be avoided.
//Thus, this program utalizes cycleable multidimentional arrays to store the recorded notes while conserving memory space.
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 
 #include <SoftwareSerial.h>
 
 //declare the digital port that each segement of the seven segment display connects too
 int top = 8;
 int bottom = 3;
 int leftTop = 7;
 int rightTop = 9;
 int leftBottom = 2;
 int rightBottom = 4;
 int middle = 6;
 int dot = 5;

 //declare arrays containing the segments of the seven segment display that must be lit up to create each number
 int zero[8] = {top, bottom, leftTop, leftBottom, rightTop, rightBottom};
 int one[8] = {leftTop, leftBottom};
 int two[8] = {top, middle, rightTop, leftBottom, bottom};
 int three[8] = {top, middle, bottom, rightTop, rightBottom};
 int four[8] = {rightTop, leftTop, middle, rightBottom};
 int five[8] = {top, middle, rightBottom, leftTop, bottom};
 int six[8] = {top, middle, bottom, leftTop, leftBottom, rightBottom};
 int seven[8] = {top, rightTop, rightBottom};
 int eight[8] = {top, bottom, leftTop, leftBottom, rightTop, rightBottom, middle};
 int nine[8] = {top, leftTop, rightTop, rightBottom, middle, bottom};

 //create an array containing all other digit arrays to allow easy referencing.
 int* digits[10] = { zero, one, two, three, four, five, six, seven, eight, nine };

 //delcare any constant varibles used by the speaker code
 const int speakerPin = 12;
 const int songArraySize = 19;

 //initialise any non-costant variables used by the speaker code
 int currentTone = 0;
 bool lastButtonState = false;
 bool pbLastButtonState = false;
 double pressTime = 0;
 double releaseTime = 0;
 double upTime = 0;
 double downTime = 0;
 double song[(songArraySize + 1)][2] = {{}};

//the setup function that begins the serial monitor, clears the display and declares the input and output pins
void setup() {
 Serial.begin(9600);
 Serial.println("Serial On");
 clearDisplay();

 for(int i = 2; i <= 9; i++){ pinMode(i,OUTPUT); }
 pinMode(13, INPUT);
 pinMode(10, INPUT);
  
}

void loop() {
 int nob = map(analogRead(A0), 0, 1023, 0, 9);
 displayNumber(digits[nob]);

 analogWrite(A1, analogRead(A0));
 digitalWrite(11, HIGH);
 currentTone = map(analogRead(A0), 0, 1023, 2000, 4000);
 
 if(!digitalRead(13)){
  tone(speakerPin, currentTone, 100);
 }

 playbackPressed();
 timeRecording();
}


void clearDisplay(){
  for(int i = 2; i<11; i++){
    digitalWrite(i,LOW);
  }
}

void displayNumber(int number[]){
  clearDisplay();
  for(int x = 0; x<8; x++){
    digitalWrite(number[x],HIGH);
  }
}

void timeRecording(){
   bool buttonState = digitalRead(13);
   if( buttonState != lastButtonState ){
    if(!buttonState){
      pressTime = millis();
      upTime = (pressTime - releaseTime);
      cycleArray(upTime, 0);
      Serial.print("upTime: ");
      Serial.println(song[songArraySize][0]);
      Serial.print("Tone: ");
      Serial.println(song[songArraySize][1]);
      Serial.println(" ");
    } 
    if(buttonState){
      releaseTime = millis();
      downTime = (releaseTime - pressTime);
      cycleArray(downTime, currentTone);
      Serial.print("downTime: ");
      Serial.println(song[songArraySize][0]);
      Serial.print("Tone: ");
      Serial.println(song[songArraySize][1]);
      Serial.println(" ");
    }
    lastButtonState = buttonState;
   }
}


void playbackPressed(){
    bool pbButtonState = digitalRead(11);
    if(pbButtonState != pbLastButtonState){
      if(!pbButtonState){
        Serial.println("Playback Pressed");
        playback();
      }
      pbLastButtonState = pbButtonState;
    }
  }

void playback(){
    for(int i = 0; i < (songArraySize + 1); i++){
      if(song[i][1] == 0){
        delay(song[i][0]);
      } else {
        tone(speakerPin, song[i][1], song[i][0]);
      }
    }
  }

void cycleArray(double _time, double _tone){
    for(int i = 0; i < (songArraySize + 1); i++){
      if(i <= (songArraySize - 1)){
        song[i][0] = song[i+1][0];
        song[i][1] = song[i+1][1];
      } else {
        song[songArraySize][0] = _time;
        song[songArraySize][1] = _tone;
      }
    }
  }

