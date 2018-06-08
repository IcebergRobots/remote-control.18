#define POTI_X A0
#define POTI_Y A1
#define DEBUG 1
#define DEBUG_SERIAL Serial
#define RANGE 100

#define BLUETOOTH_SERIAL Serial   // Serial des Bluetooth-Moduls
#define START_MARKER 254          // Startzeichen einer Bluetooth-Nachricht
#define END_MARKER 255            // Endzeichen einer Bluetooth-Nachricht

double x = 0;
double y = 0;
double angle = 0;
double squarePower = 0;
double squarePowerMax = 0;
double power = 0;

void setup() {
  Serial.begin(115200);
  pinMode(POTI_X, INPUT);
  pinMode(POTI_Y, INPUT);
}

void loop() {

  /*
    Ziel: Joystickposition zu Richtung und Stärke unrechnen.
    Problem: Joystick bewegt sich im Quadrat
      und hat bei verschiedenen Richtungen verschiedene Maximalstärken.

    +-----.-------.-----+  RANGE -.
    |  .'         . '.  |         |
    | /      v   /    \ |         |
    |'         /       '|         |
    |         °         |   0     y
    |.                 .|         |
    | \               / |         |
    |  '.           .'  |         |
    +-----'-------'-----+ -RANGE -'
    -RANGE    0       RANGE
    '---------x--------'

    Joystick gibt mit zwei Potis Punkt im Quadrat an.
    Punkt wird als Vektor (v) betrachtet.
    Richtung(angle) und Betrag(squarePower) werden gespeichert.
  */
  x = map(analogRead(POTI_X), 0, 1023, -RANGE, RANGE);  // horizontale Änderung
  y = map(analogRead(POTI_Y), 0, 1023, -RANGE, RANGE);  // vertikale Änderung
  angle = atan2(y, x) * 180 / PI; // Drehwinkel
  squarePower = vectorLength(x, y); // Vektorlänge in einem Quadrat


  /*
    +-----.-------.--/--+
    |  .'          /'.  |
    | /      v_m /    \ |
    |'         /       '|
    |         °         |
    |.                 .|
    | \               / |
    |  '.           .'  |
    +-----'-------'-----+

    Vektor wird skaliert(v_m), sodass er das Quadrat berührt.
    Sein Betrag(squarePowerMax) ist der maximale Betrag für diese Richtung.
  */
  squarePowerMax = vectorLength(min(abs(x), abs(y)) * RANGE / max(abs(x), abs(y)), RANGE);


  /*
    +-----.-------.-----+
    |  .'           '.  |
    | /      v' .     \ |
    |'         /       '|
    |         °         |
    |.                 .|
    | \               / |
    |  '.           .'  |
    +-----'-------'-----+

    Ursprünglicher Vektor(v) wird mit Kehrwert des jeweils maximalen Betrags(squarePowerMax) skaliert,
    damit der auszugebende maximale Betrag konstant RANGE bleibt.
    Der Betrag(power) des Vektors(v') wird gespeichert.
  */
  power = map(squarePower, 0, squarePowerMax, 0, RANGE);  // Vektorlänge skaliert in einen Kreis
  /*debug(angle, 4);
    debug(power, 4);
    debugln();*/
  byte data[4];
  data[0] = 'h';
  data[1] = angle >= 0;
  data[2] = abs(angle);
  data[3] = power;
  broadcast(data, 4);
}

double vectorLength(long a, long b) {
  return sqrt(a * a + b * b);
}

void broadcast(byte * data, byte numberOfElements) {
  BLUETOOTH_SERIAL.write(START_MARKER);
  for (byte i = 0; i < numberOfElements; i++) {
    BLUETOOTH_SERIAL.write(constrain(data[i], 0, 253));
  }
  BLUETOOTH_SERIAL.write(END_MARKER);
}
