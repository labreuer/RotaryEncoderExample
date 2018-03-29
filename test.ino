

void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), fall2, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), fall3, FALLING);

  Serial.begin(9600);

  //Timer1.initialize(1000);
  //Timer1.attachInterrupt(tim);
  DDRB |= 0xF;
}

volatile int _v = 0;

ISR(TIMER1_OVF_vect) {
  PORTB &= ~_BV(3);
  PORTB |= _BV(2);
  TCCR1B = 0;
  EIFR = 0x03;
  EIMSK |= 0x03;
  PORTB &= ~_BV(2);
}

void fall2() {
  if (!(PIND & _BV(3)))
    return;
  PORTB |= _BV(0);
  _v++;
  setDelay();
  PORTB &= ~_BV(0);
}

void fall3() {  
  if (!(PIND & _BV(2)))
    return;
  PORTB |= _BV(1);
  _v--;
  setDelay();
  PORTB &= ~_BV(1);
}

void setDelay() {
  EIMSK &= ~0x03;
  ICR1 = 400;
  TCCR1A = _BV(WGM11);
  TCCR1B = _BV(CS12) | _BV(WGM13) | _BV(WGM12);
  TIMSK1 |= _BV(TOIE1);
  PORTB |= _BV(3);
}

volatile int _last = 0;
void loop() {
  // put your main code here, to run repeatedly:
  if (_v != _last)
  {
    _last = _v;
    Serial.print(_v);
    Serial.println();
  }
  //delay(100);
}
