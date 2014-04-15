/* Code adapted from Adafruit's player_simple.ino code for the VS1053 Library */

// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

#define RESET 9      // VS1053 reset pin (output)
#define CS 10        // VS1053 chip select pin (output)
#define DCS 8        // VS1053 Data/command select pin (output)
#define CARDCS 4     // Card chip select pin
#define DREQ 3       // VS1053 Data request, ideally an Interrupt pin

//pins for the telephone keypad
//since there aren't enough pins for the individual
//buttons, I have to have some buttons share a pin...
int hookswitch = 0;
int key_1_or_2 = 1;
int key_3_or_4 = 2;
int key_5_or_6 = 5;
int key_7_or_8 = 6;
int key_9_or_0 = 7;


bool key_pressed = false;
bool playing_audio = false;

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(RESET, CS, DCS, DREQ, CARDCS);

String phoneNumber = "";

void setup() {
  //Serial.begin(9600);
  SD.begin(CARDCS);    // initialise the SD card
  
  //pinMode(hookswitch, INPUT);
  pinMode(key_1_or_2, INPUT);
  pinMode(key_3_or_4, INPUT);
  pinMode(key_5_or_6, INPUT);
  pinMode(key_7_or_8, INPUT);
  pinMode(key_9_or_0, INPUT);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(100,100);
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
  
}

void loop() {
  int press1or2 = digitalRead(key_1_or_2);
  int press3or4 = digitalRead(key_3_or_4);
  int press5or6 = digitalRead(key_5_or_6);
  int press7or8 = digitalRead(key_7_or_8);
  int press9or0 = digitalRead(key_9_or_0);
  /*if(digitalRead(hookswitch) == HIGH) {
    if (!musicPlayer.stopped())
      musicPlayer.stopPlaying();
    key_pressed = false;
    playing_audio = false;
    phoneNumber = "";
  }*/

  //check phone string for the combinations
  
  if(!playing_audio && phoneNumber == "ebcccdfeed") { //my phone number
    playing_audio = true;
    musicPlayer.begin();
    musicPlayer.startPlayingFile("track002.mp3");
  } 
  else if(!playing_audio && phoneNumber == "eddedefefe") { //tekkoshocon
  //else if(!playing_audio && phoneNumber == "bd") { //testing
    //Serial.print("here\n");
    playing_audio = true;
    musicPlayer.begin();
    musicPlayer.startPlayingFile("rave.mp3");
  } 

  //otherwise continue to append to the string
  else if(!key_pressed && press1or2 == HIGH) {
    //Serial.print("pressed2\n");
    key_pressed = true;
    phoneNumber += "b";
  }
  else if(!key_pressed && press3or4 == HIGH) {
    key_pressed = true;
    phoneNumber += "c";
  }
  else if(!key_pressed && press5or6 == HIGH) {
    Serial.print("pressed5\n");
    key_pressed = true;
    phoneNumber += "d";
  }
  
  else if(!key_pressed && press7or8 == HIGH) { 
    Serial.print("pressed8\n");
    key_pressed = true;
    phoneNumber += "e";
  }
  else if(!key_pressed && press9or0 == HIGH) {
    key_pressed = true;
    phoneNumber += "f";
  }
  else if (press1or2 == LOW && press3or4 == LOW && 
           press5or6 == LOW && press7or8 == LOW && 
           press9or0 == LOW) {
    Serial.print(phoneNumber + "\n");
    key_pressed = false;
  }

}
