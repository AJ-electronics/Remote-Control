#include <SPI.h>
#include <RF24.h>
#include <Wire.h>
#include <U8g2lib.h>

// ===================== ANALOG INPUTS =====================

#define PIN_J1_X   34
#define PIN_J1_Y   35

#define PIN_J2_X   32
#define PIN_J2_Y   33

#define PIN_POT1   36
#define PIN_POT2   39

#define BATTERY_PIN 26

// ===================== DIGITAL INPUTS =====================

#define PIN_TOG1   13
#define PIN_TOG2   14

#define PIN_BTN1   17
#define PIN_BTN2   16

// ===================== NRF24 =====================

#define NRF_CE     25
#define NRF_CSN    27

#define NRF_SCK    18
#define NRF_MISO   19
#define NRF_MOSI   23

RF24 radio(NRF_CE, NRF_CSN);

const byte address[6] = "RC001";

// ===================== OLED =====================

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(
  U8G2_R0,
  U8X8_PIN_NONE
);

// ===================== DATA STRUCT =====================

struct ControlPacket {
  uint16_t j1x;
  uint16_t j1y;
  uint16_t j2x;
  uint16_t j2y;
  uint16_t pot1;
  uint16_t pot2;
  bool tog1;
  bool tog2;
  bool btn1;
  bool btn2;
};

ControlPacket data;

// ===================== SMOOTH ANALOG READ =====================

uint16_t smoothRead(int pin) {
  uint32_t sum = 0;
  for (int i = 0; i < 8; i++) {
    sum += analogRead(pin);
    delayMicroseconds(100);
  }
  return sum / 8;
}

// ===================== INVERT ADC =====================

uint16_t invert12(uint16_t v) {
  return 4095 - v;
}

// ===================== BATTERY CALIBRATION TABLE =====================

const uint16_t adcTable[] = {
  1637, 1664, 1690, 1716, 1744,
  1773, 1800, 1824, 1852, 1878,
  1904, 1934, 1960, 1987, 2015
};

const float voltTable[] = {
  3.60, 3.65, 3.70, 3.75, 3.80,
  3.85, 3.90, 3.95, 4.00, 4.05,
  4.10, 4.15, 4.20, 4.25, 4.30
};

const int TABLE_SIZE = sizeof(adcTable) / sizeof(adcTable[0]);

uint16_t readBatteryRaw() {
  uint32_t sum = 0;
  for (int i = 0; i < 16; i++) {
    sum += analogRead(BATTERY_PIN);
    delayMicroseconds(100);
  }
  return sum / 16;
}

float adcToVoltage(uint16_t raw) {
  if (raw <= adcTable[0]) {
    return voltTable[0];
  }

  if (raw >= adcTable[TABLE_SIZE - 1]) {
    return voltTable[TABLE_SIZE - 1];
  }

  for (int i = 0; i < TABLE_SIZE - 1; i++) {
    if (raw >= adcTable[i] && raw <= adcTable[i + 1]) {
      float x0 = adcTable[i];
      float x1 = adcTable[i + 1];
      float y0 = voltTable[i];
      float y1 = voltTable[i + 1];

      return y0 + (raw - x0) * (y1 - y0) / (x1 - x0);
    }
  }

  return 0.0f;
}

float getBatteryVoltage() {
  uint16_t raw = readBatteryRaw();
  return adcToVoltage(raw);
}

