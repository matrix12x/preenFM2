
/*
 * Copyright 2013 Xavier Hosxe
 *
 * Author: Xavier Hosxe (xavier . hosxe (at) gmail . com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include "Storage.h"
#include "Menu.h"
#include "PresetUtil.h"

// Set param in memmory reachable with USB : static is OK
struct OneSynthParams reachableParam;
uint8_t dx7PackedPatch[DX7_PACKED_PATCH_SIZED];

void Storage::init(uint8_t*timbre1, uint8_t*timbre2, uint8_t*timbre3, uint8_t*timbre4) {
    timbre[0] = timbre1;
    timbre[1] = timbre2;
    timbre[2] = timbre3;
    timbre[3] = timbre4;
}

void Storage::copy(char* dest, const char* source, int length) {
    for (int k=0; k<length; k++) {
        dest[k] = source[k];
    }
}

void Storage::addNumber(char* name, int offset, int number) {
    int div = 100;
    while (div > 0) {
        int digit = number / div;
        name[offset++] = '0' + digit;
        number -= digit * div;
        div /= 10;
    }
    name[offset] = '\0';
}

void Storage::removeDefaultCombo() {
    remove(DEFAULT_COMBO);
}

void Storage::saveDefaultCombo() {
    // Default : no name
    // delete to be sure the size is OK...
    remove(DEFAULT_COMBO);
    for (int timbre = 0; timbre < 4; timbre++)  {
        for (int k=0; k<PFM_PATCH_SIZE; k++) {
            ((uint8_t*)&reachableParam)[k] = this->timbre[timbre][k];
        }
        save(DEFAULT_COMBO, ALIGNED_PATCH_SIZE * timbre,  (void*)&reachableParam, PFM_PATCH_SIZE);
    }
}

bool Storage::loadDefaultCombo() {
    // default : non ame

    if (checkSize(DEFAULT_COMBO) != (ALIGNED_PATCH_SIZE*3+PFM_PATCH_SIZE)) {
        return false;
    }

    for (int timbre = 0; timbre < 4; timbre++)  {
        int result = load(DEFAULT_COMBO,  ALIGNED_PATCH_SIZE * timbre ,  (void*)&reachableParam, PFM_PATCH_SIZE);

        if (result == 0) {
            for (int k=0; k<PFM_PATCH_SIZE; k++) {
               this->timbre[timbre][k] = ((uint8_t*)&reachableParam)[k];
            }
        }
    }
    return true;
}


void Storage::createPatchBank(const char* name) {
#ifndef BOOTLOADER
	const struct BankFile * newBank = addEmptyBank(name);
	const char* fullBankName = getPreenFMFullName(name);
	if (newBank == 0) {
		return;
	}
    for (int k=0; k<PFM_PATCH_SIZE; k++) {
        ((uint8_t*)&reachableParam)[k] = ((uint8_t*)&preenMainPreset)[k];
    }
    // back up name
    copy(reachableParam.presetName, "Preset 0\0\0\0\0\0", 12);
	for (int k=0; k<128; k++) {
        addNumber(reachableParam.presetName, 7, k + 1);
		savePreenFMPatch(newBank, k, &reachableParam);
	}
#endif
}


void Storage::createComboBank(const char* name) {
#ifndef BOOTLOADER
	const struct BankFile * newBank = addEmptyCombo(name);
	const char* fullBankName = getPreenFMFullName(name);

	if (newBank == 0) {
		return;
	}
    for (int k=0; k<PFM_PATCH_SIZE; k++) {
        ((uint8_t*)&reachableParam)[k] = ((uint8_t*)&preenMainPreset)[k];
    }
	char comboName[12];
    copy(comboName,  "Combo \0\0\0\0\0\0", 12);
    copy(reachableParam.presetName, "Preset 0\0\0\0\0\0", 12);
	for (int comboNumber=0; comboNumber<128; comboNumber++) {

        addNumber(comboName, 6, comboNumber + 1);
		save(fullBankName, (ALIGNED_PATCH_SIZE * 4 + 12) * comboNumber , comboName, 12);

        for (int timbre = 0; timbre < 4; timbre++)  {
            addNumber(reachableParam.presetName, 7, timbre + 1);
            save(fullBankName, (ALIGNED_PATCH_SIZE * 4 + 12) * comboNumber +  12 + ALIGNED_PATCH_SIZE * timbre,  (void*)&reachableParam, PFM_PATCH_SIZE);
        }
	}

#endif
#ifdef DONOTEXISTATALL
	char comboName[12];
    copy(comboName,  "Combo \0\0\0\0\0\0", 12);

    for (int combo = 0; combo < 128; combo++) {
        addNumber(comboName, 6, combo + 1);
        saveCombo(combo, comboName);
    }
#endif
}

void Storage::loadConfig(char* midiConfig) {
    char* reachableProperties = (char*)&reachableParam;
    if (checkSize(PROPERTIES) != MIDICONFIG_SIZE) {
        return;
    }
    // Don't load in params directly because params is in CCM memory
    int result = load(PROPERTIES, 0,  reachableProperties, MIDICONFIG_SIZE);
    if (result == 0) {
        for (int k=0; k<MIDICONFIG_SIZE; k++) {
           midiConfig[k] = reachableProperties[k];
        }
    }
}

void Storage::saveConfig(const char* midiConfig) {
    char* reachableProperties = (char*)&reachableParam;
    for (int k=0; k<MIDICONFIG_SIZE; k++) {
        reachableProperties[k] = midiConfig[k];
    }
    // delete it so that we're sure the new one has the right size...
    remove(PROPERTIES);
    save(PROPERTIES, 0,  reachableProperties, MIDICONFIG_SIZE);
}

// NEW mechanism ===

uint8_t* Storage::dx7LoadPatch(const struct BankFile* bank, int patchNumber) {
	const char* fullBankName = getDX7BankFullName(bank->name);
    int result = load(fullBankName, patchNumber * DX7_PACKED_PATCH_SIZED + 6,  (void*)dx7PackedPatch, DX7_PACKED_PATCH_SIZED);
    if (result >0) {
    	return (uint8_t*)0;
    }
    return dx7PackedPatch;
}



void Storage::loadPreenFMPatch(const struct BankFile* bank, int patchNumber, struct OneSynthParams *params) {
	const char* fullBankName = getPreenFMFullName(bank->name);

    // Don't load in params directly because params is in CCM memory
    int result = load(fullBankName, patchNumber * ALIGNED_PATCH_SIZE,  (void*)&reachableParam, PFM_PATCH_SIZE);

    if (result == 0) {
        for (int k=0; k<PFM_PATCH_SIZE; k++) {
           ((uint8_t*)params)[k] = ((uint8_t*)&reachableParam)[k];
        }
    }
}

const char* Storage::loadPreenFMPatchName(const struct BankFile* bank, int patchNumber) {
	const char* fullBankName = getPreenFMFullName(bank->name);
    int result = load(fullBankName, ALIGNED_PATCH_SIZE * patchNumber + PFM_PATCH_SIZE - 16,  (void*)presetName, 12);
    presetName[12] = 0;
    return presetName;
}

void Storage::savePreenFMPatch(const struct BankFile* bank, int patchNumber, struct OneSynthParams *params) {
	const char* fullBankName = getPreenFMFullName(bank->name);

    char zeros[ALIGNED_PATCH_ZERO];
    for (int k=0; k<ALIGNED_PATCH_ZERO;k++) {
        zeros[k] = 0;
    }
    for (int k=0; k<PFM_PATCH_SIZE; k++) {
        ((uint8_t*)&reachableParam)[k] = ((uint8_t*)params)[k];
    }

    // Save patch
    save(fullBankName, patchNumber * ALIGNED_PATCH_SIZE,  (void*)&reachableParam, PFM_PATCH_SIZE);

    // Add zeros
    save(fullBankName, patchNumber * ALIGNED_PATCH_SIZE  + PFM_PATCH_SIZE,  (void*)zeros, ALIGNED_PATCH_ZERO);
}

void Storage::loadPreenFMCombo(const struct BankFile* combo, int comboNumber) {
	const char* fullBankName = getPreenFMFullName(combo->name);
    for (int timbre = 0; timbre < 4; timbre++)  {
        int result = load(fullBankName,  (ALIGNED_PATCH_SIZE * 4 + 12) * comboNumber +  12 + ALIGNED_PATCH_SIZE * timbre ,  (void*)&reachableParam, PFM_PATCH_SIZE);

        if (result == 0) {
            for (int k=0; k<PFM_PATCH_SIZE; k++) {
               this->timbre[timbre][k] = ((uint8_t*)&reachableParam)[k];
            }
        }
    }
}
const char* Storage::loadPreenFMComboName(const struct BankFile* combo, int comboNumber) {
	const char* fullBankName = getPreenFMFullName(combo->name);
    int result = load(fullBankName, (ALIGNED_PATCH_SIZE * 4 + 12) * comboNumber ,  (void*)presetName, 12);
    presetName[12] = 0;
    return presetName;
}

void Storage::savePreenFMCombo(const struct BankFile* combo, int comboNumber, char* comboName) {
	const char* fullBankName = getPreenFMFullName(combo->name);
    save(fullBankName, (ALIGNED_PATCH_SIZE * 4 + 12) * comboNumber ,  (void*)comboName, 12);

    for (int timbre = 0; timbre < 4; timbre++)  {
        for (int k=0; k<PFM_PATCH_SIZE; k++) {
            ((uint8_t*)&reachableParam)[k] = this->timbre[timbre][k];
        }
        save(fullBankName, (ALIGNED_PATCH_SIZE * 4 + 12) * comboNumber +  12 + ALIGNED_PATCH_SIZE * timbre,  (void*)&reachableParam, PFM_PATCH_SIZE);
    }
}



void Storage::saveBank(const char* newBankName, const uint8_t* sysexTmpMem) {
#ifndef BOOTLOADER
	const struct BankFile * newBank = addEmptyBank(newBankName);
	if (newBank == 0) {
		return;
	}
	for (int k=0; k<128; k++) {
		PresetUtil::convertCharArrayToSynthState(sysexTmpMem + PATCH_SIZE_PFM2 * k, &oneSynthParamsTmp);
		savePreenFMPatch(newBank, k, &oneSynthParamsTmp);
	}
#endif
}

int Storage::bankBaseLength(const char* bankName) {
	int k;
	for (k=0; k<8 && bankName[k]!=0 && bankName[k]!='.'; k++);
	return k;
}

bool Storage::bankNameExist(const char* bankName) {
	int nameLength = bankBaseLength(bankName);
	for (int b=0; getPreenFMBank(b)->fileType != FILE_EMPTY && b<NUMBEROFPREENFMBANKS; b++) {
		const struct BankFile* pfmb = getPreenFMBank(b);
		if (nameLength != bankBaseLength(pfmb->name)) {
			continue;
		}
		bool sameName = true;
		for (int n=0; n < nameLength && sameName; n++) {
			// Case insensitive...
			char c1 = bankName[n];
			char c2 = pfmb->name[n];
			if (c1 >= 'a' && c1<='z') {
				c1 = 'A' + c1 - 'a';
			}
			if (c2 >= 'a' && c2<='z') {
				c2 = 'A' + c2 - 'a';
			}
			if (c1 != c2) {
				sameName = false;
			}
		}
		if (sameName) {
			return true;
		}
	}
	return false;
}

bool Storage::comboNameExist(const char* comboName) {
	int nameLength = bankBaseLength(comboName);
	for (int b=0; getPreenFMCombo(b)->fileType != FILE_EMPTY && b<NUMBEROFPREENFMCOMBOS; b++) {
		const struct BankFile* pfmc = getPreenFMCombo(b);
		if (nameLength != bankBaseLength(pfmc->name)) {
			continue;
		}
		bool sameName = true;
		for (int n=0; n < nameLength && sameName; n++) {
			// Case insensitive...
			char c1 = comboName[n];
			char c2 = pfmc->name[n];
			if (c1 >= 'a' && c1<='z') {
				c1 = 'A' + c1 - 'a';
			}
			if (c2 >= 'a' && c2<='z') {
				c2 = 'A' + c2 - 'a';
			}
			if (c1 != c2) {
				sameName = false;
			}
		}
		if (sameName) {
			return true;
		}
	}
	return false;
}

