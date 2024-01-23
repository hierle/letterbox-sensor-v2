// 
// letterbox-sensor-v2.ino
//

#include <Arduino.h>
#include "config.h"
#include "pins.h"
#include "lorawan.h"

void setup() {

  // set up serial line
  Serial.begin(115200);
  delay(1000);
  Serial.println("-------------------------");
  Serial.println("Letterbox sensor v2");
  Serial.printf("Firmware Version: %s\r\n", api.system.firmwareVersion.get().c_str());
  Serial.printf("CLI Version: %s\r\n", api.system.cliVersion.get().c_str());
  Serial.printf("API Version: %s\r\n", api.system.apiVersion.get().c_str());
  Serial.printf("Model ID: %s\r\n", api.system.modelId.get().c_str());
  Serial.printf("Hardware ID: %s\r\n", api.system.chipId.get().c_str());
  Serial.println("-------------------------");

  // set pinmodes
  pinMode(statusled, OUTPUT);
  pinMode(irled1, OUTPUT);
  pinMode(irled2, OUTPUT);
  pinMode(irdiode1, OUTPUT);
  pinMode(irdiode2, OUTPUT);
  pinMode(irsens1, INPUT);
  pinMode(irsens2, INPUT);

  // init lorawan
  lorawan_init();

  // set low power mode
  if (!api.system.lpm.set(1)) {	Serial.println("Set low power mode failed! \r\n");}

  // ADC mode
  //udrv_adc_set_mode(UDRV_ADC_MODE_3_3);

  // blink led once on startup
  digitalWrite(statusled,HIGH);
  delay(50);
  digitalWrite(statusled,LOW);

	// create timer
	api.system.timer.create(RAK_TIMER_0, sensor_handler, RAK_TIMER_PERIODIC);
  // start timer
	api.system.timer.start(RAK_TIMER_0, PERIOD*60*1000, NULL);

  // get and send sensor values on boot
	sensor_handler(NULL);
}

// read sensor values and send lorawan package
void sensor_handler(void *) {

  // variables
  unsigned int s1 = 0; // sensor#1
  unsigned int s2 = 0; // sensor#2
  unsigned int bat = 0; // battery

  // data buffer
  uint8_t data_length = 8;
  uint8_t data[data_length];

  // measure sensor #1
  digitalWrite(irled1,HIGH);
  digitalWrite(irdiode1,HIGH);
  delay(10);
  for(int i = 0 ; i <3 ; i++){
    delay(25);
    s1 += analogRead(irsens1);
  }
  s1=s1/3;
  digitalWrite(irled1,LOW);
  digitalWrite(irdiode1,LOW);
  Serial.printf("Sensor#1: %d\r\n", s1);

  // measure sensor #2
  digitalWrite(irled2,HIGH);
  digitalWrite(irdiode2,HIGH);
  delay(10);
  for(int i = 0 ; i <3 ; i++){
    delay(25);
    s2 += analogRead(irsens2);
  }
  s2=s2/3;
  digitalWrite(irled2,LOW);
  digitalWrite(irdiode2,LOW);
  Serial.printf("Sensor#2: %d\r\n", s2);

  // letterbox full/empty
  if((s1 > THRESHOLD)||(s2 > THRESHOLD)) { data[0] = 0xFF; }
  else { data[0] = 0x00; }

  // battery voltage from external voltage divider
  analogReadResolution(12);
  delay(25);
  bat=analogRead(vdiv);
  //Serial.printf("Voltage12b: %d\r\n", bat);
  //bat = 1750 + (bat - 1235)*10; // 2x 3.3MOhm
  bat = 1550 + (bat - 1040)*8; // 2x 10MOhm
  Serial.printf("Voltage: %d\r\n", bat);
  analogReadResolution(10);

  // battery
  data[1] = (bat & 0xFF);
  data[2] = ((bat >> 8) & 0xFF);

  // sensor#1
  data[3] = (s1 & 0xFF);
  data[4] = ((s1 >> 8) & 0xFF);

  // sensor#2
  data[5] = (s2 & 0xFF);
  data[6] = ((s2 >> 8) & 0xFF);

  // threshold
  data[7] = THRESHOLD;

  // temperature N/A for now
  //data[8] = readTemp();

  Serial.println("-------------------------");

  // send data via lorawan
  lorawan_send(data, data_length);

}


void loop(){
  // destroy this busy loop and use a timer instead,
  // so that the system thread can auto enter low power mode by api.system.lpm.set(1)
  api.system.scheduler.task.destroy();
}

