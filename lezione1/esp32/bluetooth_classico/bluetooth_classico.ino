
#include "BluetoothSerial.h"

String device_name = "ESP32-BT-CMD";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

#define BUFF_SIZE 32

BluetoothSerial SerialBT;



/* configurazione per la luminosità del led*/
#define LEDC_CHANNEL_0     0
#define LEDC_TIMER_12_BIT  12
#define LEDC_BASE_FREQ     5000
#define LED_PIN            2


char buffer[BUFF_SIZE];
char response[BUFF_SIZE];
int idx = 0;

void clean_buffer() {
  memset(buffer, '\0', BUFF_SIZE);
  idx = 0;
}

void send_buffer(const char *buff) {
  Serial.print("->");
  Serial.println(buff);
  SerialBT.write(reinterpret_cast<const uint8_t *>(buff), strlen(buff));
}

void execute_read(int pin) {
  uint8_t pin_name = 0;

  switch (pin) {
  case 0:
    pin_name = T0;
    break;
  case 1:
    pin_name = T1;
    break;
  case 2:
    pin_name = T2;
    break;
  case 3:
    pin_name = T3;
    break;
  case 4:
    pin_name = T4;
    break;
  case 5:
    pin_name = T5;
    break;
  case 6:
    pin_name = T6;
    break;
  case 7:
    pin_name = T7;
    break;
  case 8:
    pin_name = T8;
    break;
  case 9:
    pin_name = T9;
    break;
  default:
    break;
  }

  touch_value_t value = touchRead(pin_name);
  memset(response, '\0', BUFF_SIZE);
  sprintf(response, "ACK,%d;", value);
  send_buffer(response);
  clean_buffer();
}

void execute_write(int val) {
  constexpr uint32_t valueMax = 255;
  uint32_t value = val;
  // calculate duty, 4095 from 2 ^ 12 - 1
  uint32_t duty = (4095 / valueMax) * min(value, valueMax);

  // write duty to LEDC
  ledcWrite(LEDC_CHANNEL_0, duty);
  Serial.print("->");
  Serial.println(buffer);
  memset(response, '\0', BUFF_SIZE);
  sprintf(response, "ACK,%d;", value);
  send_buffer(response);
  clean_buffer();
}

void send_nak() {
  Serial.print("->");
  Serial.println(buffer);
  send_buffer("NAK;");
  clean_buffer();
}

void parse(char c) {
  if (idx >= BUFF_SIZE) {
    /* abbiamo esaurito il buffer senza trovare il carattere terminatore.
     * puliamo il buffer e ritorniamo errore
     */
    clean_buffer();
    return;
  }
  buffer[idx++] = c;
  if (c != ';')
    /* il comando non è terminato, ritorniamo 0*/
    return;

  // facciamo il parsing del comando
  int val;
  if (sscanf(buffer, "R%i;", &val) == 1) {
    if (val < 0 || val >= 10) {
      // valore oltre ai limiti,ritorniamo errore
      send_nak();
      return;
    }
    /* comando riconosciuto, eseguiamo*/
    execute_read(val);
    return;
  }

  if (sscanf(buffer, "W%i;", &val) == 1) {
    if (val < 0 || val > 255) {
      // valore oltre ai limiti,ritorniamo errore
      send_nak();
      return;
    }
    /* comando riconosciuto, eseguiamo*/
    execute_write(val);
    return;
  }

  // se raggiungiamo questa riga, allora il comando non è stato riconosciuto
  send_nak();
}

void setup() {
  /* setup led*/
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_12_BIT);
  ledcAttachPin(LED_PIN, LEDC_CHANNEL_0);

  Serial.begin(115200);
  SerialBT.begin(device_name); // Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it "
                "with Bluetooth!\n",
                device_name.c_str());
}

void loop() {
  while (SerialBT.available()) {
    int c = SerialBT.read();
    if (c < 0)
      break; // c'è stato un erroe di lettura usciamo dal loop
    parse(c);
  }
  delay(100);
}