#include "freq_to_note.h"

#include <iostream>
#include <cmath>

std::pair<std::string, int> freq_to_note(double freq){
    double a4Ratio = freq / 440;
    double cents = 1200 * std::log2(a4Ratio);
    int note_difference = cents / 100;
    int cent_difference = (int)cents % 100;

    if(cent_difference > 50){
        note_difference++;
        cent_difference = cent_difference - 100;
    } else if (cent_difference < -50){
        note_difference--;
        cent_difference = 100 + cent_difference;
    }

    int octave_difference = note_difference / 12;
    note_difference = note_difference % 12;

    if(note_difference < 0){
        note_difference += 12;
        octave_difference--;
    }

    std::string note = NOTES[note_difference] + std::to_string(OCTAVES[note_difference] + octave_difference);
    return std::pair<std::string, int>(note, cent_difference);
}
