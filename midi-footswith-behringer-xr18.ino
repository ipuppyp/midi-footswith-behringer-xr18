const int PIN_2 = 2;
const int PIN_3 = 3;
const int PIN_4 = 4;
const int PIN_5 = 5;
const int PIN_6 = 6;
const int PIN_12 = 12;

const int BUTTON_PRESSED_TRESHOLD = 10;
const int TAP_DELAY_TRESHOLD = 3000;
const int MIDI_PROGRAM_CHANGE_START = 175;

const int MIDI_CHANNEL_2 = 2;
const int MIDI_FX_2_ADDRESS = 18;
const int MIDI_FX_4_ADDRESS = 20;

const long TAP_LED_ON_LENGTH = 50;

int ledVal = HIGH;
int midiVal = 127;

long previousMillis = 0;
long currentMillis = 0;
long diff = 2000;

int thous;
int huns;
int tens;
int units;

long tapLedSwithedOnTime = 0;
long tapLedSwithedOn = false;

void setup() {
  pinMode(PIN_12, OUTPUT);
  pinMode(PIN_4, OUTPUT);
  pinMode(PIN_5, OUTPUT);
  pinMode(PIN_6, OUTPUT);
  pinMode(PIN_2, INPUT_PULLUP);
  pinMode(PIN_3, INPUT_PULLUP);
  digitalWrite(PIN_5, HIGH);
  Serial.begin(31250);
}

void loop() {
  currentMillis = millis();

  handleFxMute();
  handleTapDelay();
  handleTapDelayBlinking();
  delay(10);

}

void handleFxMute() {
  if (digitalRead(PIN_2) == LOW) {
    midiVal = ledVal == HIGH ? 127 : 0;
    sendMidiCc(MIDI_CHANNEL_2, MIDI_FX_2_ADDRESS, midiVal);
    sendMidiCc(MIDI_CHANNEL_2, MIDI_FX_4_ADDRESS, midiVal);

    digitalWrite(PIN_4, ledVal);
    ledVal = ledVal == HIGH ? LOW : HIGH;
    digitalWrite(LED_BUILTIN, ledVal);
    digitalWrite(PIN_5, ledVal);

    waitTillButtonPressed(PIN_2);
  }
}
void handleTapDelay() {
  if (digitalRead(PIN_3) == LOW) {
    diff  =   currentMillis - previousMillis;

    if (diff < TAP_DELAY_TRESHOLD) {
      thous = 48 + diff / 1000;
      huns = 48 + diff / 100 % 10;
      tens = 48 + diff / 10 % 10;
      units = 48 + diff % 10;

      // message: /fx/4/par/02 + delay time
	  // SysEx message is generated with "Sysex Osc Generator"
      uint8_t fx_4_par_02_msg[23] = {0xF0, 0x00, 0x20, 0x32, 0x32, 0x2F, 0x66, 0x78, 0x2F, 0x34, 0x2F, 0x70, 0x61, 0x72, 0x2F, 0x30, 0x32, 0x20, thous, huns, tens, units, 0xF7};
      Serial.write(fx_4_par_02_msg, sizeof(fx_4_par_02_msg));

    }

    waitTillButtonPressed(PIN_3);
    previousMillis = currentMillis;
  }
}

void handleTapDelayBlinking() {

  if (!tapLedSwithedOn && tapLedSwithedOnTime + diff < currentMillis) {
    digitalWrite(PIN_6, HIGH);
    tapLedSwithedOnTime = currentMillis;
    tapLedSwithedOn = true;
  }
  else if (tapLedSwithedOn && tapLedSwithedOnTime + TAP_LED_ON_LENGTH < currentMillis) {
    digitalWrite(PIN_6, LOW);
    tapLedSwithedOn = false;
  }
}

void sendMidiCc(int channel, int command, int value) {
  Serial.write(MIDI_PROGRAM_CHANGE_START + channel);
  Serial.write(command);
  Serial.write(value);
}

void waitTillButtonPressed(int input)  {
  while (digitalRead(input) == LOW) {
    delay(BUTTON_PRESSED_TRESHOLD);
  }
}

