#include <LiquidCrystal.h>

#define ROT_PORT PORTC
#define ROT_PIN PINC
#define ROT_A _BV(1)
#define ROT_B _BV(2)
#define ROT_BTN _BV(3)
#define ROT_DELAY 110
#define ROT_BTN_DELAY 255

const int rs = 2, en = 4, d4 = 8, d5 = 9, d6 = 10, d7 = 11;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte smiley[8] = {
  B00000,
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

  OCR2A = 110;
  TCCR2A = _BV(WGM20);
  TIMSK2 |= _BV(TOIE2);

  ROT_PORT |= ROT_A | ROT_B;
  PCIFR |= _BV(PCIF1);
  PCICR |= _BV(PCIE1);
  PCMSK1 |= ROT_A | ROT_B | ROT_BTN;
}

volatile int _v = 0;
volatile int _b = 0;
volatile byte _lastRot;
volatile char _sign = 0;

ISR(TIMER2_OVF_vect) {
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
  TCCR2B = _BV(CS22) | _BV(WGM22);
}

volatile int _last = 0;
volatile int _lastB = 0;
void loop() {
  if (_v != _last || _b != _lastB)
  {
    lcd.setCursor(0, 0);
    lcd.print(_v);
    lcd.print("    ");
    lcd.setCursor(0, 5);
    lcd.write((byte)0);
    _last = _v;
    _lastB = _b;
    Serial.print(_v);
    Serial.print("\t");
    Serial.print(_b);
    Serial.println();
  }
}
