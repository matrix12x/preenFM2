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

#ifndef SYNTHSTATUS_H_
#define SYNTHSTATUS_H_

#include "Common.h"
#include "EncodersListener.h"
#include "SynthParamListener.h"
#include "SynthParamChecker.h"
#include "SynthMenuListener.h"
#include "PresetUtil.h"
#include "Menu.h"
#include "Storage.h"

#define BUTTON_SYNTH  0
#define BUTTON_OSC    1
#define BUTTON_ENV    2
#define BUTTON_MATRIX 3
#define BUTTON_LFO    4

#define BUTTON_BACK         5
#define BUTTON_MENUSELECT   6

#define BUTTON_DUMP   0


#define NUMBER_OF_ENCODERS 4
#define NUMBER_OF_BUTTONS 7


enum {
    ENCODER_ENGINE_ALGO = 0,
    ENCODER_ENGINE_VELOCITY,
    ENCODER_ENGINE_VOICE,
    ENCODER_ENGINE_GLIDE
};

enum {
    ENCODER_ENGINE_IM1 = 0,
    ENCODER_ENGINE_IM2,
    ENCODER_ENGINE_IM3,
    ENCODER_ENGINE_IM4
};

enum {
    ENCODER_ENGINE_IM5 = 0,
    ENCODER_ENGINE_IM6,
};


enum {
    ENCODER_ENGINE_MIX1 = 0,
    ENCODER_ENGINE_PAN1,
    ENCODER_ENGINE_MIX2,
    ENCODER_ENGINE_PAN2,
};

enum {
    ENCODER_ENGINE_MIX3 = 0,
    ENCODER_ENGINE_PAN3,
    ENCODER_ENGINE_MIX4,
    ENCODER_ENGINE_PAN4
};

enum {
    ENCODER_PERFORMANCE_CC1 = 0,
    ENCODER_PERFORMANCE_CC2,
    ENCODER_PERFORMANCE_CC3,
    ENCODER_PERFORMANCE_CC4
};

enum {
    ENCODER_OSC_SHAP = 0,
    ENCODER_OSC_FTYPE,
    ENCODER_OSC_FREQ,
    ENCODER_OSC_FTUNE
};

enum {
    ENCODER_ENV_A = 0,
    ENCODER_ENV_D,
    ENCODER_ENV_S,
    ENCODER_ENV_R
};

enum {
    ENCODER_MATRIX_SOURCE = 0,
    ENCODER_MATRIX_MUL,
    ENCODER_MATRIX_DEST,
};

enum {
    ENCODER_LFO_SHAPE = 0,
    ENCODER_LFO_FREQ,
    ENCODER_LFO_BIAS,
    ENCODER_LFO_KSYNC
};

enum {
    ENCODER_LFO_ENV2_SILENCE = 0,
    ENCODER_LFO_ENV2_ATTACK,
    ENCODER_LFO_ENV2_RELEASE,
    ENCODER_LFO_ENV2_LOOP
};


enum {
    ENCODER_STEPSEQ_BPM = 0,
    ENCODER_STEPSEQ_GATE
};


typedef unsigned char uchar;

enum Algorithm {
	ALGO1 = 0,
	ALGO2,
	ALGO3,
	ALGO4,
	ALGO5,
    ALGO6,
    ALGO7,
    ALGO8,
    ALGO9,
    ALG10,
    ALG11,
    ALG12,
    ALG13,
    ALG14,
    ALG15,
    ALG16,
    ALG17,
    ALG18,
    ALG19,
    ALG20,
    ALG21,
    ALG22,
    ALG23,
    ALG24,
    ALG25,
    ALG26,
    ALG27,
	ALGO_END
};



enum OscShape {
	OSC_SHAPE_SIN = 0,
    OSC_SHAPE_SAW,
    OSC_SHAPE_SQUARE,
    OSC_SHAPE_SIN_SQUARE,
    OSC_SHAPE_SIN_ZERO,
    OSC_SHAPE_SIN_POS,
    OSC_SHAPE_RAND,
	OSC_SHAPE_OFF,
	OSC_SHAPE_LAST
};

enum LfoType {
	LFO_SIN = 0,
	LFO_RAMP,
	LFO_SAW,
	LFO_SQUARE,
	LFO_RANDOM,
	LFO_TYPE_MAX
};


enum OscFrequencyType {
	OSC_FT_KEYBOARD = 0,
	OSC_FT_FIXE
};

enum OscEnv2Loop {
    LFO_ENV2_NOLOOP = 0,
    LFO_ENV2_LOOP_SILENCE,
    LFO_ENV2_LOOP_ATTACK
};



// Display information


struct ParameterRowDisplay {
	const char* rowName;
	const char* paramName[4];
	struct ParameterDisplay params[4];
};


struct AllParameterRowsDisplay {
	struct ParameterRowDisplay* row[NUMBER_OF_ROWS];
};


