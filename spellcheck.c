/*
* @file spellcheck.c
* @author Stephen Hildebrand (sfhildeb@ncsu.edu)
*
* The�spellcheck.c�component will contain the main() function and any other
* supporting functions you decide to use. It will be responsible for handling
* the command-line arguments, finding words in the text, checking to see if
* they're in the dictionary, and responding to user commands.
*/

/* Headers */
#include <stdio.h>      /* For file management */
#include <stdlib.h>     /* For dynamic memory allocation */
#include <string.h>     /* For string functions */
#include <ctype.h>      /* For character functions */
#include "text.h"       /* For readLines, readLine, and freeLines functions */

/* Function Prototypes */
//char *readWord(char *);
void isValidDictionary(char **, int);
void setColor(char[], int);
//void addToDictionary(char**, char*, int);
int compareWords(const void *, const void *);
void sortWords(char **, int);
char *searchDictionary(char *, char **, int);

/* Variables */
#define ESC 27                /* ASCII escape character. */
#define RED_SIZE 5            /* Size of red color sequence arrays */
#define DEFAULT_SIZE 4        /* Size of default color char sequence array */
#define USER_QUIT 1           /* Error code when user selects quit */
#define VALID_DICTIONARY 10   /* Number returned for a valid dictionary */

/* Array of 5-character sequence to print color red */
char colorRed[] = { ESC, '[', '3', '1', 'm' };
/* Array holding 4-character sequence to switch print color to default. */
char colorDefault[] = { ESC, '[', '0', 'm' };


