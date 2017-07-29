void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(8, OUTPUT);
  pinMode(13, OUTPUT);
}

char curKey = 0;
bool sustain = false;

bool lit = true;

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0) {
    char command = Serial.read();

    if (!(command & 0x80)) return; // Not a command byte, WTF. 

    //digitalWrite(13, lit);
    lit = !lit;
    
    delay(5); // Allow time for Serial to arrive.
    Serial.write(command);
    Serial.write(0x00);
    Serial.write(0x7F);
    
    if ((command & 0xF0) == 0x90) {
      while (Serial.available() < 2) delayMicroseconds(100);
      digitalWrite(13, HIGH);
      // Note on, 2 data bytes.
      char key = Serial.read();
      char vel = Serial.read();
      
      uint16_t freq = pow(2,(key-0x45)/12.0)*440.0;
      
      curKey = key;
      
      tone(8,freq);
    }

    // Note off, 2 data bytes
    if ((command&0xF0) == 0x80) {
      while (Serial.available() < 2) delayMicroseconds(100);
      digitalWrite(13, LOW);
      char key = Serial.read();
      char vel = Serial.read();

      if (key == curKey) noTone(8);
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
    }
    
    if ((command&0xF0) == 0xF0) {
      // Hopefully, nothing.
      while (Serial.available()) Serial.read();
    }
  }
}