// Class define to allow initalization
enum EventType {
    MIDI_NOTE_OFF = 0x80,
    MIDI_NOTE_ON = 0x90,
    MIDI_POLY_AFTER_TOUCH = 0xA0,
    MIDI_CONTROL_CHANGE = 0xb0,
    MIDI_PROGRAM_CHANGE =0xc0,
    MIDI_AFTER_TOUCH = 0xd0,
    MIDI_PITCH_BEND = 0xe0,
    MIDI_REAL_TIME_EVENT = 0xf0,
    MIDI_SYSEX = 0xf0,
    MIDI_SONG_POSITION = 0xf2,
    MIDI_START = 0xfa,
    MIDI_CLOCK = 0xf8,
    MIDI_CONTINUE = 0xfb,
    MIDI_STOP = 0xfc,
    MIDI_SYSEX_END = 0xf7
};

enum EventState {
	MIDI_EVENT_WAITING = 0,
    MIDI_EVENT_IN_PROGRESS ,
    MIDI_EVENT_SYSEX ,
	MIDI_EVENT_COMPLETE
};

struct MidiEventState {
    EventState eventState;
    unsigned char numberOfBytes;
    unsigned char index;
};

struct MidiEvent {
	unsigned char channel;
	EventType eventType;
	unsigned char value[2];
};

class Hexter;

class SynthState : public EncodersListener {
public:
	SynthState();

	void setStorage(Storage* storage) {
	    this->storage = storage;
	}
	void setHexter(Hexter* hexter) {
	    this->hexter = hexter;
	}
	void encoderTurned(int encoder, int ticks);
	void buttonPressed(int button);
	void twoButtonsPressed(int button1, int button2);
	void encoderTurnedWhileButtonPressed(int encoder, int ticks, int button);
	void encoderTurnedForStepSequencer(int row, int num, int ticks);

	static SynthParamType getListenerType(int row) {
		if (row>= ROW_ENGINE_FIRST && row<=ROW_ENGINE_LAST) {
			return SYNTH_PARAM_TYPE_ENGINE;
		} else if (row>= ROW_OSC_FIRST && row<=ROW_OSC_LAST) {
			return SYNTH_PARAM_TYPE_OSC;
        } else if (row>= ROW_ENV_FIRST && row<=ROW_ENV_LAST) {
			return SYNTH_PARAM_TYPE_ENV;
        } else if (row>= ROW_MATRIX_FIRST && row<=ROW_MATRIX_LAST) {
			return SYNTH_PARAM_TYPE_MATRIX;
        } else if (row>= ROW_LFO_FIRST && row<=ROW_LFO_LAST) {
			return SYNTH_PARAM_TYPE_LFO;
		}
		return SYNTH_PARAM_TYPE_INVALID;
	}

	void changeSynthModeRow(int button, int step);
	void setNewValue(int timbre, int row, int encoder, float newValue);
	void setNewStepValue(int timbre, int whichStepSeq, int step, int newValue);


	const MenuItem* afterButtonPressed();
	const MenuItem* menuBack();

	int getCurrentRow() {
		return currentRow;
	}


	void insertParamListener(SynthParamListener *listener) {
		if (firstParamListener!=0) {
			listener->nextListener = firstParamListener;
		}
		firstParamListener = listener;
	}

    void insertParamChecker(SynthParamChecker *checker) {
        if (firstParamChecker!=0) {
            checker->nextChecker = firstParamChecker;
        }
        firstParamChecker = checker;
    }

    void insertMenuListener(SynthMenuListener *listener) {
		if (firstMenuListener!=0) {
			listener->nextListener = firstMenuListener;
		}
		firstMenuListener = listener;
	}

	void propagateNewSynthMode() {
    	propagateNoteOff();
		for (SynthMenuListener* listener = firstMenuListener; listener !=0; listener = listener->nextListener) {
			listener->newSynthMode(&fullState);
		}
	}

	void propagateMenuBack(enum MenuState oldMenuState) {
		for (SynthMenuListener* listener = firstMenuListener; listener !=0; listener = listener->nextListener) {
			listener->menuBack(oldMenuState, &fullState);
		}
	}

	void propagateNewMenuState() {
		for (SynthMenuListener* listener = firstMenuListener; listener !=0; listener = listener->nextListener) {
			listener->newMenuState(&fullState);
		}
	}

	void propagateNewMenuSelect() {
		for (SynthMenuListener* listener = firstMenuListener; listener !=0; listener = listener->nextListener) {
			listener->newMenuSelect(&fullState);
		}
	}

    void propagateNewMidiConfig(int menuSelect, char newValue) {
        for (SynthParamListener* listener = firstParamListener; listener !=0; listener = listener->nextListener) {
            listener->newMidiConfig(menuSelect, newValue);
        }
    }

