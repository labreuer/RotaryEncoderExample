#define sbi(x,y) x |= _BV(y) //set bit - using bitwise OR operator 
#define cbi(x,y) x &= ~(_BV(y)) //clear bit - using bitwise AND operator

#define ROT_PORT PORTC
#define ROT_PIN PINC
#define ROT_A _BV(1)
#define ROT_B _BV(2)

void setup() {
  Serial.begin(9600);

  DDRB |= 0x3F;
  OCR2A = 110;
  TCCR2A = _BV(WGM20);
  TIMSK2 |= _BV(TOIE2);

  ROT_PORT |= ROT_A | ROT_B;
  PCIFR |= _BV(PCIF1);
  PCICR |= _BV(PCIE1);
  PCMSK1 |= ROT_A | ROT_B;
}

volatile int _v = 0;
volatile byte _lastRot;
volatile char _sign = 0;

ISR(TIMER2_OVF_vect) {
  PORTB &= ~0x1F;
  TCCR2B = 0;
  _v += _sign;
}

ISR(PCINT1_vect) {
  byte _rot = ROT_PIN;
  if ((_lastRot ^ _rot) & ROT_A && (_rot & ROT_B) == 0) {
    if (_rot & ROT_A) {
      TCCR2B = 0;
    } else {
      _sign = -1;
      setDelay();
    }
  }
  if ((_lastRot ^ _rot) & ROT_B && (_rot & ROT_A) == 0) {
    if (_rot & ROT_B) {
      TCCR2B = 0;
    } else {
      _sign = 1;
      setDelay();
    }
  }
  _lastRot = _rot;
}

void setDelay() {
  TCNT2 = 1;
  TCCR2B = _BV(CS22) | _BV(WGM22);
  PORTB |= _BV(4);
}

volatile int _last = 0;
void loop() {
  if (_v != _last)
  {
    _last = _v;
    Serial.print(_v);
    Serial.println();
  }
}
