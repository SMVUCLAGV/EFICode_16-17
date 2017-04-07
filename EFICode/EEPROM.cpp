#include "constants.h"
#include "EEPROM.h"

const int VALUESIZE = 4;

union tableValue {
	char bytes[VALUESIZE];
	double value;
};

//Calculates the EEPROM index for the first byte of a given table value
int Controller::calculateEEPROMIndex(int row, int col) {
	if (row < 0 || col < 0 || row > maxTableRowIndex || col > maxTableColIndex) {
		return -1;
	}
	return (row * numTableCols * VALUESIZE) + (col * VALUESIZE);
}

//writes a specified fuel ratio table entry to EEPROM
void Controller::writeTableValue(int row, int col) {
	if (row < 0 || col < 0 || row > maxTableRowIndex || col > maxTableColIndex) {
		return;
	}
	int EEPROMIndex = calculateEEPROMIndex(row, col);
	tableValue currentValue;
	currentValue.value = fuelRatioTable[row][col];
	for (int i = 0; i < VALUESIZE; ++i)
	{
		EEPROM.update(EEPROMIndex + i, currentValue.bytes[i]);
	}
}

//reads a specified fuel ratio table entry from EEPROM
void Controller::readTableValue(int row, int col) {
	if (row < 0 || col < 0 || row > maxTableRowIndex || col > maxTableColIndex) {
		return;
	}
	int EEPROMIndex = calculateEEPROMIndex(row, col);
	tableValue currentValue;
	for (int i = 0; i < VALUESIZE; ++i)
	{
		currentValue.bytes[i] = EEPROM.read(a + i);
	}
	fuelRatioTable[row][col] = currentValue.value;
}