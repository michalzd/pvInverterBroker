# pvInverterBroker

w katalogu dist/Release s¹ wersje skompilowane na 
 Raspberry Pi 1 (wszystkie wersje z Broadcom BCM2835, CPU ARM11  ) 
 Raspberry Pi 3 (Broadcom BCM2837, CPU ARM-8 Cortex-A53), dzia³a równie¿ na Pi 4

Potrzebna jest do tego biblioteka https://github.com/michalzd/MQTT_LinuxLib
równie¿ w katalogu dist/Release s¹ wersje skompilowane na Raspberry Pi.


Binarka biblioteki powinna znaleŸæ siê w katalogu /usr/lib
Binarka pvInverterBroker mo¿e byæ w dowolnym katalogu.
Przyk³adowy plik konfiguracyjny: https://github.com/michalzd/pvInverterBroker/blob/mqtt/etc/inverterbroker
Wprowadziæ w nim odpowiednie dane:
 -dostêp do loggera falownika
 -adres serwera mqtt
 -mo¿na zmieniæ tematy wiadomoœci mqtt

uruchomienie brokera z parametrem --d  pozwala œledziæ dzia³anie w terminalu,
bez tego parametru status programu jest w syslog.

kompilacja wymaga instalacji kernel headers :

sudo apt install linux-headers-$(uname -r)	 


