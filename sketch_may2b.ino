
#include <LiquidCrystal.h>

LiquidCrystal lcd(12, 11, 5, 4, 1, 0);
#include <SoftwareSerial.h>
SoftwareSerial pmsSerial(2,3);
float CO=0;
float O3=0;
float CO_ppm=0;
float O3_ppm=0;
void setup() {
  // our debugging output
  Serial.begin(9600);
 
  // sensor baud rate is 9600
  pmsSerial.begin(9600);
  lcd.begin(16, 2);
  analogWrite(6, 100);
}
 
struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};
 
struct pms5003data data;
    
void loop() {
  lcd.setCursor(0, 0);

  CO=analogRead(A0);
  O3=analogRead(A1);
  O3_ppm=O3;
  CO_ppm=CO*5/1024;

  if(readPMSdata(&pmsSerial)){
    lcd.print("4 pollutants");
    delay(1000);
    lcd.clear();
    delay(1000);
    lcd.print("CO O3 PM25 PM10");
    lcd.clear();
    delay(1000);
    // reading data was successful!
    Serial.print("\t\tCO: "); Serial.print(CO_ppm);
    lcd.print(CO_ppm);
    delay(1000);
    lcd.clear();
    delay(1000);
    Serial.print("\t\tO3: "); Serial.print(O3_ppm);
    lcd.print(O3_ppm);
    delay(1000);
    lcd.clear();
    delay(1000);
    Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_standard);
    lcd.print(data.pm25_standard);
    delay(1000);
    delay(1000);
    lcd.clear();
    delay(1000);
    Serial.print("\t\tPM 10: "); Serial.println(data.pm100_standard);
    lcd.print(data.pm100_standard);
    delay(1000);
    lcd.clear();
    delay(1000);

    delay(1000);
  }
}
 
boolean readPMSdata(Stream *s) {
  if (! s->available()) {
    return false;
  }
  
  // Read a byte at a time until we get to the special '0x42' start-byte
  if (s->peek() != 0x42) {
    s->read();
    return false;
  }
 
  // Now read all 32 bytes
  if (s->available() < 32) {
    return false;
  }
    
  uint8_t buffer[32];    
  uint16_t sum = 0;
  s->readBytes(buffer, 32);
 
  // get checksum ready
  for (uint8_t i=0; i<30; i++) {
    sum += buffer[i];
  }
 
  /* debugging
  for (uint8_t i=2; i<32; i++) {
    Serial.print("0x"); Serial.print(buffer[i], HEX); Serial.print(", ");
  }
  Serial.println();
  */
  
  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i=0; i<15; i++) {
    buffer_u16[i] = buffer[2 + i*2 + 1];
    buffer_u16[i] += (buffer[2 + i*2] << 8);
  }
 
  // put it into a nice struct :)
  memcpy((void *)&data, (void *)buffer_u16, 30);
 
  if (sum != data.checksum) {
    Serial.println("Checksum failure");
    return false;
  }
  // success!
  return true;
}
