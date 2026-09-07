/**
 * 10_Buzzer
 *
 * Play tones with the Melo buzzer.
 *
 * Expected behavior:
 *   - Play a simple melody
 *   - Pause
 *   - Repeat
 */

#include <Melo.h>

// Note frequencies (Hz)
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523

void setup() {
    Melo.begin();
}

void loop() {
    // Play a simple ascending scale
    Serial.println("Playing ascending scale...");

    int notes[] = { NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5 };
    int noteCount = 8;

    for (int i = 0; i < noteCount; i++) {
        Melo.beep(notes[i], 200);  // Each note 200ms
        delay(50);                 // 50ms gap between notes
    }

    delay(1000);  // Pause before repeating

    // Play a single long tone
    Serial.println("Playing single tone (880Hz, 500ms)...");
    Melo.beep(880, 500);

    delay(2000);
}