/*
* The main function for the SpellCheck program. Handles command-line
* arguments and check for valid files.
*
* @param argc The number of command line parameters
* @param *argv[] The array of command line parameters
* @return 0 if runs successfully, any other number if unsuccessful
*/
int main(int argc, char *argv[])
{
  char **txtArray = NULL;               /* Array containing the lines read */
  char **dictArray = NULL;              /* Array containing the dictionary */
  FILE *txtFile = NULL;                 /* Pointer to the text file */
  FILE *dictFile = NULL;                /* Pointer to the dictionary file */
  const char *txtName = NULL;           /* Input file name */
  const char *dictName = NULL;           /* Dictionary file name */
  int txtLineCount;                     /* Count of number of lines read */
  int dictLineCount;                    /* Count of entries/lines in dictionary */

                                        /* Check for valid number of arguments */
  if (argc < 2 || argc > 3) {
    fprintf(stderr, "usage: spellcheck <textfile.txt> [words.txt]\n");
    exit(EXIT_FAILURE);
  }

  /* Check that file can be opened */
  txtName = argv[1];
  txtFile = fopen(txtName, "r");
  if (txtFile == NULL) {
    fprintf(stderr, "Can't open file: %s\n", txtName);
    exit(EXIT_FAILURE);
  }

  /* Check for a custom dictionary file */
  if (argc == 3)
    dictName = argv[2];
  else
    dictName = "words.txt";

  /* Check that the dictionary file can be opened */
  dictFile = fopen(dictName, "r");
  if (dictFile == NULL) {
    fprintf(stderr, "Can't open file: %s\n", dictName);
    exit(EXIT_FAILURE);
  }

  /* Close files (will be passed by name to readLines() function) */
  fclose(txtFile);
  fclose(dictFile);

  /* Read lines from file and dictionary and store in corresponding arrays */
  txtArray = readLines(txtName, &txtLineCount);
  dictArray = readLines(dictName, &dictLineCount);

  /* Check that the dictionary array contains valid, lowercase entries */
  isValidDictionary(txtArray, dictLineCount);


  /* Sort the dictionary using qsort(), to be searched via bsearch() */
  sortWords(dictArray, dictLineCount);

  /* Iterate, line-by-line, through the array **txtArray */
  for (int i = 0; i < txtLineCount; i++) {
    char *word;             /* Character buffer for a word */
                            // int wordStart;          /* Index in the line that this word started */
    char ch;            /* The character currently pointed to */
    int lineLen = 0;        /* The total length of the current line */
    int wordLen = 0;        /* Count of number of characters in a word */
    int capacity = 5;       /* Initial word capacity */

                            /* Allocate a small, resizable array to hold the current word */
    word = (char *)malloc(capacity * sizeof(char));

    lineLen = strlen(txtArray[i]);

    /* Iterate through a single line until a null terminator is read */
    for (int p = 0; p < lineLen; p++) {
      wordLen = 0;              // Reset counter for this next word's length
      ch = dictArray[i][p];

      while (!isalpha(ch)) {    // Ignore any series of non-alphabetical chars
        ch = dictArray[i][p];
        p++;
      }
      while (isalpha(ch)) {     // While iterating through an alphabetic word
        if ((wordLen + 1) >= capacity) {  // Increase word capacity as needed
          capacity *= 2;
          word = (char *)realloc(word, capacity * sizeof(char));
        }
        word[wordLen++] = ch;   // Add the character to the word array
        ch = dictArray[i][p];
        p++;
      }

      for (int t = 0; word[t]; t++) {
        word[t] = tolower(word[t]);
      }

      /* Check whether the word is in the dictionary array */
      if (searchDictionary(word, dictArray, dictLineCount) == 0) {

        /* Prompt user and handle mispelled word accordingly */
      prompt:

        /* Print the text surrounding the misspelled word */
        if (i - 1 >= 0) {
          printf("\n%s\n", txtArray[i - 1]);          /* Print previous line */
        }
        for (int k = 0; k < (p - wordLen); k++) /* Print previous part of line */
          printf("%c", txtArray[i][k]);

        setColor(colorRed, RED_SIZE);         /* Set text color to red */
        printf("%s", word);                   /* Print mispelled word in red*/
        setColor(colorDefault, DEFAULT_SIZE); /* Set text color back to default */
        int c;                                /* Print the remainder of line */

        while ((c = getchar()) != '\n')
          putchar(c);

        if (i + 1 <= txtLineCount)
          printf("\n%s\n", txtArray[i + 1]);  /* Print the next line */

                                              /* Prompt for user input response */
        printf("\n%s\n(r)eplace, (a)dd, (n)ext or (q)uit: ", txtArray[i + 1]);
        int response = getchar();

        /* Handle each possible input to quit, next, add, or replace word */
        if (response == 'q') {        // Quit
          fprintf(stderr, "%s\n", "Discarding changes");
          exit(USER_QUIT);
        }
        else if (response == 'n') {   // Next word
          break;
        }
        else if (response == 'a') {   // Add word to dictionary
                                      // Implement addToDictionary function
          dictArray = (char **)realloc(dictArray, dictLineCount + sizeof(char *));
          int lineLength = strlen(word);       // Get the string length.
          dictArray[dictLineCount] = malloc(lineLength + 1); // Space of length+1 for null term.
          strncpy(dictArray[dictLineCount], word, lineLength); // Deep copy the line into list[].
          dictLineCount++;                  // Increment the line count.
        }
        else if (response == 'r') {   // Replace word
                                      //   char *correction;
          getchar();                  // Skip blank space
          int chNew;
          // Continue reading input until EOL
          while ((chNew = getchar()) != '\0') {
            if ((wordLen + 1) >= capacity) {
              capacity *= 2;
              word = (char *)realloc(word, capacity * sizeof(char));
            }
            word[wordLen++] = ch;
          }
          // Add null terminator to the word
          word[wordLen++] = '\0';
        }
        else {
          printf("Unknown command\n");
          goto prompt;
        }

      } // If word not in dictionary

    } // Read a single line

  } // For each line


  printf("Spellcheck complete.\n");

  // Implement file backup
  int txtNameLength = strlen(txtName);
  char *txtNewName = (char *)malloc(txtNameLength + (5 * sizeof(char)));
  strcat(txtNewName, ".bak");
  printf("Backing up %s to %s", txtName, txtNewName);
  rename(txtName, txtNewName);

  // TODO Write the updates text to the original text file
  printf("Writing updated %s\n", txtName);


  /* Check that file can be opened */
  FILE *txtOut = fopen(txtName, "w");
  for (int j = 0; j < txtLineCount; j++) {
    fprintf(txtOut, "%s\n", txtArray[j]);
  }
  fclose(txtOut);

  // Free the memory used to hold each array
  freeLines(txtArray, txtLineCount);
  freeLines(dictArray, dictLineCount);

  return EXIT_SUCCESS;

}

