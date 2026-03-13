#include <DHT.h>

#define DHTPIN 4
#define DHTTYPE DHT11

// Using '15' for timing stability on RISC-V
DHT dht(DHTPIN, DHTTYPE, 15);

// Threshold with decimal (e.g., 32.5)
float tempThreshold = 35.0;  // Critical threshold in Celsius

void setup() {
  Serial.begin(115200);
  pinMode(DHTPIN, INPUT_PULLUP);
  dht.begin();

  Serial.println("===============================");
  Serial.println(" RISC-V ASM WARNING (DECIMAL)  ");
  Serial.println("===============================");
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Sensor Error: Retrying...");
    delay(2000);
    return;
  }

  // Convert floats to fixed-point integers (Scale by 100 to keep 2 decimals)
  // Example: 32.55 becomes 3255
  int scaled_temp = (int)(temperature * 100);
  int scaled_thresh = (int)(tempThreshold * 100);
  int is_critical = 0;

  /* --- RISC-V ASSEMBLY LOGIC --- */
  asm volatile(
    "mv t0, %[curr]          \n\t"  // Move scaled_temp to t0
    "mv t1, %[thresh]        \n\t"  // Move scaled_thresh to t1
    "li %[flag], 0           \n\t"  // Assume Normal
    "blt t0, t1, finished    \n\t"  // Branch if t0 < t1
    "li %[flag], 1           \n\t"  // Set Critical if t0 >= t1
    "finished:               \n\t"
    : [flag] "=r"(is_critical)
    : [curr] "r"(scaled_temp),
      [thresh] "r"(scaled_thresh)
    : "t0", "t1");

  // Output with full decimal points
  Serial.print("Current Temp: ");
  Serial.print(temperature, 2);  // Print with 2 decimal places
  Serial.print(" C | Humidity: ");
  Serial.print(humidity, 2);
  Serial.println(" %");

  if (is_critical) {
    Serial.println(">> STATUS: [!!! CRITICAL !!!]");
    Serial.println(">> ALERT: High temperature detected via ASM logic.");
  } else {
    Serial.println(">> STATUS: [NORMAL]");
  }

  Serial.println("-------------------------------");
  delay(2000);
}