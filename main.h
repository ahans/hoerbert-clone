#pragma once

#include "wavreader.h"

#include <i2s.h>
#include <i2s_reg.h>

namespace de { namespace ahans {

class Main
{
public:
    Main()
        : isPlaying_(false)
        , curPlaylist_(0)
        , rate_(32000)

    {
        for (auto& p : position_) p = 0;
    }
    
    void loop()
    {
        if (isPlaying_)
        {
            if (!continuePlaying())
            {
                stopPlaying();
                startPlaying(curPlaylist_);
                isPlaying_ = false;
            }
        }
        else
        {
            curPlaylist_ = 1;
            startPlaying(curPlaylist_);
        }

        static int lastT = millis();
        int now = millis();
        static int curBtn = 0;
        if (now - lastT > 100)
        {
            int newBtn = readButton();
            if (curBtn != newBtn)
            {
                curBtn = newBtn;
                Serial.println(curBtn);
                
                if (curBtn > 0)
                {
                    if (isPlaying_) stopPlaying();
                    curPlaylist_ = curBtn;
                    startPlaying(curBtn);
                }
            }
            lastT = now;
        }
    }

private:
    bool startPlaying(int playlist)
    {
        // generate next filename
        char buf[10];
        ++position_[playlist];
        sprintf(buf, "/%d/%02d.wav", playlist, position_[playlist]);
        
        // open file
        Serial.print("Trying to open ");
        Serial.println(buf);
        if (!wavReader_.open(buf))
        {
            Serial.println("file open failed, playlist over?");
            position_[playlist] = 0;
            return false;   
        }
        
        // setup I2S
        i2s_begin();
        i2s_set_rate(rate_);

        isPlaying_ = true;
        return true;
    }

    bool continuePlaying()
    {
        size_t remainingBytes = wavReader_.numRemainingBytes();
        if (remainingBytes > 0)
        {
            int numBytes = _min(sizeof(buffer_), remainingBytes);
            numBytes = wavReader_.read(reinterpret_cast<char*>(buffer_), numBytes);
            for (int i = 0; i < numBytes / 2; i++)
            {    
                int sample = buffer_[i] & 0xffff;
                sample = sample << 16;
                sample |= buffer_[i] & 0xffff;

                i2s_write_sample(sample);
            }
            return true;
        }
        return false;
    }

    void stopPlaying()
    {
      wavReader_.close();
      i2s_end();
    }
    
    int readButton()
    {
        int a = analogRead(A0);
        //   15   150   260   379   519   627   735   840
        // 0   100   200   320   450   575   680    780    1024
        static int thresh[] = { 0, 100, 200, 320, 450, 575, 680, 780, 1024 };
        int btnState = 0;
        for (int i = 0; i < 8; ++i)
        {
            if (thresh[i] <= a && a < thresh[i + 1])
            {
                btnState = i + 1;
                break;
            }
        }
        return btnState;
    }

private:
    WavReader wavReader_;
    int16_t buffer_[512];
    int16_t position_[8];
    uint8_t curPlaylist_;
    bool isPlaying_;
    int rate_;
};

} } // namespaces