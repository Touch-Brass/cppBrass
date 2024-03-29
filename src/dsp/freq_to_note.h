#pragma once

#include <string>

const std::string NOTES[12] = { "A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#" };
const int8_t OCTAVES[12] = { 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5 };

std::pair<std::string, int> freq_to_note(double freq);