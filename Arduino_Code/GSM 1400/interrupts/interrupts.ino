

#include <Wire.h> // I2C

// For IR Remote
#include <Arduino.h>
//#define NO_LED_FEEDBACK_CODE // saves 92 bytes program memory
#if FLASHEND <= 0x1FFF  // For 8k flash or less, like ATtiny85. Exclude exotic protocols.
#define EXCLUDE_EXOTIC_PROTOCOLS
#  if !defined(DIGISTUMPCORE) // ATTinyCore is bigger than Digispark core
#define EXCLUDE_UNIVERSAL_PROTOCOLS // Saves up to 1000 bytes program memory.
#  endif
#endif
//#define DEBUG // Activate this for lots of lovely debug output from the decoders.
#define INFO // To see valuable informations from universal decoder for pulse width or pulse distance protocols
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp>
#if defined(APPLICATION_PIN)
#define DEBUG_BUTTON_PIN    APPLICATION_PIN // if low, print timing for each received data set
#else
#define DEBUG_BUTTON_PIN 6
#endif
#if defined(ARDUINO_ARCH_SAMD)
#define Serial SerialUSB
#endif

const byte IR_interrupt_pin = 1;
static char IR_array[200] = {"Nothing \n"};

// Function prototypes
void get_IR();
void IR_REMOTE_ISR();

// *** SETUP *********************************************************************************************************************************************************************************************

void setup() {

  pinMode(IR_interrupt_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(IR_interrupt_pin), IR_REMOTE_ISR, LOW);
  
  Serial.begin(9600); // Initialize the serial port.
  Wire.begin(); // Initialize I2C.

// *** REMOTE SETUP *************************************************************************************************************************************************************************************
  #if defined(_IR_MEASURE_TIMING) && defined(_IR_TIMING_TEST_PIN)
    pinMode(_IR_TIMING_TEST_PIN, OUTPUT);
  #endif
  #if FLASHEND >= 0x3FFF  // For 16k flash or more, like ATtiny1604. Code does not fit in program memory of ATtiny85 etc.
      pinMode(DEBUG_BUTTON_PIN, INPUT);
  #endif
  
      Serial.begin(115200);
  #if defined(__AVR_ATmega32U4__) || defined(SERIAL_PORT_USBVIRTUAL) || defined(SERIAL_USB) /*stm32duino*/|| defined(USBCON) /*STM32_stm32*/|| defined(SERIALUSB_PID) || defined(ARDUINO_attiny3217)
      delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
  #endif
  // Just to know which program is running on my Arduino
      Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));
  
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
      Serial.println(F("Enabling IRin..."));
  
      // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
      IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  
      Serial.print(F("Ready to receive IR signals of protocols: "));
      printActiveIRProtocols(&Serial);
      Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));
  
  #if FLASHEND >= 0x3FFF  // For 16k flash or more, like ATtiny1604. Code does not fit in program memory of ATtiny85 etc.
      Serial.print(F("Debug button pin is "));
      Serial.println(DEBUG_BUTTON_PIN);
  
      // infos for receive
      Serial.print(RECORD_GAP_MICROS);
      Serial.println(F(" us is the (minimum) gap, after which the start of a new IR packet is assumed"));
      Serial.print(MARK_EXCESS_MICROS);
      Serial.println(F(" us are subtracted from all marks and added to all spaces for decoding"));
  #endif
}

// *** MAIN FUNCTION *************************************************************************************************************************************************************************************

void loop() {
  Serial.print(IR_array);
  Serial.print("\n");
  }

void IR_REMOTE_ISR() {
  IrReceiver.printIRResultMinimal(&IR_array);
}

void get_IR() {
    /*
     * Check if received data is available and if yes, try to decode it.
     * Decoded result is in the IrReceiver.decodedIRData structure.
     *
     * E.g. command is in IrReceiver.decodedIRData.command
     * address is in command is in IrReceiver.decodedIRData.address
     * and up to 32 bit raw data in IrReceiver.decodedIRData.decodedRawData
     */
    if (IrReceiver.decode()) {
        Serial.println();
#if FLASHEND >= 0x3FFF  // For 16k flash or more, like ATtiny1604
        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW) {
            Serial.println(F("Overflow detected"));
            Serial.println(F("Try to increase the \"RAW_BUFFER_LENGTH\" value of " STR(RAW_BUFFER_LENGTH) " in " __FILE__));
            // see also https://github.com/Arduino-IRremote/Arduino-IRremote#modifying-compile-options-with-sloeber-ide
#  if !defined(ESP8266) && !defined(NRF5)
            /*
             * do double beep
             */
#    if !defined(ESP32)
            IrReceiver.stop(); // ESP32 uses another timer for tone()
#    endif
            tone(TONE_PIN, 1100, 10);
            delay(50);
            tone(TONE_PIN, 1100, 10);
            delay(50);
#    if !defined(ESP32)
            IrReceiver.start(100000); // to compensate for 100 ms stop of receiver. This enables a correct gap measurement.
#    endif
#  endif

        } else {
            // Print a short summary of received data
            IrReceiver.printIRResultShort(&Serial);

            if (IrReceiver.decodedIRData.protocol == UNKNOWN || digitalRead(DEBUG_BUTTON_PIN) == LOW) {
                // We have an unknown protocol, print more info
                IrReceiver.printIRResultRawFormatted(&Serial, true);
            }
        }

        // tone on esp8266 works once, then it disables the successful IrReceiver.start() / timerConfigForReceive().
#  if !defined(ESP8266) && !defined(NRF5)
        if (IrReceiver.decodedIRData.protocol != UNKNOWN) {
            /*
             * If a valid protocol was received, play tone, wait and restore IR timer.
             * Otherwise do not play a tone to get exact gap time between transmissions.
             * This will give the next CheckForRecordGapsMicros() call a chance to eventually propose a change of the current RECORD_GAP_MICROS value.
             */
#    if !defined(ESP32)
            IrReceiver.stop(); // ESP32 uses another timer for tone()
#    endif
            tone(TONE_PIN, 2200, 8);
#    if !defined(ESP32)
            delay(8);
            IrReceiver.start(8000); // to compensate for 8 ms stop of receiver. This enables a correct gap measurement.
#    endif
        }
#  endif
#else
        // Print a minimal summary of received data
        IrReceiver.printIRResultMinimal(&Serial);
#endif // FLASHEND

        /*
         * !!!Important!!! Enable receiving of the next value,
         * since receiving has stopped after the end of the current received data packet.
         */
        IrReceiver.resume();

        /*
         * Finally check the received data and perform actions according to the received address and commands
         */
        if (IrReceiver.decodedIRData.address == 0) {
            if (IrReceiver.decodedIRData.command == 0x10) {
                // do something
            } else if (IrReceiver.decodedIRData.command == 0x11) {
                // do something else
            }
        }
    } // if (IrReceiver.decode())

    /*
     * Your code here
     * For all users of the FastLed library, use this code for strip.show() to improve receiving performance (which is still not 100%):
     * if (IrReceiver.isIdle()) {
     *     strip.show();
     * }
     */

}
