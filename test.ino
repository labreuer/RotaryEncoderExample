void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), toggle, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), toggle, FALLING);

  Serial.begin(9600);

  //Timer1.initialize(1000);
  //Timer1.attachInterrupt(tim);
  ICR1 = 100*16;

  DDRB |= 0x7;
}

volatile int _t = 0;
volatile int _resets = 0;

ISR(TIMER1_OVF_vect) {
  PORTB |= _BV(3);
  _t++;
  TCCR1B = 0;
  PORTB &= ~_BV(0);
  PORTB &= ~_BV(3);
}

void toggle() {
  if (TCCR1B != 0)
  {
    PORTB |= _BV(1);
    PORTB &= ~_BV(0);
    _resets++;
    PORTB &= ~_BV(1);
  }
  else
  {
    PORTB |= _BV(2);
    PORTB &= ~_BV(2);
  }
  TCNT1 = 1;
  PORTB |= _BV(0);
  ICR1 = 100*16;
  TCCR1A = _BV(WGM11);
  TCCR1B = _BV(CS10) | _BV(WGM13) | _BV(WGM12);
  TIMSK1 = _BV(TOIE1);
}

volatile int _x = 0;
volatile int _y = 0;

void toggleA() {
  _x++;
}

void toggleB() {  
  _y++;
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(_resets);
  Serial.print(" ");
  Serial.print(_t);
  Serial.println();
  delay(1000);
}