    void propagateNewParamValue(int timbre, int currentRow, int encoder, ParameterDisplay* param, float oldValue, float newValue) {
		for (SynthParamListener* listener = firstParamListener; listener !=0; listener = listener->nextListener) {
			listener->newParamValue(timbre, getListenerType(currentRow), currentRow, encoder, param, oldValue, newValue);
		}
	}

    void propagateNewParamCheck(int encoder, float oldValue, float *newValue) {
        for (SynthParamChecker* checker = firstParamChecker; checker !=0; checker = checker->nextChecker) {
            checker->checkNewParamValue(currentTimbre, currentRow, encoder, oldValue, newValue);
        }
    }

	void propagateNewPresetName(bool cleanFirst) {
		for (SynthParamListener* listener = firstParamListener; listener !=0; listener = listener->nextListener) {
			listener->newPresetName(cleanFirst);
		}
	}

	void propagateNewParamValueFromExternal(int timbre, int currentRow, int encoder, ParameterDisplay* param, float oldValue, float newValue) {
		for (SynthParamListener* listener = firstParamListener; listener !=0; listener = listener->nextListener) {
			listener->newParamValueFromExternal(timbre, getListenerType(currentRow), currentRow, encoder, param, oldValue, newValue);
		}
	}

	void propagateNewCurrentRow(int newCurrentRow) {
		for (SynthParamListener* listener = firstParamListener; listener !=0; listener = listener->nextListener) {
			listener->newcurrentRow(currentTimbre, newCurrentRow);
		}
	}


	void propagateBeforeNewParamsLoad() {
        for (SynthParamListener* listener = firstParamListener; listener !=0; listener = listener->nextListener) {
            listener->beforeNewParamsLoad(currentTimbre);
        }
    }

    bool canPlayNote() {
    	return (fullState.synthMode == SYNTH_MODE_EDIT)
    			|| (fullState.currentMenuItem->menuState == MENU_LOAD_SELECT_BANK_PRESET)
    			||  (fullState.currentMenuItem->menuState == MENU_LOAD_SELECT_DX7_PRESET);
    }

	void propagateShowAlgo() {
		for (SynthParamListener* listener = firstParamListener; listener !=0; listener = listener->nextListener) {
			listener->showAlgo();
		}
	}

    void propagateNoteOff() {
    	if (this->isPlayingNote && canPlayNote()) {
			for (SynthParamListener* listener = firstParamListener; listener !=0; listener = listener->nextListener) {
				listener->stopNote(playingTimbre, playingNote);
			}
			this->isPlayingNote = false;
    	}
    }

    void propagateNoteOn(int shift) {
    	if (canPlayNote()) {
			if (this->isPlayingNote && (this->playingNote == fullState.midiConfigValue[MIDICONFIG_TEST_NOTE] + shift)) {
				propagateNoteOff();
			} else {
				if (this->isPlayingNote) {
					propagateNoteOff();
				}
				playingNote = fullState.midiConfigValue[MIDICONFIG_TEST_NOTE] + shift ;
				playingTimbre = currentTimbre;
				for (SynthParamListener* listener = firstParamListener; listener !=0; listener = listener->nextListener) {
					listener->playNote(playingTimbre, playingNote, fullState.midiConfigValue[MIDICONFIG_TEST_VELOCITY]);
				}
				this->isPlayingNote = true;
			}
    	}
    }

    void propagateAfterNewParamsLoad();
    void propagateAfterNewComboLoad();
    void propagateNewTimbre(int timbre);

    SynthEditMode getSynthMode() {
		return fullState.synthMode;
	}

    int getOperatorNumber() {
    	return operatorNumber;
    }

    void newSysexBankReady();

    void tempoClick();

    void setParamsAndTimbre(struct OneSynthParams *newParams, int newCurrentTimbre);

    void resetDisplay();

    bool getIsPlayingNote() {
    	return isPlayingNote;
    }

    int currentTimbre;
    struct OneSynthParams *params;
    struct OneSynthParams backupParams;
	struct FullState fullState;
	char stepSelect[2];

private:
	int getRowFromOperator();
	bool isCurrentRowAvailable();
	bool isEnterNameState(int currentItme);
	char engineRow, oscillatorRow, envelopeRow, matrixRow, lfoRow;
	char operatorNumber, operatorView;
	char currentRow;
	bool isPlayingNote ;
	char playingNote;
	char playingTimbre;

	// Done menu temporisation
	unsigned int doneClick;

	SynthParamListener* firstParamListener;
	SynthMenuListener* firstMenuListener;
    SynthParamChecker* firstParamChecker;


    int getLength(const char *str) {
        int length = 0;
        for (const char *c = str; *c != '\0'; c++) {
            length++;
        }
        return length;
    }

    Storage* storage;
    Hexter* hexter;
};

// Global structure used all over the code
extern struct AllParameterRowsDisplay allParameterRows;


#endif /* SYNTHSTATUS_H_ */
