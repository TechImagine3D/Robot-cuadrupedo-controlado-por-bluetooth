
/*
 *Código para robot Kachokame
 * 
 D2-D9 Conexionado para servo
D2 ->leg0A servo
D3 ->leg0B servo
D4 ->leg1A servo
D5 ->leg1B servo
D6 ->leg2A servo
D7 ->leg2B servo
D8 ->leg3A servo
D9 ->leg3B servo
 */
#include <Servo.h>  

#define START_PIN 2
#define TIMEOUT 5000

#define FORWARD 'a'  //Datos que recibirá desde bluetooth
#define LEFT 'b'
#define STAND 'c'
#define RIGHT 'd'
#define BACKWARD 'e'

#define CONTROL_OFF 'g'
#define CONTROL_ON  'f'


typedef struct { //Crea una matriz de datos que indica el número del servo
  int index;     // el grado que se va a mover, y el tiempo que permanecerá 
  int degree;    //en ese movimiento
  int delay;
} angle_t;

angle_t *forward = (angle_t []) {
  {0, 140, 0},
  {1, 60, 0},
  {2, 40, 0},
  {3, 90, 0},
  {4, 150, 0},
  {5, 60, 0},
  {6, 55, 0},
  {7, 90, 200},
  {4, 80, 0},
  {6, 100, 200},
  {1, 90, 0},
  {3, 120, 0},
  {5, 110, 0},
  {7, 120, 200},
  {0, 80, 0},
  {2, 100, 0},
  {3, 90, 0},
  {4, 125, 0},
  {6, 55, 200},
  { -1, -1, -1}
};

angle_t *left = (angle_t []) {
  {3, 150, 0},
  {4, 132, 0},
  {7, 150, 100},
  {2, 125, 0},
  {6, 125, 100},
  {3, 90, 0},
  {7, 90, 100},
  {1, 30, 0},
  {5, 30, 100},
  {2, 55, 0},
  {6, 55, 100},
  {1, 90, 0},
  {5, 90, 100},
  { -1, -1, -1}
};

angle_t *right = (angle_t []) {
  {1, 50, 0},
  {5, 50, 100},
  {0, 50, 0},
  {4, 50, 100},
  {1, 90, 0},
  {5, 90, 100},
  {3, 150, 0},
  {7, 150, 100},
  {0, 125, 0},
  {4, 125, 100},
  {3, 90, 0},
  {7, 90, 100},
  { -1, -1, -1}
};

angle_t *backward = (angle_t []) {
  {0, 125, 0},
  {1, 60, 0},
  {2, 55, 0},
  {3, 90, 0},
  {4, 150, 0},
  {5, 60, 0},
  {6, 55, 0},
  {7, 90, 200},
  {0, 80, 0},
  {2, 100, 200},
  {1, 90, 0},
  {3, 120, 0},
  {5, 90, 0},
  {7, 120, 200},
  {0, 125, 0},
  {2, 55, 0},
  {4, 80, 0},
  {6, 110, 200},
  { -1, -1, -1}
};

angle_t *stand = (angle_t []) {
  {0, 125, 0},
  {2, 55, 0},
  {4, 125, 0},
  {6, 55, 0},
  {1, 90, 0},
  {3, 90, 0},
  {5, 90, 0},
  {7, 90, 150},
  { -1, -1, -1}
};

void locomotion(angle_t angles[]);

Servo servo[8];
unsigned long cur_time, prev_time;
char action;
char prev_action;
boolean auto_mode = true;

void setup() {
  int i;
  for (i = 0; i < 8; i++) {//Al iniciar se pone todos los servos a 90 grados
    servo[i].attach(i + START_PIN);
    servo[i].write(90);
  }
  Serial.begin(9600);
  locomotion(stand);
  randomSeed(analogRead(0));
  cur_time = prev_time = millis();
  prev_action = action = FORWARD;
  delay(3000);
}

void loop() {

  if (Serial.available () > 0) {  //Detecta si hay datos
    action = Serial.read ();
    //Serial.println(cmd);
    switch (action) {      //Si llega datos se pone en modo control desde movil o autonomo
      case CONTROL_ON: auto_mode = false; action = STAND;
        break;
      case CONTROL_OFF: auto_mode = true; action = STAND;
        //Serial.println("control off");
        break;
    }
  }
// Si esta en modo autonomo se elige los movimiento aleatoriamente
  if (auto_mode) {
    cur_time = millis();                    //si el tiempo desde que se ha encendido 
    if (cur_time - prev_time >= TIMEOUT) {  //sobrepasa 5 seg, se pone en modo autonomo
      prev_time = cur_time;
      do {
        action = (int)random(FORWARD, BACKWARD + 1); 
      } while (action == prev_action || action == STAND);
      //Serial.println(action);
    }
  }
//en caso que llega datos al poner en modo movil se selecciona los movimiento 
//dependiendo de los datos que llege desde el movil.
  if (1) {
    switch (action) {
      case FORWARD: if (action != prev_action) {
          prev_action = action;
          locomotion(stand);
          //Serial.println("forward");
        }
        locomotion(forward);
        break;
      case BACKWARD: if (action != prev_action) {
          prev_action = action;
          locomotion(stand);
          //Serial.println("backward");
        }
        locomotion(backward);
        break;
      case LEFT: if (action != prev_action) {
          prev_action = action;
          locomotion(stand);
          //Serial.println("left");
        }
        locomotion(left);
        break;
      case RIGHT: if (action != prev_action) {
          prev_action = action;
          locomotion(stand);
          //Serial.println("right");
        }
        locomotion(right);
        break;
      case STAND:
        locomotion(stand);
        break;
    }
  }

}
// Función de movimiento del motor dependiendo de los parámetros
// de numero de servo, grados y tiempo de duración.
void locomotion(angle_t angles[]) {
  int i = 0;
  while (1) { // si los datos lleda -1 salta la rutina del movimiento
    if (angles[i].degree == -1 ||
        angles[i].index == -1 ||
        angles[i].delay == -1) {
      break;
    }
    servo[angles[i].index].write(angles[i].degree);
    if (angles[i].delay > 0) delay(angles[i].delay);
    i++;
    if (0) {
      Serial.print("loop ");
      Serial.print(i);
      Serial.print("-");
      Serial.print(angles[i].index);
      Serial.print(" ");
      Serial.print(angles[i].degree);
      Serial.print(" ");
      Serial.print(angles[i].delay);
      Serial.println();
    }
  }
  //delay(1000);
}


