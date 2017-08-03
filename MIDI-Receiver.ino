
const int nFreqs = 16;
uint16_t freqs[nFreqs] = {0};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(8, OUTPUT);
  pinMode(13, OUTPUT);
}

bool lit = true;

int chordingPeriod = 60; // ms

long unsigned int lastNoteT = 0;

int16_t pitchBend = 0;

// Organize and count the active tones.
uint16_t getCurFreq () {
  int nPlaying = 0;
  for (int i = 0; i < nFreqs; i++) {
    if (freqs[i]) {
      while (i > 0 && freqs[i-1] == 0) {
        freqs[i-1] = freqs[i];
        i--;
      }
      nPlaying ++;
      
    }
  }
  
  if (nPlaying == 0) return 0;
  //return freqs[((millis())/(chordingPeriod/nPlaying)) % nPlaying];
  return freqs[((millis()-lastNoteT)/(chordingPeriod/nPlaying)) % nPlaying] * pow(2.0,(pitchBend/8192.0));
  //return freqs[(millis()/(chordingPeriod)) % nPlaying];
}

void loop() {
  
  uint16_t curTone = getCurFreq();

  if (curTone) tone(8,curTone);
  else noTone(8);
  
  if (Serial.available() > 0) {
    char command = Serial.read();

    if (!(command & 0x80)) return; // Not a command byte, WTF. 

    //digitalWrite(13, lit);
    lit = !lit;
    
    delay(5); // Allow time for Serial to arrive.
    //Serial.write(command);
    //Serial.write(0x00);
    //Serial.write(0x7F);
    
    if ((command & 0xF0) == 0x90) {
      while (Serial.available() < 2) delayMicroseconds(100);
      digitalWrite(13, HIGH);
      // Note on, 2 data bytes.
      char key = Serial.read();
      char vel = Serial.read();
      
      uint16_t freq = pow(2,(key-0x45)/12.0)*440.0;
      
      for (int i = 0; i < nFreqs; i++) {
        if (freqs[i] == 0) {
          
          //freqs[i] = freqs[0];
          freqs[i] = freq;
          lastNoteT = millis();
          break;
        }
        
      }
      
    }

    // Note off, 2 data bytes
    if ((command&0xF0) == 0x80) {
      
      while (Serial.available() < 2) delayMicroseconds(100);
      
      digitalWrite(13, LOW);
      char key = Serial.read();
      char vel = Serial.read();

      uint16_t freq = pow(2,(key-0x45)/12.0)*440.0;

      for (int i = 0; i < nFreqs; i++) {
        if (freqs[i] == freq) {
          freqs[i] = 0;
        }
      }
      
    }

    // Aftertouch, 2 bytes
    if ((command&0xF0) == 0xA0) {
      char key = Serial.read();
      char touch = Serial.read();
    }

    // Continuous controller, 2 bytes
    if ((command&0xF0) == 0xB0) {
      char controllerN = Serial.read();
      char controlVal = Serial.read();

      if (controllerN == 0x01) {
        chordingPeriod = controlVal*2 + 1;
      }
      
    }

    // Patch change, 2 bytes
    if ((command&0xF0) == 0xC0) {
      char key = Serial.read(); // ?
      char vel = Serial.read(); // ?
    }
    
    if ((command&0xF0) == 0xD0) {
      char pressure = Serial.read();
    }

    // Pitch bend
    if ((command&0xF0) == 0xE0) {
      char pitchLSB = Serial.read();
      char pitchMSB = Serial.read();
      //pitchBend = pitchLSB + (pitchMSB<<7) - 8192; // Yes, that should be 7.
    }
    
    if ((command&0xF0) == 0xF0) {
      // Hopefully, nothing.
      while (Serial.available()) Serial.read();
    }
  }
}
