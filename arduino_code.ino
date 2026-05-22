char data;
int motor = 8;

void setup() {
  Serial.begin(9600);
  pinMode(motor, OUTPUT);
}

void loop() {

  if (Serial.available()) {
    data = Serial.read();

    if (data == '1') {
      digitalWrite(motor, HIGH);   // Motor ON
    }

    else {
      digitalWrite(motor, LOW);    // Motor OFF
    }
  }
}