#ifndef EEPROM_H
#define EEPROM_H
#include "Wire.h"

// Write up to 128 bytes of data to address. Excess values in data[] are
// discarded.
// Memory is divided into 128 byte pages. Writing across page boundaries
// overflows address register and starts writing again at first byte in
// page. Only use multiples of 128 for addresses to avoid this.
void EEPROMWrite(unsigned int block, unsigned int address, byte[] data) {
	Wire.beginTransmission(block);
	Wire.send(address >> 8);
	Wire.send(address & 0xFF);
	int i = 0;
	while(i != 128 && data[i] != ‘\0’) {
		Wire.send(data[i]);
		i++;
	}
	Wire.endTransmission(block);
}

// Reads 32 bytes starting from address. Can cross page boundaries
// safely.
void EEPROMRead(unsigned int block, unsigned int address) {
	Wire.beginTransmission(block);
	Wire.send(address >> 8);
	Wire.send(address & 0xFF);
	Wire.endTransmission(block);
	
	Wire.requestFrom(block, 32);
	while (Wire.available()) {
			Serial.write(Wire.read());
		}
}

// Reads 4 byte integer value from EEPROM.
int EEPROMReadInt(unsigned int block, unsigned int address) {
	Wire.beginTransmission(block);
	Wire.send(address >> 8);
	Wire.send(address & 0xFF);
	Wire.endTransmission(block);
	
	unsigned int val = 0;
	Wire.requestFrom(block, 4);
	while (Wire.available()) {
			val = (val << 4) & Wire.read();
	}
	return val;
}

#endif // EEPROM_H*/
