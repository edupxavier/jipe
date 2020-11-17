/* Leitura da RPM a partir do polo negativo da bobina
 * 
 */
 
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


const short DISTRIBUIDOR_GND_PIN = 18;
const short INTERVALO_AMOSTRAGEM = 500;   // ms
const short CILINDROS = 6;
unsigned long distribuidorPulsos = 0;


// interrupção quando o distribuidor pulsa negativo
void IRAM_ATTR isr() {
  distribuidorPulsos++;
}




void setup() {
  // configura display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
  }
  display.cp437(true);
  display.setTextColor(SSD1306_WHITE);


  
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(DISTRIBUIDOR_GND_PIN, INPUT_PULLUP);

  attachInterrupt(DISTRIBUIDOR_GND_PIN, isr, RISING);

  Serial.begin(115200);
}



unsigned long int ultimaAmostra = 0;
unsigned long int ultimaPulsos = 0;
unsigned long int nAmostra = 0;

void loop() {
//  digitalWrite(LED_BUILTIN, HIGH); delay(250);
//  digitalWrite(LED_BUILTIN, LOW);  delay(250);

  if ((ultimaAmostra + INTERVALO_AMOSTRAGEM) <= millis()) {
    int pulsos = (distribuidorPulsos - ultimaPulsos);
    int rpm = 0;
    int intervalo = millis() - ultimaAmostra;
    nAmostra++;
    if ((pulsos != 0) && (intervalo != 0)) { 
       rpm = 60000 * pulsos / intervalo / CILINDROS * 2;
    }

    Serial.printf("#%i   rpm = %i   pulsos = %u\n", nAmostra, rpm, pulsos);
       
    ultimaAmostra = millis();
    ultimaPulsos = distribuidorPulsos;

    displayRPM(rpm);
  }
}




void displayRPM(int rpm) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("RPM");

  display.setTextSize(3);
  display.setCursor(15, 12);
  display.printf("%i", rpm);

  display.display();
}
