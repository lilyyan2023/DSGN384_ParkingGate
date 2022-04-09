

#include <Wire.h> // I2C
#include <QMC5883LCompass.h> // Compass library
#include "ds3231.h" // RTC Library
#include <Keypad.h> // Keypad library
#define BUFF_MAX 128 // RTC
#define ledpin 13 // Keypad

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
#define DEBUG_BUTTON_PIN   6
#endif
#if defined(ARDUINO_ARCH_SAMD)
#define Serial SerialUSB
#endif

// RTC Variables
uint8_t time[8];
char recv[BUFF_MAX];
unsigned int recv_size = 0;
unsigned long prev, interval = 300;

// Compass Variables
QMC5883LCompass compass;
volatile int x, y, z;

// Keypad Variables
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
// Define the Keymap
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'#','0','*'}
};
byte rowPins[ROWS] = { 0, 1, 3, 4 }; // row pins
byte colPins[COLS] = { 5, 6, 7}; // column pins
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

// Function prototypes
void parse_cmd(char *cmd, int cmdsize);
void print_time();
void magnetometer();
void membrane_keypad();
void get_IR();

// *** SETUP *********************************************************************************************************************************************************************************************

void setup() {
  
  Serial.begin(9600); // Initialize the serial port.
  Wire.begin(); // Initialize I2C.
  
  compass.init(); // Initialize the Compass.
  
  DS3231_init(DS3231_CONTROL_INTCN); // Initialize RTC
  memset(recv, 0, BUFF_MAX);
  
  Serial.println("GET time");

// *** REMOTE SETUP *************************************************************************************************************************************************************************************
  #if defined(_IR_MEASURE_TIMING) && defined(_IR_TIMING_TEST_PIN)
    pinMode(_IR_TIMING_TEST_PIN, OUTPUT);
  #endif
  #if FLASHEND >= 0x3FFF  // For 16k flash or more, like ATtiny1604. Code does not fit in program memory of ATtiny85 etc.
      pinMode(DEBUG_BUTTON_PIN, INPUT_PULLUP);
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

  // read the sensor:

  if (Serial.available() > 0) {

    int mode = Serial.read();
    switch (mode) {
      case 'k':
        Serial.print("Entering KEYPAD mode\n");
        
        while(Serial.read() != 'x') { membrane_keypad(); }
        break;

      case 'r':
        Serial.print("Entering REMOTE mode\n");

        while(Serial.read() != 'x') { get_IR(); }
        break;

      case 'm':
        Serial.print("Entering MAGNETOMETER mode\n");

        while(Serial.read() != 'x') { magnetometer(); }
        break;

      case 'c':
        Serial.print("Entering CLOCK mode\n");

        while(Serial.read() != 'x') { print_time(); }
        break;
        
      default:
        Serial.print("Entering DEFAULT mode\n");
        break;

    }
  }
}

// *** MAGNETOMETER ***************************************************************************************************************************************************************************************

void magnetometer() {
  // Read compass values
  compass.read();

  x = compass.getX();
  y = compass.getY();
  z = compass.getZ();

  Serial.print("X: ");
  Serial.print(x);
  Serial.print("   Y: ");
  Serial.print(y);
  Serial.print("   Z: ");
  Serial.println(z);

  delay(300);
}

// *** RTC PRINT FUNCTION ******************************************************************************************************************************************************************************

void print_time() {

  // Setup RTC variables
    char in;
    char buff[BUFF_MAX];
    unsigned long now = millis();
    struct ts t;
    
  // show time once in a while
    if ((now - prev > interval) && (Serial.available() <= 0)) {
        DS3231_get(&t);

        // there is a compile time option in the library to include unixtime support
    #ifdef CONFIG_UNIXTIME
    #ifdef __AVR__
            snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d %ld", t.year,
    #error AVR
    #else
            snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d %d", t.year,
    #endif
                 t.mon, t.mday, t.hour, t.min, t.sec, t.unixtime);
    #else
            snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d", t.year,
                 t.mon, t.mday, t.hour, t.min, t.sec);
    #endif

        Serial.println(buff);
        prev = now;
    }

    if (Serial.available() > 0) {
        in = Serial.read();

        if ((in == 10 || in == 13) && (recv_size > 0)) {
            parse_cmd(recv, recv_size);
            recv_size = 0;
            recv[0] = 0;
        } else if (in < 48 || in > 122) {;       // ignore ~[0-9A-Za-z]
        } else if (recv_size > BUFF_MAX - 2) {   // drop lines that are too long
            // drop
            recv_size = 0;
            recv[0] = 0;
        } else if (recv_size < BUFF_MAX - 2) {
            recv[recv_size] = in;
            recv[recv_size + 1] = 0;
            recv_size += 1;
        }

    }
  
}

