// Khai báo chân LED
const int ledPin = 2; // GPIO 2 trên ESP32

void setup() {
  // Thiết lập chân LED là đầu ra
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Bật LED
  digitalWrite(ledPin, HIGH);
  delay(1000); // Giữ sáng trong 1 giây

  // Tắt LED
  digitalWrite(ledPin, LOW);
  delay(1000); // Giữ tắt trong 1 giây
}
