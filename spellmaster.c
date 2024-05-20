#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_SPELL_LENGTH 20
#define MAX_NUM_SPELLS 100
#define MAX_WORD_LENGTH 100
#define ALPHABET_SIZE 26

// Structure to store counts for each unique letter
typedef struct {
    int countFirst;
    int countLast;
} LetterCount;

// Function declarations
int isSpellAlreadyCast(char* spell, char** castSpells, int numCastSpells);
int areAvailableWords(char** spellList, int numSpells, char* previousSpell, LetterCount* letterCounts);
int wordExists(char** spellList, int numSpells, char* spell);
void updateLetterCounts(char* spell, LetterCount* letterCounts);
int getPlayerSpell(char* previousSpell, char** castSpells, int numCastSpells, char** spellList, int numSpells, char* playerSpell);
int getComputerSpell(char* previousSpell, char** castSpells, int numCastSpells, char** spellList, int numSpells, char* computerSpell, LetterCount* letterCounts);

int main() {
    FILE* file;
    char spell[MAX_SPELL_LENGTH];
    char* spellList[MAX_NUM_SPELLS];
    char* castSpells[MAX_NUM_SPELLS];
    char playerSpell[MAX_SPELL_LENGTH];
    char computerSpell[MAX_SPELL_LENGTH];
    int numSpells = 0;
    int numCastSpells = 0;

    // Initialize letterCounts array
    LetterCount letterCounts[26] = {0};

    // Open the file for reading
    file = fopen("spells.txt", "r");

    // Check if the file is successfully opened
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }
    

    // Read spells from the file and store them in spellList
    while (fscanf(file, "%s", spell) != EOF && numSpells < MAX_NUM_SPELLS) {
        spellList[numSpells++] = strdup(spell);
        updateLetterCounts(spell, letterCounts);
    }

    // Close the file
    fclose(file);

    // Randomly choose the first spell for the computer
    strcpy(computerSpell, spellList[rand() % numSpells]);
    castSpells[numCastSpells++] = strdup(computerSpell);
    updateLetterCounts(computerSpell, letterCounts);
    printf("Computer casts: %s\n", computerSpell);

    while (1) {
        // Player's turn
        if (!getPlayerSpell(computerSpell, castSpells, numCastSpells, spellList, numSpells, playerSpell)) {
            printf("Computer wins!\n");
            break;
        }
        if (!wordExists(spellList, numSpells, playerSpell)) {
            printf("Computer wins!\n");
            break;
        }
        castSpells[numCastSpells++] = strdup(playerSpell);
        updateLetterCounts(playerSpell, letterCounts);

        // Check if the computer won
        if (strlen(playerSpell) == 0) {
            printf("Computer wins! Player failed to cast a spell.\n");
            break;
        }

        // Computer's turn
        if (!getComputerSpell(playerSpell, castSpells, numCastSpells, spellList, numSpells, computerSpell, letterCounts)) {
            printf("Player wins!\n");
            break;
        }
        castSpells[numCastSpells++] = strdup(computerSpell);
        updateLetterCounts(computerSpell, letterCounts);

        // Check if the player won
        if (strlen(computerSpell) == 0) {
            printf("Player wins!\n");
            break;
        }
    }

    // Free memory for cast spells and spell list
    for (int i = 0; i < numCastSpells; ++i) {
        free(castSpells[i]);
    }

    for (int i = 0; i < numSpells; ++i) {
        free(spellList[i]);
    }

    return 0;
}

// Function definitions
int isSpellAlreadyCast(char* spell, char** castSpells, int numCastSpells) {
    for (int i = 0; i < numCastSpells; ++i) {
        if (strcmp(spell, castSpells[i]) == 0) {
            return 1;  // Spell has already been cast
        }
    }
    return 0;  // Spell has not been cast
}

int areAvailableWords(char** spellList, int numSpells, char* previousSpell, LetterCount* letterCounts) {
    for (int i = 0; i < numSpells; ++i) {
        if (previousSpell[strlen(previousSpell) - 1] == spellList[i][0] &&
            !isSpellAlreadyCast(spellList[i], NULL, 0)) {
            return 1;  // There is at least one available word
        }
    }
    return 0;  // No available words
}

int wordExists(char** spellList, int numSpells, char* spell) {
    for (int i = 0; i < numSpells; ++i) {
        if (strcmp(spell, spellList[i]) == 0) {
            if (isSpellAlreadyCast(spellList[i], NULL, 0))
                return 0;  // There is at least one available word
            return 1;
        }
    }
    return 0;  // No available words
}

void updateLetterCounts(char* spell, LetterCount* letterCounts) {
    letterCounts[spell[0] - 'a'].countFirst++;
    letterCounts[spell[strlen(spell) - 1] - 'a'].countLast++;
}

int getPlayerSpell(char* previousSpell, char** castSpells, int numCastSpells, char** spellList, int numSpells, char* playerSpell) {
    printf("Enter your spell: ");
    scanf("%s", playerSpell);

    // Validate the chosen spell
    if (
        isSpellAlreadyCast(playerSpell, castSpells, numCastSpells) ||
        (numCastSpells > 0 && previousSpell[strlen(previousSpell) - 1] != playerSpell[0])
    ) {
        printf("Game Over\n");
        return 0;
    }
    return 1;
}

int getComputerSpell(char* previousSpell, char** castSpells, int numCastSpells, char** spellList, int numSpells, char* computerSpell, LetterCount* letterCounts) {
    if (!areAvailableWords(spellList, numSpells, previousSpell, letterCounts)) {
        printf("No available words. Game Over.\n");
        computerSpell[0] = '\0';  // Set an empty string to indicate the end of the game
        return 0;
    }

    // Winning Move Strategy
    for (int i = 0; i < numSpells; ++i) {
        if (!isSpellAlreadyCast(spellList[i], castSpells, numCastSpells) &&
            previousSpell[strlen(previousSpell) - 1] == spellList[i][0] &&
            letterCounts[spellList[i][strlen(spellList[i]) - 1] - 'a'].countFirst == 0) {
            strcpy(computerSpell, spellList[i]);
            printf("Computer casts: %s\n", computerSpell);
            return 1;
        }
    }

    // Blocking Move Strategy
    char maxLastLetter = '\0';
    int maxCountFirst = 0;
    for (int i = 0; i < numSpells; ++i) {
        if (!isSpellAlreadyCast(spellList[i], castSpells, numCastSpells) &&
            previousSpell[strlen(previousSpell) - 1] == spellList[i][0]) {
            char lastLetter = spellList[i][strlen(spellList[i]) - 1];
            int countFirst = letterCounts[lastLetter - 'a'].countFirst;
            if (countFirst > maxCountFirst) {
                maxCountFirst = countFirst;
                maxLastLetter = lastLetter;
                strcpy(computerSpell, spellList[i]);
            }
        }
    }

    // If a blocking move is available, choose it
    if (maxLastLetter != '\0') {
        printf("Computer casts: %s\n", computerSpell);
        return 1;
    }

    // If no blocking move, choose a random spell
    int counter = 0;
    do {
        strcpy(computerSpell, spellList[rand() % numSpells]);
        counter++;
    } while ((isSpellAlreadyCast(computerSpell, castSpells, numCastSpells) ||
              previousSpell[strlen(previousSpell) - 1] != computerSpell[0]) && counter != 3 * numSpells);

    if (counter == numSpells)
        return 0;

    printf("Computer casts: %s\n", computerSpell);
    return 1;
}