// *** RTC PARSE FUNCTION ******************************************************************************************************************************************************************************

void parse_cmd(char *cmd, int cmdsize) {
  
    uint8_t i;
    uint8_t reg_val;
    char buff[BUFF_MAX];
    struct ts t;

    //snprintf(buff, BUFF_MAX, "cmd was '%s' %d\n", cmd, cmdsize);
    //Serial.print(buff);

    // TssmmhhWDDMMYYYY aka set time
    if (cmd[0] == 84 && cmdsize == 16) {
        //T355720619112011
        t.sec = inp2toi(cmd, 1);
        t.min = inp2toi(cmd, 3);
        t.hour = inp2toi(cmd, 5);
        t.wday = cmd[7] - 48;
        t.mday = inp2toi(cmd, 8);
        t.mon = inp2toi(cmd, 10);
        t.year = inp2toi(cmd, 12) * 100 + inp2toi(cmd, 14);
        DS3231_set(t);
        Serial.println("OK");
    } else if (cmd[0] == 49 && cmdsize == 1) {  // "1" get alarm 1
        DS3231_get_a1(&buff[0], 59);
        Serial.println(buff);
    } else if (cmd[0] == 50 && cmdsize == 1) {  // "2" get alarm 1
        DS3231_get_a2(&buff[0], 59);
        Serial.println(buff);
    } else if (cmd[0] == 51 && cmdsize == 1) {  // "3" get aging register
        Serial.print("aging reg is ");
        Serial.println(DS3231_get_aging(), DEC);
    } else if (cmd[0] == 65 && cmdsize == 9) {  // "A" set alarm 1
        DS3231_set_creg(DS3231_CONTROL_INTCN | DS3231_CONTROL_A1IE);
        //ASSMMHHDD
        for (i = 0; i < 4; i++) {
            time[i] = (cmd[2 * i + 1] - 48) * 10 + cmd[2 * i + 2] - 48; // ss, mm, hh, dd
        }
        uint8_t flags[5] = { 0, 0, 0, 0, 0 };
        DS3231_set_a1(time[0], time[1], time[2], time[3], flags);
        DS3231_get_a1(&buff[0], 59);
        Serial.println(buff);
    } else if (cmd[0] == 66 && cmdsize == 7) {  // "B" Set Alarm 2
        DS3231_set_creg(DS3231_CONTROL_INTCN | DS3231_CONTROL_A2IE);
        //BMMHHDD
        for (i = 0; i < 4; i++) {
            time[i] = (cmd[2 * i + 1] - 48) * 10 + cmd[2 * i + 2] - 48; // mm, hh, dd
        }
        uint8_t flags[5] = { 0, 0, 0, 0 };
        DS3231_set_a2(time[0], time[1], time[2], flags);
        DS3231_get_a2(&buff[0], 59);
        Serial.println(buff);
    } else if (cmd[0] == 67 && cmdsize == 1) {  // "C" - get temperature register
        Serial.print("temperature reg is ");
        Serial.println(DS3231_get_treg(), DEC);
    } else if (cmd[0] == 68 && cmdsize == 1) {  // "D" - reset status register alarm flags
        reg_val = DS3231_get_sreg();
        reg_val &= B11111100;
        DS3231_set_sreg(reg_val);
    } else if (cmd[0] == 70 && cmdsize == 1) {  // "F" - custom fct
        reg_val = DS3231_get_addr(0x5);
        Serial.print("orig ");
        Serial.print(reg_val,DEC);
        Serial.print("month is ");
        Serial.println(bcdtodec(reg_val & 0x1F),DEC);
    } else if (cmd[0] == 71 && cmdsize == 1) {  // "G" - set aging status register
        DS3231_set_aging(0);
    } else if (cmd[0] == 83 && cmdsize == 1) {  // "S" - get status register
        Serial.print("status reg is ");
        Serial.println(DS3231_get_sreg(), DEC);
    } else {
        Serial.print("unknown command prefix ");
        Serial.println(cmd[0]);
        Serial.println(cmd[0], DEC);
    }
}

// *** MEMBRANE KEYPAD ******************************************************************************************************************************************************************************

void membrane_keypad()
{
  char key = kpd.getKey();
  if(key)  // Check for a valid key.
  {
    switch (key)
    {
      case '*':
        digitalWrite(ledpin, LOW);
        break;
      case '#':
        digitalWrite(ledpin, HIGH);
        break;
      default:
        Serial.println(key);
    }
  }
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
