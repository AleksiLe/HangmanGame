#include <string>
#include <vector>
#include <algorithm>

class Hangman {
public:
    Hangman();

    void Initialize(const std::wstring& word);
    bool Guess(wchar_t letter);
    bool IsGameOver() const;
    bool IsGameWon() const;
    std::wstring GetDisplayWord() const;
    int GetWrongGuessCount() const;
    std::wstring GetGuessedCharacters() const;
    void AddGuessedCharacters(wchar_t letter);

private:
    std::wstring wordToGuess;
    std::vector<bool> guessedLetters;
    std::wstring guessedCharacters;
    int wrongGuesses;
    const int maxWrongGuesses = 7;
};