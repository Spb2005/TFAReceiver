#ifndef TFA_RECEIVER_H
#define TFA_RECEIVER_H

#include <Arduino.h>

struct TFAResult {
  byte type;
  byte id;
  bool battery;
  byte channel;
  float temperature;
  byte humidity;
};


class TFAReceiver {
  public:
    TFAReceiver(uint8_t pin);
    void begin();
    void checkBuf();
    bool isAvailable();
    TFAResult getResult();
    void setTemperatureUnit(bool useCelsius);

  private:
    static void handleInterrupt();
    static void resetState();
    static void addBit(uint8_t bitData);
    static byte lfsr_digest8(const byte message[], unsigned n, byte gen, byte key);
    static TFAResult parseResult();
    static int binToDec(int s, int e);
    static bool getBit(int k);
    static bool isRepeat(TFAResult r);

    static uint8_t _pin;
    static bool _useCelsius;  

    static volatile byte buff[6];
    static volatile byte tempBit;
    static volatile boolean firstZero;
    static volatile byte headerHits;
    static volatile byte discardHits;
    static volatile byte dataByte;
    static volatile byte nosBits;
    static volatile byte nosBytes;
    static volatile bool checkBufFlag;
    static bool avail;
    static unsigned long lastResultTime;
    static TFAResult result;
};

#endif
