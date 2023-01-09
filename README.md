# git-IV-JF
Obvod má slúžiť na ovládanie elektromagnetických ventilov pomocou relé na základe porovnania nameranej a nastavenej teploty a spínanie vonkajšieho osvetlenia na základe intenzity osvetlenia. Pripojenie k obvodu je cez vlastnú bezdrôtovú sieť a ovládanie pomocou internetového prehliadača.  Obvod môže pracovať v automatickom režime, alebo môže byť ovládaný manuálne.

Použité súčiastky:
vývojová doska NODE MCU ESP32 DEVKITV1,
4 kanálové relé  – 1 ks,
snímač teploty DS18B20 – 3 ks,
fotorezistor - 1 ks,
rezistor 10 kOhm – 1 ks,
rezistor 4,7 kOhm – 1 ks,
vodiče,
kontaktné pole.

Relé pripojíme k VCC, GND a ESP32 pomocou vodičov prepojíme k piny ESP32-Relé, D25-IN1,D26-IN2,D27-IN3,D33-IN4. Ak chceme aby boli relé napájané iným zdrojom napätia rozpojíme spojku JD-VCC na doske relé. 
Použité prostredie: ARDUINO IDE 2.0.2
Doska: ESP32 Dev Module
Popis programu:
Relé osvetlenia je v programe nastavené na pin D33 a relé je K4. 
Použité knižnice: <OneWire.h>, <DallasTemperature.h>, <WiFi.h>
Využitý Wifi server nastavený na:
ssid     = "DodoESP32"; password = "Abcd1234"; 
IP adresa je ponechaná predvolená 192.168.4.1 port je však zmenený na :4321;
Na spojenie zo snímačmi teploty je využitý onewire od Dallas Semiconductor na pine 4. Adresy snímačov sú zadané priamo v programe.
Požadovaná teplota aj hodnota pre osvetlenie je zadaná priamo v programe. 
Pre režim auto program pre využíva timer ESP32 80MHz a načíta hodnoty zo snímačov každých 10s. Pre premennú časovača je využitá časť pamäte pre inštrukcie IRAM. Podľa požadovaných hodnôt teploty a osvetlenia zapne, alebo vypne príslušné relé.

Zdroje:
https://arduinoposlovensky.sk/projekty/dallas-ds18b20/
https://www.donskytech.com/read-ldr-photoresistor-sensor-using-esp32/
http://www.iotsharing.com/2017/06/how-to-use-interrupt-timer-in-arduino-esp32.html
https://randomnerdtutorials.com/esp32-web-server-arduino-ide/
https://esp32.com/viewtopic.php?t=26225
