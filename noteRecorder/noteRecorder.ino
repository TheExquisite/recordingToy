// Author: James Clare
// Year: 2017
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//The purpose of this program is to control the pitch of a small speaker, along with allowing for the pitch and duration of each played note to be recorded and played back.
//The device requires careful memory managment to store the recorded notes as the arduinos have limited Random Access Memory and memory leaks must be avoided.
//Thus, this program utalizes cycleable multidimentional arrays to store the recorded notes while conserving memory space.
//The device also displays the position of the pitch potentiometer on a seven segement display. Each of the digits to be displayed are stored as arrays of the segements
//need to be lit up to display that digit. These arrays are then be passed into a fucntion which turns on the segements to display the digit.
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

 //Initialise any non-costant variables used by the speaker code
 //Most of the varibles used in the recording feature had to be of type double
 //to have enough storage space to hold the required data.
 int currentTone = 0;
 bool lastButtonState = false;
 bool pbLastButtonState = false;
 double pressTime = 0;
 double releaseTime = 0;
 double upTime = 0;
 double downTime = 0;
 //The song varibale is a multidimentional array of type double.
 //This array is used to store the tone and pitch of each not in the recorded song.
 double song[(songArraySize + 1)][2] = {{}};

//the setup function that begins the serial monitor, clears the display and declares the input and output pins
void setup() {
  //open serial port for deubgging
  Serial.begin(9600);
  Serial.println("Serial On");
  //call clear display function to clear display
  clearDisplay();
  
  //For loop used to set outputs without having to write out 8 lines of code.
  for(int i = 2; i <= 9; i++){ pinMode(i,OUTPUT); }
  //Set input pins
  pinMode(13, INPUT);
  pinMode(10, INPUT);
  
}

//Loop function used to update all aspects of the device
void loop() {
 //the private variable nob is used to map the potentiometer input between 0 and 9.
 int nob = map(analogRead(A0), 0, 1023, 0, 9);
 //Using the mapped potentiometer value the corresponding digit array is taken from 
 //the digits array and is passed into the display function.
 displayNumber(digits[nob]);

 //The varaiable currentTone maps the value of the potentiometer to the target min and max tone of the speakers.
 currentTone = map(analogRead(A0), 0, 1023, 2000, 4000);
 //If the speaker button is pressed then the speaker plays the current tone.
 if(!digitalRead(13)){
  tone(speakerPin, currentTone, 100);
 }

 //call the playBackPressed function.
 playbackPressed();
 //call the timeRecording function.
 timeRecording();
}

//The clear display function sets every pin of the seven segment display to low using a For loop.
void clearDisplay(){
  for(int i = 2; i<11; i++){
    digitalWrite(i,LOW);
  }
}

//The display number function takes the passed in array which contains the pins that need to be lit up to display 
//the corresponding digit on the display and turns on the correct pins using a For loop.
void displayNumber(int number[]){
  clearDisplay();
  for(int x = 0; x<8; x++){
    digitalWrite(number[x],HIGH);
  }
}

//The timeRecording function is used to record the time and pitch of the note that is currently being played 
//on the device and store it in the multidimentional array named song.
void timeRecording(){
   //the variable buttonState stores the current of the recording button.
   bool buttonState = digitalRead(13);
   //If the there has been a change in the state of the button then perform the recording code.
   if( buttonState != lastButtonState ){
    //If the button has just changed to being down then record the up time,
    if(!buttonState){
      //pressTime stores the time the button was last pressed.
      pressTime = millis();
      //upTime is the time between the last release and last press.
      upTime = (pressTime - releaseTime);
      //Pass the upTime and a tone of zero into the cycleArray function to add them to the song array.
      cycleArray(upTime, 0);
      //Write the stored upTime and tone to the debug console.
      Serial.print("upTime: ");
      Serial.println(song[songArraySize][0]);
      Serial.print("Tone: ");
      Serial.println(song[songArraySize][1]);
      Serial.println(" ");
    } 
    //If the button has just changed to being up then record the down time and tone.
    if(buttonState){
      //releaseTime stores the time the button was last released.
      releaseTime = millis();
      //downTime is the time between the last press and the last release.
      downTime = (releaseTime - pressTime);
      //Pass the downTime and the current tone into the cycleArray function to add them to the song array.
      cycleArray(downTime, currentTone);
      //Write the stored downTime and tone to the console for debugging.
      Serial.print("downTime: ");
      Serial.println(song[songArraySize][0]);
      Serial.print("Tone: ");
      Serial.println(song[songArraySize][1]);
      Serial.println(" ");
    }
    //Set the varible lastButtonState to the current button state at the end of the 
    //block so that changes in the button state can be detected.
    lastButtonState = buttonState;
   }
}

//The playBackPressed function is intended to check to see if the playback button has been pressed.
//It functions identiaclly to the button state checking in the timeRecording function.
void playbackPressed(){
    //Store current button state.
    bool pbButtonState = digitalRead(11);
    //check to see if the button has changed state.
    if(pbButtonState != pbLastButtonState){
      //if the button is pressed then call the playback function.
      if(!pbButtonState){
        Serial.println("Playback Pressed");
        playback();
      }
      //Set the play back last button state varibaled to the current button state at the end of the 
      //block so that changes in the button state can be detected.
      pbLastButtonState = pbButtonState;
    }
  }

//The playback function is intended to playback the previously played notes that have been stored in the song array.
void playback(){
    //For each item in the song array use the first value as the duration of the note and the second value as the tone of the note.
    for(int i = 0; i < (songArraySize + 1); i++){
      //If the element has a tone of zero the delay for the duration of the note,
      if(song[i][1] == 0){
        delay(song[i][0]);
      } 
      //else play a note that has a pitch of the element and the duration of the element.
      else {
        tone(speakerPin, song[i][1], song[i][0]);
      }
    }
  }

//The cycleArray function is intended to move each element in the songs array forward 
//one position and then add the new element to the end of the array.
void cycleArray(double _time, double _tone){
    //For each element in the songs array
    for(int i = 0; i < (songArraySize + 1); i++){
      //if the element is not the last element, replace it with the element after it.
      if(i <= (songArraySize - 1)){
        song[i][0] = song[i+1][0];
        song[i][1] = song[i+1][1];
      } 
      //Else if it is the last element then store the duration and tone of the new note.
      else {
        song[songArraySize][0] = _time;
        song[songArraySize][1] = _tone;
      }
    }
  }

