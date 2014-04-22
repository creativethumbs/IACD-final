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
int hookswitch = 14; //analog pin A0
int key_1_or_2 = 15; //analog pin A1
int key_3_or_4 = 2;
int key_5_or_6 = 5;
int key_7_or_8 = 6;
int key_9_or_0 = 7;
bool rang = false;

bool key_pressed = false;
bool playing_audio = false;

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(RESET, CS, DCS, DREQ, CARDCS);

String phoneNumber = "";

void setup() {
  Serial.begin(9600);
  SD.begin(CARDCS);    // initialise the SD card
  
  pinMode(hookswitch, INPUT);
  pinMode(key_1_or_2, INPUT);
  pinMode(key_3_or_4, INPUT);
  pinMode(key_5_or_6, INPUT);
  pinMode(key_7_or_8, INPUT);
  pinMode(key_9_or_0, INPUT);
  
  musicPlayer.setVolume(100,100);
  musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
  
}

void loop() {
  int presshook = digitalRead(hookswitch);
  int press1or2 = digitalRead(key_1_or_2);
  int press3or4 = digitalRead(key_3_or_4);
  int press5or6 = digitalRead(key_5_or_6);
  int press7or8 = digitalRead(key_7_or_8);
  int press9or0 = digitalRead(key_9_or_0);
  if(presshook == HIGH) { //if the hook is down
    if (!musicPlayer.stopped())
      musicPlayer.stopPlaying();
    key_pressed = false;
    playing_audio = false;
    phoneNumber = "";
  }
  
  Serial.print(phoneNumber + "\n");

  //check phone string for the combinations
  
  if(!playing_audio && phoneNumber == "ebcccdfeed") { //my phone number
    beginRinging();
    musicPlayer.startPlayingFile("track002.mp3");
  } 
  else if(!playing_audio && phoneNumber == "eddedefefe") { //tekkoshocon
  //866-767-9708
    beginRinging();
    musicPlayer.startPlayingFile("rave.mp3");
    
  } 
  
  else if(!playing_audio && phoneNumber == "cbbbdebbcf") { //dining services
  //(412) 268-2139
    beginRinging();
    musicPlayer.startPlayingFile("lunch.mp3");
  } 

  else if(!playing_audio && phoneNumber == "febcbcebbe") { //auntie's number
  //082-324-8228
    beginRinging();
    musicPlayer.startPlayingFile("play.mp3");
  } 
  
  else if(!playing_audio && phoneNumber == "febecdedce") { //temple***
  //082-836-7537
    beginRinging();
    musicPlayer.startPlayingFile("monks.mp3");
  } 
  
  else if(!playing_audio && phoneNumber == "febebddcdc") { //talking to ploy***
  //082-825-5464
    beginRinging();
    musicPlayer.startPlayingFile("ploy.mp3");
  } 
  
  else if(!playing_audio && phoneNumber == "febefbbcbd") { //waiting for prachanth??
  //082-702-2426
    beginRinging();
    musicPlayer.startPlayingFile("waiting.mp3");
  } 


  //otherwise continue to append to the string
  else if(!key_pressed && press1or2 == HIGH) {
    key_pressed = true;
    phoneNumber += "b";
  }
  else if(!key_pressed && press3or4 == HIGH) {
    key_pressed = true;
    phoneNumber += "c";
  }
  else if(!key_pressed && press5or6 == HIGH) {
    key_pressed = true;
    phoneNumber += "d";
  }
  
  else if(!key_pressed && press7or8 == HIGH) { 
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


void beginRinging() {
  playing_audio = true;
  musicPlayer.begin();
  SD.begin(CARDCS); 
  musicPlayer.playFullFile("ringing.mp3");
}

