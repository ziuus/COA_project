extern "C" {
  // This tells the compiler the function is in our .S file
  int check_temp_asm(int current_temp, int threshold);
}

#include <DHT.h>

#define DHTPIN 4
DHT dht(DHTPIN, DHT11, 15);

void setup() {
  Serial.begin(115200);
  pinMode(DHTPIN, INPUT_PULLUP);
  dht.begin();
  Serial.println("System Ready - RISC-V Assembly Linked.");
}

void loop() {
  float temp = dht.readTemperature();
  if (isnan(temp)) return;

  // Scale by 100 to handle decimals in the ASM integer registers
  int current = (int)(temp * 100);
  int thresh = 3300; // 33.00 Degrees

  // CALL THE ASSEMBLY FUNCTION
  int is_critical = check_temp_asm(current, thresh);

  Serial.print("Temp: "); Serial.print(temp);
  if (is_critical == 1) {
    Serial.println(" | [!!! CRITICAL ALERT !!!]");
  } else {
    Serial.println(" | [NORMAL]");
  }
  
  delay(2000);
}