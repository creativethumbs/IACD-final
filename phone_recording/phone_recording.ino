// adapted from Adafruit code


// include SPI, MP3 and SD libraries
#include <SPI.h>
#include <Adafruit_VS1053.h>
#include <SD.h>

// define the pins used
#define RESET 9      // VS1053 reset pin (output)
#define CS 10        // VS1053 chip select pin (output)
#define DCS 8        // VS1053 Data/command select pin (output)
#define CARDCS A0     // Card chip select pin
#define DREQ A1       // VS1053 Data request, ideally an Interrupt pin

Adafruit_VS1053_FilePlayer musicPlayer = Adafruit_VS1053_FilePlayer(RESET, CS, DCS, DREQ, CARDCS);

File recording;  // the file we will save our recording to
#define RECBUFFSIZE 128  // 64 or 128 bytes.
uint8_t recording_buffer[RECBUFFSIZE];

int hookswitch = 7;

float startRec;
float timeNow;

int current = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("Adafruit VS1053 Ogg Recording Test");

  // initialise the music player
  if (!musicPlayer.begin()) {
    Serial.println("VS1053 not found");
    while (1);  // don't do anything more
  }

  SD.begin(CARDCS);
  
  // Set volume for left, right channels. lower numbers == louder volume!
  musicPlayer.setVolume(10,10);
  
  pinMode(hookswitch, INPUT);
  
  // load plugin from SD card! We'll use mono 44.1KHz, high quality
  if (! musicPlayer.prepareRecordOgg("v44k1q05.img")) {
     Serial.println("Couldn't load plugin!");
     while (1);    
  }
}

uint8_t isRecording = false;

void loop() {  
  int presshook = digitalRead(hookswitch);
  
  if(presshook != HIGH) { //if the hook is not down
    Serial.println("off hook\n");
    if (!isRecording) {
      isRecording = true;
      delay(1000); //give the user some time to put the phone against their ear
      
      char filename[15];
      strcpy(filename, "RECORD00.OGG");
      for (uint8_t i = 0; i < 100; i++) {
        filename[6] = '0' + i/10;
        filename[7] = '0' + i%10;
        
        // create if does not exist, do not open existing, write, sync after write
        if (! SD.exists(filename)) {
          current = i; 
          Serial.print(current + "\n");
          break;
        }
      }
      
      if (current > 0) {
        beginPlayback();
      }
      
      recording = SD.open(filename, FILE_WRITE);
      
      if (! recording) {
         Serial.println("Couldn't open file to record!");
         while (1);
      }
      beginBeep();
      musicPlayer.startRecordOgg(true); // use microphone (for linein, pass in 'false')
    }
    
    else {
      saveRecordedData(isRecording);
    }
      
  }
  
  else if (isRecording) {
      Serial.println("End recording");
      musicPlayer.stopRecordOgg();
      isRecording = false;
      // flush all the data!
      saveRecordedData(isRecording);
      // close it up
      recording.close();
      delay(1000);
  }
}

uint16_t saveRecordedData(boolean isrecord) {
  uint16_t written = 0;
  
    // read how many words are waiting for us
  uint16_t wordswaiting = musicPlayer.recordedWordsWaiting();
  
  // try to process 256 words (512 bytes) at a time, for best speed
  while (wordswaiting > 256) {
    for (int x=0; x < 512/RECBUFFSIZE; x++) {
      // fill the buffer!
      for (uint16_t addr=0; addr < RECBUFFSIZE; addr+=2) {
        uint16_t t = musicPlayer.recordedReadWord();
        //Serial.println(t, HEX);
        recording_buffer[addr] = t >> 8; 
        recording_buffer[addr+1] = t;
      }
      if (! recording.write(recording_buffer, RECBUFFSIZE)) {
            Serial.print("Couldn't write "); Serial.println(RECBUFFSIZE); 
            while (1);
      }
    }
    // flush 512 bytes at a time
    recording.flush();
    written += 256;
    wordswaiting -= 256;
  }
  
  wordswaiting = musicPlayer.recordedWordsWaiting();
  if (!isrecord) {
    Serial.print(wordswaiting); Serial.println(" remaining");
    // wrapping up the recording!
    uint16_t addr = 0;
    for (int x=0; x < wordswaiting-1; x++) {
      // fill the buffer!
      uint16_t t = musicPlayer.recordedReadWord();
      recording_buffer[addr] = t >> 8; 
      recording_buffer[addr+1] = t;
      if (addr > RECBUFFSIZE) {
          if (! recording.write(recording_buffer, RECBUFFSIZE)) {
                Serial.println("Couldn't write!");
                while (1);
          }
          recording.flush();
          addr = 0;
      }
    }
    if (addr != 0) {
      if (!recording.write(recording_buffer, addr)) {
        Serial.println("Couldn't write!"); while (1);
      }
      written += addr;
    }
    musicPlayer.sciRead(VS1053_SCI_AICTRL3);
    if (! (musicPlayer.sciRead(VS1053_SCI_AICTRL3) & _BV(2))) {
       recording.write(musicPlayer.recordedReadWord() & 0xFF);
       written++;
    }
    recording.flush();
  }

  return written;
}

void beginPlayback() {
  char message[15];
  strcpy(message, "RECORD00.OGG");
  
  int previous = current - 1;
  
  message[6] = '0' + previous/10;
  message[7] = '0' + previous%10;
  
  musicPlayer.begin();
  SD.begin(CARDCS);
  
  musicPlayer.playFullFile(message); 
  
}

void beginBeep() {
  musicPlayer.begin();
  SD.begin(CARDCS);
  
  musicPlayer.playFullFile("beep.wav"); 
  musicPlayer.prepareRecordOgg("v44k1q05.img");
}

