#include <DHT.h>

// --- Pin Definitions ---
#define DHTPIN 4       // DHT11 Data Pin
#define DHTTYPE DHT11

// Using GPIO 8 and 9 to avoid boot-strapping conflicts on ESP32-C3

#define LED_WARNING 6  // Red LED

// Using '15' for timing stability on RISC-V (ESP32-C3/S3)
DHT dht(DHTPIN, DHTTYPE, 15);

// Threshold with decimal (e.g., 35.0)
float tempThreshold = 33.0;  

void setup() {
  Serial.begin(115200);
  
  // Initialize LED pins
  pinMode(LED_WARNING, OUTPUT);
  
  // Ensure LEDs are off at start
  digitalWrite(LED_WARNING, LOW);

  pinMode(DHTPIN, INPUT_PULLUP);
  dht.begin();

  Serial.println("===============================");
  Serial.println(" ESP32-C3 RISC-V COA PROJECT   ");
  Serial.println(" Status: System Booted         ");
  Serial.println("===============================");
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Handle Sensor Failures
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println(">> [ERROR]: DHT11 Sensor Timeout");
    // Blink both LEDs to indicate hardware error
    digitalWrite(LED_WARNING, HIGH);
    delay(500);
    digitalWrite(LED_WARNING, LOW);
    delay(1500); 
    return;
  }

  // Convert floats to fixed-point (Scale by 100 for 2 decimal precision)
  int scaled_temp = (int)(temperature * 100);
  int scaled_thresh = (int)(tempThreshold * 100);
  int is_critical = 0;

  /* --- RISC-V ASSEMBLY LOGIC BLOCK --- */
  asm volatile(
    "mv t0, %[curr]          \n\t"  // Load scaled current temp into t0
    "mv t1, %[thresh]        \n\t"  // Load scaled threshold into t1
    "li %[flag], 0           \n\t"  // Default: Normal (0)
    "blt t0, t1, finished    \n\t"  // If t0 < t1, skip to finished
    "li %[flag], 1           \n\t"  // If t0 >= t1, set flag to 1
    "finished:               \n\t"
    : [flag] "=r"(is_critical)      // Output: maps to is_critical
    : [curr] "r"(scaled_temp),      // Input: current temperature
      [thresh] "r"(scaled_thresh)   // Input: threshold
    : "t0", "t1");                  // Clobber list

  // Serial Output
  Serial.print("Current Temp: ");
  Serial.print(temperature, 2);
  Serial.print(" C | Humidity: ");
  Serial.print(humidity, 2);
  Serial.println(" %");

  /* --- HARDWARE INDICATION --- */
  if (is_critical) {
    digitalWrite(LED_WARNING, HIGH); // Red ON
    Serial.println(">> STATUS: [!!! CRITICAL !!!]");
    Serial.println(">> ALERT: High temp warning.");
  } else {
    Serial.println(">> STATUS: [NORMAL]");
  }

  Serial.println("-------------------------------");
  delay(1000); // 2-second delay for DHT11 stability
}