int readWords(char *words[], int capacity)
{
  int count = 0;
  char str[100];
  while (count < capacity && scanf("%99s", str) == 1) {
    words[count] = (char *)malloc(strlen(str) + 1);
    strcpy(words[count], str);
    count++;
  }
  return count;
}

/*
* This function is used by the other functions within spellcheck.c to set
* the text color.
*
* @param color array char sequence specifying a color
* @param size number of characters in the array
*/
void setColor(char color[], int size)
{
  for (int i = 0; i < size; i++) {
    putchar(color[i]); // Insert character sequence for text color
  }
}

/*
* Compares string a to string b and returns a negative integer if a < b,
* zero if they are equal, and a positive integer if a > b. Function used
* by qsort() within main for sorting the dictionary.
*
* @param a The object to compare
* @param b The object to be compared to
*/
int compareWords(const void *a, const void *b)
{
  return (strcmp(*(char **)a, *(char **)b));
}

/*
* Verifies that a dicitonary file array contains a valid dictionary
* with alphabetic, lower-case letters. Each line of a dictionary file
* is expected to be a sequence of lower case characters followed by
* the null terminator ('\0').
*
* @param **dictionary The dictionary word array to check
* @param lineCount The number of lines in the dictionary array
* @return VALID_DICTIONARY if valid, or 0 if dictionary contains an error
*/
void isValidDictionary(char **dictionary, int lineCount)
{
  for (int i = 0; i < lineCount; i++) {   // For each line
    char ch;
    int j = 0;
    while (ch != '\0') {                    // While null terminator not found
      ch = dictionary[i][j];                // Scan each char of this line
      //sscanf(dictionary[i], "%c", &ch);
      if (isspace(ch)) {                    // Skip over whitespace
        fprintf(stderr, "Invalid word, line: line-no%d\n", (i + 1));
        exit(EXIT_FAILURE);
      }
      else if (isupper(ch)) {
        fprintf(stderr, "Invalid word, line: line-no%d\n", (i + 1));
        exit(EXIT_FAILURE);
      } else if (!islower(ch)) {            // Else if not a lower-case
        fprintf(stderr, "Invalid word, line: line-no%d\n", (i + 1));
        exit(EXIT_FAILURE);                         // Returns 0 if invalid
      }
    }
  }
}

/*
* Sorts the dictionary using the qsort() method.
*
* @param dictArray The dictionary list to sort
* @param dictLineCount The number of lines to sort
*/
void sortWords(char **dictArray, int dictLineCount)
{
  qsort(dictArray, dictLineCount, sizeof(char *), compareWords);
}

/*
* Performs a binary search for the the word in the sorted dictionary
* array via the bsearch function. This function works as follows:
* void *bsearch(const void *key, const void *base, size_t nitems,
*               size_t size, int (*compar)(const void *, const void *));
*
* @param word the sought string
* @param dictArray the dictionary array being searched
* @return a pointer to a pointer to the char found, or 0 if not found
*/
char *searchDictionary(char *word, char **dictArray, int dictLineCount)
{
  char **ch = bsearch(&word, dictArray, dictLineCount,
    sizeof(dictArray), compareWords);
  if (ch == 0) {
    return 0;
  }
  return *ch;
}
