/*
 * ============================================================
 *   METHANE GAS DETECTION & ALERT SYSTEM
 *   Hardware: Arduino Uno + MQ-9 + LCD + GSM + Buzzer + LED
 *   Author  : [Aarav Rasquinha]
 *   Version : 1.0.0
 * ============================================================
 */

#include <LiquidCrystal_I2C.h>   
#include <SoftwareSerial.h>       

#define MQ9_ANALOG_PIN   A0    
#define BUZZER_PIN        8   
#define LED_PIN           7   


#define GSM_TX_PIN        
#define GSM_RX_PIN        
SoftwareSerial gsmSerial(GSM_RX_PIN, GSM_TX_PIN);


LiquidCrystal_I2C lcd(0x27, 16, 2);


#define WARN_THRESHOLD   8    
#define ALERT_THRESHOLD  10   

const char ALERT_NUMBER[] = "+91XXXXXXXXXX";  


const float R0 = 10.0;         
const float RL = 10.0;          

bool smsSent        = false;   
unsigned long lastSMSTime = 0;
const unsigned long SMS_COOLDOWN = 60000UL; 


float readGasPercent();
void  updateLCD(float pct);
void  handleAlerts(float pct);
void  sendSMS(const char* msg);
void  gsmInit();


void setup() {
  Serial.begin(9600);
  gsmSerial.begin(9600);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN,    OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN,    LOW);

  // LCD init
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("  GAS DETECTOR  ");
  lcd.setCursor(0, 1);
  lcd.print(" Initialising.. ");
  delay(2000);
  lcd.clear();

  // GSM init
  gsmInit();

  Serial.println(F("System ready."));
}


void loop() {
  float gasPct = readGasPercent();

  updateLCD(gasPct);
  handleAlerts(gasPct);

  // Serial debug
  Serial.print(F("GAS Level: "));
  Serial.print(gasPct, 1);
  Serial.println(F(" %"));

  delay(1000);   // 
}

float readGasPercent() {
  int   raw   = analogRead(MQ9_ANALOG_PIN);
  float vout  = raw * (5.0 / 1023.0);          
  float rs    = ((5.0 - vout) / vout) * RL;    
  float ratio = rs / R0;                        
  float ppm    = 1000.0 * pow(ratio / 6.4, 1.0 / -0.38);
  float pctLEL = (ppm / 50000.0) * 100.0;

  if (pctLEL < 0.0)   pctLEL = 0.0;
  if (pctLEL > 100.0) pctLEL = 100.0;

  return pctLEL;
}

void updateLCD(float pct) {
  lcd.setCursor(0, 0);
  lcd.print("GAS Level:");
  lcd.print((int)pct);
  lcd.print(" %  ");   

  lcd.setCursor(0, 1);
  if (pct >= ALERT_THRESHOLD) {
    lcd.print("!! DANGER !!    ");
  } else if (pct >= WARN_THRESHOLD) {
    lcd.print("WARNING         ");
  } else {
    lcd.print("NORMAL          ");
  }
}
void handleAlerts(float pct) {
  unsigned long now = millis();

  if (pct >= ALERT_THRESHOLD) {
   
    digitalWrite(BUZZER_PIN, HIGH);
   
    digitalWrite(LED_PIN, HIGH);

    if (!smsSent || (now - lastSMSTime >= SMS_COOLDOWN)) {
      sendSMS("ALERT: Methane level CRITICAL! Immediate evacuation required.");
      smsSent    = true;
      lastSMSTime = now;
    }

  } else if (pct >= WARN_THRESHOLD) {
    // — Buzzer OFF, LED ON (warning) —
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, HIGH);

    // — SMS warning (once per event) —
    if (!smsSent || (now - lastSMSTime >= SMS_COOLDOWN)) {
      sendSMS("WARNING: Methane level elevated. Please check area.");
      smsSent    = true;
      lastSMSTime = now;
    }

  } else {
    // — All clear —
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN,    LOW);
    smsSent = false;   
  }
}

void sendSMS(const char* msg) {
  Serial.print(F("Sending SMS: "));
  Serial.println(msg);

  gsmSerial.println(F("AT+CMGF=1"));   
  delay(500);
  gsmSerial.print(F("AT+CMGS=\""));
  gsmSerial.print(ALERT_NUMBER);
  gsmSerial.println(F("\""));
  delay(500);
  gsmSerial.print(msg);
  delay(200);
  gsmSerial.write(26);                 
  delay(3000);
}

void gsmInit() {
  lcd.clear();
  lcd.print("GSM Init...");
  delay(1000);

  gsmSerial.println(F("AT"));          
  delay(500);
  gsmSerial.println(F("AT+CMGF=1"));  
  delay(500);
  gsmSerial.println(F("AT+CNMI=1,2,0,0,0")); 
  delay(500);

  lcd.clear();
  lcd.print("GSM Ready");
  delay(1000);
  lcd.clear();
}