// ===================== SETUP =====================

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(PIN_TOG1, INPUT_PULLUP);
  pinMode(PIN_TOG2, INPUT_PULLUP);
  pinMode(PIN_BTN1, INPUT_PULLUP);
  pinMode(PIN_BTN2, INPUT_PULLUP);

  analogReadResolution(12);

  analogSetPinAttenuation(PIN_J1_X, ADC_11db);
  analogSetPinAttenuation(PIN_J1_Y, ADC_11db);

  analogSetPinAttenuation(PIN_J2_X, ADC_11db);
  analogSetPinAttenuation(PIN_J2_Y, ADC_11db);

  analogSetPinAttenuation(PIN_POT1, ADC_11db);
  analogSetPinAttenuation(PIN_POT2, ADC_11db);

  analogSetPinAttenuation(BATTERY_PIN, ADC_11db);

  // ===================== OLED =====================

  Wire.begin(21, 22);

  display.begin();

  display.clearBuffer();

  display.setFont(u8g2_font_6x12_tr);

  display.drawStr(0, 15, "ESP32 REMOTE");

  display.drawStr(0, 35, "OLED OK");

  display.sendBuffer();

  // ===================== NRF24 =====================

  SPI.begin(NRF_SCK, NRF_MISO, NRF_MOSI, NRF_CSN);

  Serial.println("Starting NRF24...");

  if (!radio.begin()) {
    Serial.println("NRF NOT DETECTED");

    display.clearBuffer();

    display.drawStr(0, 20, "NRF FAILED");

    display.sendBuffer();

    while (1) {
      delay(100);
    }
  }

  radio.setChannel(76);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(address);
  radio.stopListening();

  Serial.println("NRF OK");

  display.clearBuffer();

  display.drawStr(0, 20, "NRF OK");

  display.sendBuffer();

  delay(1000);
}

// ===================== LOOP =====================

void loop() {
  // ===================== READ ANALOGS =====================

  data.j1x = invert12(smoothRead(PIN_J1_Y));
  data.j1y = invert12(smoothRead(PIN_J1_X));

  data.j2x = invert12(smoothRead(PIN_J2_Y));
  data.j2y = invert12(smoothRead(PIN_J2_X));

  data.pot1 = invert12(smoothRead(PIN_POT2));
  data.pot2 = invert12(smoothRead(PIN_POT1));

  // ===================== READ DIGITALS =====================

  data.tog1 = !digitalRead(PIN_TOG2);
  data.tog2 = !digitalRead(PIN_TOG1);

  data.btn1 = !digitalRead(PIN_BTN1);
  data.btn2 = !digitalRead(PIN_BTN2);

  // ===================== BATTERY =====================

  float battVoltage = getBatteryVoltage();

  // ===================== SEND NRF =====================

  bool ok = radio.write(&data, sizeof(data));

  // ===================== SERIAL OUTPUT =====================

  Serial.print("J1X:");
  Serial.print(data.j1x);
  Serial.print(" J1Y:");
  Serial.print(data.j1y);
  Serial.print(" J2X:");
  Serial.print(data.j2x);
  Serial.print(" J2Y:");
  Serial.print(data.j2y);
  Serial.print(" P1:");
  Serial.print(data.pot1);
  Serial.print(" P2:");
  Serial.print(data.pot2);
  Serial.print(" T1:");
  Serial.print(data.tog1);
  Serial.print(" T2:");
  Serial.print(data.tog2);
  Serial.print(" B1:");
  Serial.print(data.btn1);
  Serial.print(" B2:");
  Serial.print(data.btn2);
  Serial.print(" BAT:");
  Serial.print(battVoltage, 2);
  Serial.print("V ");
  Serial.print(" NRF:");

  if (ok)
    Serial.println("OK");
  else
    Serial.println("FAIL");

  // ===================== OLED =====================

  display.clearBuffer();

  display.setFont(u8g2_font_5x8_tr);

  char buf[32];
  char battBuf[20];

  display.drawStr(0, 8, "ESP32 REMOTE");

  snprintf(battBuf, sizeof(battBuf), "BAT: %.2fV", battVoltage);
  display.drawStr(0, 16, battBuf);

  snprintf(buf, sizeof(buf),
           "J1X:%4u J1Y:%4u",
           data.j1x,
           data.j1y);

  display.drawStr(0, 28, buf);

  snprintf(buf, sizeof(buf),
           "J2X:%4u J2Y:%4u",
           data.j2x,
           data.j2y);

  display.drawStr(0, 38, buf);

  snprintf(buf, sizeof(buf),
           "P1:%4u P2:%4u",
           data.pot1,
           data.pot2);

  display.drawStr(0, 48, buf);

  snprintf(buf, sizeof(buf),
           "T1:%d T2:%d B1:%d B2:%d",
           data.tog1,
           data.tog2,
           data.btn1,
           data.btn2);

  display.drawStr(0, 58, buf);

  if (ok)
    display.drawStr(102, 16, "OK");
  else
    display.drawStr(96, 16, "NO");

  display.sendBuffer();

  delay(50);
}
