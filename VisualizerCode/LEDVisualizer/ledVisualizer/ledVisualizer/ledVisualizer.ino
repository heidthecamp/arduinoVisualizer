/*
    A simple 1-d oscilliscope: scan all the channels, setting the PWM output
    value to 4x the analog pin 0 value (0 - 1024 * 4 = 4096).  The value will
    fade to zero as the channels keep scanning.

    See the BasicUse example for hardware setup.

    Alex Leone <acleone ~AT~ gmail.com>, 2009-02-03 */
    
/*
    MODIFIED BY: hiedthecamp
    PURPOSE:
        This program reads in a mono audio input and visualizes it through TLC5940NTs
        
    LAST MODIFIED: 2014 APR 08
*/

/*
                      **********BOARD LAYOUT***********
            
            
                    ARDUINO-UNO                   TLC5940NT
        
        
         |     13|-> SCLK (pin 25)           OUT1 |1     28| OUT channel 0
         |     12|                           OUT2 |2     27|-> GND (VPRG)
         |     11|-> SIN (pin 26)            OUT3 |3     26|-> SIN (pin 11)
         |+3   10|-> BLANK (pin 23)          OUT4 |4     25|-> SCLK (pin 13)
  POW_OUT|+5    9|-> XLAT (pin 24)             .  |5     24|-> XLAT (pin 9)
 POW_GRND|-     8|                             .  |6     23|-> BLANK (pin 10)
AUDIOGRND|-     7|                             .  |7     22|-> GND
         |      6|                             .  |8     21|-> VCC (+5V)
                5|                             .  |9     20|-> 2K Resistor -> GND
 AUDIO_IN|A0    4|                             .  |10    19|-> +5V (DCPRG)
         |A1    3|-> GSCLK (pin 18)            .  |11    18|-> GSCLK (pin 3)
         |A2    2|                             .  |12    17|-> SOUT (CONNECT TO SIN OF NEXT BOARD)
         |A3    1|                             .  |13    16|-> XERR
         |A4    0|                           OUT14|14    15| OUT channel 15
        

*/

#include "Tlc5940.h"
#include "tlc_fades.h"


// which analog pin to use
#define AUDIO_IN      0 // audio input 
#define ANALOG_PIN    1 // used to stabilize audio input

// how many millis to strobe over all the LEDs
#define SCOPE_PERIOD    (250 * NUM_TLCS)
#define LED_PERIOD      SCOPE_PERIOD / (NUM_TLCS * 16)

// *MODIFY NUM_TLCS IN tlc_config.h

const int NUM_CHANNELS = 4;
TLC_CHANNEL_TYPE channel[NUM_CHANNELS];

int INTENSITY =  1;

void setup()
{
  Tlc.init();
}

void loop()
{
  uint32_t lastMillis = millis();
  int startPoint;
  int endPoint;
 
 for (int i = 0; i < NUM_CHANNELS; i++){

    tlc_addFade(channel[i],                      // led channel
                (analogRead(AUDIO_IN) * analogRead(AUDIO_IN) / (analogRead(ANALOG_PIN) + 1000 )) * INTENSITY,   // start fade value (0-4095)
                // Line above stabelizes the input
                0,                            // end fade value (0-4095)
                lastMillis + 2,               // start millis
                lastMillis + (uint16_t)SCOPE_PERIOD / 4  // end millis
               );
    
    startPoint = ((NUM_TLCS * 16) / NUM_CHANNELS) * i;
    endPoint = ((NUM_TLCS * 16) / NUM_CHANNELS) * (i + 1);
    
    if (channel[i]++ >= endPoint) {
      channel[i] = startPoint;
    }
  }
  
  uint32_t currentMillis;
  do {
    currentMillis = millis();
    tlc_updateFades(currentMillis);
  } while (currentMillis - lastMillis <= LED_PERIOD);
}

