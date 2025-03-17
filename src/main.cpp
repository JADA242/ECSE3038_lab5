#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "env.h"  


LiquidCrystal_I2C LCD(0x27, 16, 2);

void spinner() {
    static int8_t counter = 0;
    const char* glyphs = "\xa1\xa5\xdb";
    LCD.setCursor(15, 1);
    LCD.print(glyphs[counter++]);
    if (counter == strlen(glyphs)) {
        counter = 0;
    }
}

void setup() {
    Serial.begin(115200);

    
    LCD.init();
    LCD.backlight();
    LCD.setCursor(0, 0);
    LCD.print("Connecting to");
    LCD.setCursor(0, 1);
    LCD.print("WIFI...");

    if (IS_WOKWI) 
        WiFi.begin(SSID, PASS, CHANNEL);
    else 
        WiFi.begin(SSID, PASS);

   
    while (WiFi.status() != WL_CONNECTED) {
        delay(250);
        spinner();
    }

    Serial.println("WIFI connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("Online");
    LCD.setCursor(0, 1);
    LCD.print("Getting text...");
}

void loop() {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(String(ENDPOINT));
        http.addHeader("api-key", API_KEY);  

        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);

            String responseBody = http.getString();
            Serial.println(responseBody);

           
            JsonDocument object;

            DeserializationError error = deserializeJson(object, responseBody);

            if (error) {
                Serial.print("deserializeJson() failed: ");
                Serial.println(error.c_str());
                return;
            }

            const char* line1 = object["line_1"];
            const char* line2 = object["line_2"];

            
            LCD.clear();
            LCD.setCursor(0, 0);
            LCD.print(line1);
            LCD.setCursor(0, 1);
            LCD.print(line2);
        } else {
            Serial.print("Failed HTTP request.");
            Serial.println(httpResponseCode);
        }

        http.end();
    }

    delay(5000);  
}  
