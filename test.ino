#include <LiquidCrystal.h>

#define ROT_PORT PORTC
#define ROT_PIN PINC
#define ROT_A _BV(1)
#define ROT_B _BV(2)
#define ROT_BTN _BV(3)
#define ROT_DELAY 110
#define ROT_BTN_DELAY 255
#define ROT_LED_PORT PORTD
#define ROT_LED_DDR DDRD
#define ROT_LED_R _BV(7)
#define ROT_LED_G _BV(6)
#define ROT_LED_B _BV(5)
#define ROT_LED_RGB (ROT_LED_R | ROT_LED_G | ROT_LED_B)

const int rs = 2, en = 4, d4 = 8, d5 = 9, d6 = 10, d7 = 11;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte smiley[8] = {
  B10000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
};

void setup() {
  pinMode(3, OUTPUT);
  lcd.begin(20, 4);
  lcd.createChar(0, smiley);

  Serial.begin(9600);

  OCR2A = ROT_DELAY;
  TCCR2B = 0;
  TCCR2A = _BV(WGM20);
  TIFR2 |= _BV(OCF2A);
  TIMSK2 |= _BV(OCIE2A);

  ROT_LED_DDR |= ROT_LED_RGB;
  ROT_PORT |= ROT_A | ROT_B;
  PCMSK1 |= ROT_A | ROT_B | ROT_BTN;
  PCIFR |= _BV(PCIF1);
  PCICR |= _BV(PCIE1);
}

volatile int _v = 0;
volatile int _b = 0;
volatile byte _lastRot = 0;
volatile char _sign = 0;

ISR(TIMER2_COMPA_vect) {
  TCCR2B = 0;

  if (_sign == 0)
    _b++;
  else
    _v += _sign;
}

ISR(PCINT1_vect) {
  byte _rot = ROT_PIN;
  if ((_lastRot ^ _rot) & ROT_A && (_rot & ROT_B) == 0) {
    if (_rot & ROT_A) {
      TCCR2B = 0;
    } else {
      _sign = -1;
      setDelay(ROT_DELAY);
    }
  }
  if ((_lastRot ^ _rot) & ROT_B && (_rot & ROT_A) == 0) {
    if (_rot & ROT_B) {
      TCCR2B = 0;
    } else {
      _sign = 1;
      setDelay(ROT_DELAY);
    }
  }
  if ((_lastRot ^ _rot) & ROT_BTN) {
    if (_rot & ROT_BTN) {
      TCCR2B = 0;
    } else {
      _sign = 0;
      setDelay(ROT_BTN_DELAY);
    }
  }

  _lastRot = _rot;
}

void setDelay(byte dly) {
  OCR2A = dly;
  TCNT2 = 1;
  TCCR2B = _BV(CS22) | _BV(CS20) | _BV(WGM22);
}

int _last = 0;
int _lastB = 0;

byte _led[4] = {
  ROT_LED_R,
  ROT_LED_G,
  ROT_LED_B,
  ROT_LED_RGB
};

void loop() {
  int v = _v;

  if (v != _last || _b != _lastB)
  {
    byte led = _led[umod(_v + _b, 4)];
    // active-LOW
    ROT_LED_PORT |= ROT_LED_RGB ^ led;
    ROT_LED_PORT &= ~led;

    byte i = umod(_last, 5 * 8);
    byte j = umod(v, 5 * 8);
    smiley[j / 5] ^= 1 << (4-(j % 5));
    smiley[i / 5] ^= 1 << (4-(i % 5));
    lcd.createChar(0, smiley);
    lcd.setCursor(0, 0);
    lcd.print(v);
    lcd.print("    ");
    lcd.setCursor(5, 0);
    lcd.print(_b);
    lcd.setCursor(0, 3);
    lcd.write((byte)0);
    _last = v;
    _lastB = _b;
    Serial.print(_v);
    Serial.print("\t");
    Serial.print(_b);
    Serial.println();
  }
}

byte umod(int n, byte mod) {
  return ((n % mod) + mod) % mod;
}

