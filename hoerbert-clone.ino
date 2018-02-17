#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <i2s.h>
#include <i2s_reg.h>

// #include <FS.h>
int16_t buffer[512]; 

#include <SD.h>

void setup(void) {
    Serial.begin(9600);
    Serial.println("");
    Serial.println("putting WiFi to sleep");
    WiFi.forceSleepBegin(); // turn off ESP8266 RF
    delay(1); // give RF section time to shutdown

    SD.begin(D0);
    
    Serial.println("Setup finished");
}

int rate = 32000;

File f;
bool is_playing = false;

void start_playing() {
    Serial.println(F("Playing hill32.wav"));
    f = SD.open("/hill32.wav");
    if (!f) {
        Serial.println("file open failed");
        return;
    } //- See more at: http://www.esp8266.com/viewtopic.php?f=29&t=8194#sthash.u5P6kDr6.ycI23aTr.dpuf
    
    i2s_begin();
    i2s_set_rate(rate);    
}

bool continue_playing() {
      if (f.position() < (f.size()-1)) {
        int numBytes = _min(sizeof(buffer), f.size() - f.position() - 1);
        f.readBytes((char*)buffer, numBytes);
        for (int i = 0; i < numBytes / 2; i++) {
            int sample = buffer[i] & 0xffff;
            sample = sample << 16;
            sample |= buffer[i] & 0xffff;
            
            i2s_write_sample(sample);
        }
        return true;
    }
    return false;
}

void stop_playing()
{
    f.close();
    i2s_end();
}

int read_button() {
  int a = analogRead(A0);
  static int thresh[] = { 0, 120, 200, 325, 460, 585, 695, 800, 890, 1024 }; 
  int btnState = 0;
  for (int i = 0; i < 9; ++i) {
    if (thresh[i] <= a && a < thresh[i+1]) {
      btnState = i + 1;
      break;
    }
  }
  return btnState;
}

void loop() {


  if (is_playing) {
    if (!continue_playing()) {
      stop_playing();
      is_playing = false;
    }
  }

  static int lastT = millis();
  int now = millis();
  static int cur_btn = 0;
  if (now - lastT > 100) {
    int new_btn = read_button();
    if (cur_btn != new_btn) {
      cur_btn = new_btn;
      Serial.println(cur_btn);
      if (!is_playing && cur_btn > 0 && cur_btn < 9) {
        start_playing();
        is_playing = true; 
      }
      else if (is_playing && cur_btn == 9) {
        stop_playing();
        is_playing = false;
      }
    }
    lastT = now;
  }
}
