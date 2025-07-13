#include "TFAReceiver.h"

#define BUFF_SIZE 6
#define DELAY_US 250
#define HEADER_BITS 12
#define DISCARD_BITS 2
#define POLARITY 1

uint8_t TFAReceiver::_pin;
bool TFAReceiver::_useCelsius = true;

volatile byte TFAReceiver::buff[6];
volatile byte TFAReceiver::tempBit;
volatile boolean TFAReceiver::firstZero;
volatile byte TFAReceiver::headerHits;
volatile byte TFAReceiver::discardHits;
volatile byte TFAReceiver::dataByte;
volatile byte TFAReceiver::nosBits;
volatile byte TFAReceiver::nosBytes;
volatile bool TFAReceiver::checkBufFlag;
bool TFAReceiver::avail = false;
unsigned long TFAReceiver::lastResultTime = 0;
TFAResult TFAReceiver::result;

TFAReceiver::TFAReceiver(uint8_t pin) {
  _pin = pin;
}

void TFAReceiver::begin() {
  pinMode(_pin, INPUT);
  resetState();
  attachInterrupt(digitalPinToInterrupt(_pin), handleInterrupt, CHANGE);
}

void TFAReceiver::checkBuf() { //Checking buffer, if Checksum matches
  if (checkBufFlag) {
    byte expected = buff[BUFF_SIZE - 1];
    byte calculated = lfsr_digest8((const byte*)buff, BUFF_SIZE - 1, 0x98, 0x3e) ^ 0x64;

    if (expected == calculated) {
      TFAResult newResult = parseResult();
      if (!isRepeat(newResult)) {
        result = newResult;
        avail = true;
        lastResultTime = millis();
      }
    }
    resetState();
    checkBufFlag = false;
  }
}

bool TFAReceiver::isAvailable() {
  return avail;
}

TFAResult TFAReceiver::getResult() {
  avail = false;
  return result;
}

void TFAReceiver::setTemperatureUnit(bool useCelsius) { //Change the temp unit: 1 = Celsius; 0 = Fahrenheit
  _useCelsius = useCelsius;
}

void TFAReceiver::resetState() {
  tempBit = POLARITY ^ 1;
  firstZero = false;
  headerHits = 0;
  discardHits = 0;
  dataByte = 0;
  nosBits = 0;
  nosBytes = 0;

  for (int i = 0; i < BUFF_SIZE; i++) buff[i] = 0;
}

void TFAReceiver::handleInterrupt() { //Collecting all Bytes
  if (!checkBufFlag) { //If previous buffer wasnt check, skip the data collection
    if (nosBytes >= BUFF_SIZE) { // If buffer is full, check if buffer is valid
      checkBufFlag = true;
      return;
    }

    if (digitalRead(_pin) != tempBit) return;

    delayMicroseconds(DELAY_US);
    if (digitalRead(_pin) != tempBit) {
      resetState();
      return;
    }

    byte bitState = tempBit ^ POLARITY;
    delayMicroseconds(2 * DELAY_US);
    if (digitalRead(_pin) == tempBit) tempBit ^= 1;

    if (bitState == 1) {
      if (!firstZero) {
        headerHits++;
        return;
      }
      addBit(bitState);
      return;
    }

    if (headerHits < HEADER_BITS) {
      resetState();
      return;
    }

    if (!firstZero && headerHits >= HEADER_BITS) {
      firstZero = true;
    }

    addBit(bitState); //Add next bit
  }
}

void TFAReceiver::addBit(byte bitData) {
  if (discardHits < DISCARD_BITS) {
    discardHits++;
    return;
  }

  dataByte = (dataByte << 1) | bitData;
  nosBits++;

  if (nosBits == 8) {
    nosBits = 0;
    buff[nosBytes++] = dataByte;
  }
}

byte TFAReceiver::lfsr_digest8(const byte message[], unsigned n, byte gen, byte key) { //calculate checksum
  byte sum = 0;
  for (unsigned k = 0; k < n; ++k) {
    byte data = message[k];
    for (int i = 7; i >= 0; --i) {
      if ((data >> i) & 1) sum ^= key;
      key = (key >> 1) ^ (key & 1 ? gen : 0);
    }
  }
  return sum;
}

TFAResult TFAReceiver::parseResult() {
  TFAResult r;
  r.type = binToDec(0, 7);
  r.id = binToDec(8, 15);
  r.battery = binToDec(16, 16) != 1;
  r.channel = binToDec(17, 19) + 1;
  float tempf = binToDec(20, 31) / 10.0 - 40.0;
  r.temperature = (_useCelsius) ? ((tempf-32) * 5.0/ 9.0 ) :tempf ;
  r.humidity = binToDec(32, 39);
  return r;
}

int TFAReceiver::binToDec(int s, int e) {
  int result = 0;
  unsigned int mask = 1;
  for (; e > 0 && s <= e; mask <<= 1) {
    if (getBit(e--)) result |= mask;
  }
  return result;
}

bool TFAReceiver::getBit(int k) {
  int i = k / 8;
  int pos = k % 8;
  return (buff[i] & (B10000000 >> pos)) != 0;
}

bool TFAReceiver::isRepeat(TFAResult r) {
  return r.type == result.type && r.id == result.id && r.battery == result.battery &&
         r.channel == result.channel && r.temperature == result.temperature &&
         r.humidity == result.humidity && (millis() - lastResultTime < 3000);
}
