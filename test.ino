#define sbi(x,y) x |= _BV(y) //set bit - using bitwise OR operator 
#define cbi(x,y) x &= ~(_BV(y)) //clear bit - using bitwise AND operator

#define ROT_PORT PORTC
#define ROT_PIN PINC
#define ROT_A _BV(1)
#define ROT_B _BV(2)

void setup() {
  // put your setup code here, to run once:
  //pinMode(2, INPUT_PULLUP);
  //pinMode(3, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(2), change2, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(3), change3, CHANGE);

  Serial.begin(9600);

  DDRB |= 0x3F;
  ICR1 = 110;
  TCCR1A = _BV(WGM11);
  TIMSK1 |= _BV(TOIE1);

  ROT_PORT |= ROT_A | ROT_B;
  PCIFR |= _BV(PCIF1);
  PCICR |= _BV(PCIE1);
  PCMSK1 |= ROT_A | ROT_B;
}

volatile int _v = 0;
volatile byte _lastRot;
volatile char _sign = 0;

ISR(TIMER1_OVF_vect) {
  PORTB &= ~0x1F;
  TCCR1B = 0;
  _v += _sign;
}

ISR(PCINT1_vect) {
  byte _rot = ROT_PIN;
  if ((_lastRot ^ _rot) & ROT_A && (_rot & ROT_B) == 0) {
    if (_rot & ROT_A) {
      TCCR1B = 0;
    } else {
      _sign = -1;
      setDelay();
    }
  }
  if ((_lastRot ^ _rot) & ROT_B && (_rot & ROT_A) == 0) {
    if (_rot & ROT_B) {
      TCCR1B = 0;
    } else {
      _sign = 1;
      setDelay();
    }
  }
  _lastRot = _rot;
}

void change2() {
  // only pay attention if the other pin is already low
  if (!(PIND & _BV(3)))
    return;
  // if this pin high, wait for a stable low
  if (PIND & _BV(2)) {
    TCCR1B = 0;
    return;
  }

  _sign = 1;
  setDelay();
}

void change3() {
  // only pay attention if the other pin is already low
  if (!(PIND & _BV(2)))
    return;
  // if this pin high, wait for a stable low
  if (PIND & _BV(3)) {
    TCCR1B = 0;
    return;
  }

  _sign = -1;
  setDelay();
}

void setDelay() {
  //EIMSK &= ~0x03;
  TCNT1 = 1;
  TCCR1B = _BV(CS11) | _BV(CS10) | _BV(WGM13) | _BV(WGM12);
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
