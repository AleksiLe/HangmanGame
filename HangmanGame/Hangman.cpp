#include "Hangman.h"

Hangman::Hangman() : wrongGuesses(0) {}

void Hangman::Initialize(const std::wstring& word) {
    wordToGuess = word;
    guessedLetters.assign(wordToGuess.length(), false);
    guessedCharacters.clear();
    wrongGuesses = 0;
}

bool Hangman::Guess(wchar_t letter) {
    bool guessCorrect = false;
    for (size_t i = 0; i < wordToGuess.length(); ++i) {
        if (wordToGuess[i] == letter) {
            guessedLetters[i] = true;
            guessCorrect = true;
        }
    }
    if (!guessCorrect) {
        wrongGuesses++;
    }
    return guessCorrect;
}

bool Hangman::IsGameOver() const {
    return wrongGuesses > maxWrongGuesses;
}

bool Hangman::IsGameWon() const {
    return std::all_of(guessedLetters.begin(), guessedLetters.end(), [](bool v) { return v; });
}

std::wstring Hangman::GetDisplayWord() const {
    std::wstring displayWord;
    for (size_t i = 0; i < wordToGuess.length(); ++i) {
        if (guessedLetters[i]) {
            displayWord += wordToGuess[i];
        }
        else {
            displayWord += L'_';
        }
        displayWord += L' ';
    }
    return displayWord;
}

int Hangman::GetWrongGuessCount() const {
    return wrongGuesses;
}

std::wstring Hangman::GetGuessedCharacters() const {
    return guessedCharacters;
}

void Hangman::AddGuessedCharacters(wchar_t letter) {
    guessedCharacters += letter;
}