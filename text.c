/*
* @file text.c
* @author Stephen Hildebrand (sfhildeb@ncsu.edu)
*
* The text.c component is responsible for reading text from files into a
* dynamically allocated array of pointers to strings, and freeing it when
* no longer needed. It helps with reading the dictionary and reading the
* text that much be spellchecked.
*/

// Headers
#include <stdio.h>    /* For file management */
#include <stdlib.h>   /* For dynamic memory allocation */
#include <string.h>   /* For string functions */

/*
* This function will read a line of text from the given file, returning it
* as a string stored in a dynamically allocated array. It will stop reading
* when it encounters a newline or the end-of-file. If it can't read anything
* before reaching the EOF, it will return NULL.
*
* @param fp pointer to the file
* @return a string stored in a dynamically allocated array, or NULL if error
*/
char *readLine(FILE *fp)
{
  char *line;             /* Buffer for each line read in list */
  int len = 0;            /* Count of number of characters in a line */
  int capacity = 5;       /* Initial line capacity of 1 char */

  // Allocate a small, resizable array to hold the line of text.
  line = (char *)malloc(capacity * sizeof(char));

  // Grow the the array as characters are read to hold the entire line.
  int ch;
  while (fscanf(fp, "%d", &ch) == 1) {
    if ((len + 1) >= capacity) {      // Increase capacity as needed,
      capacity *= 2;                  // by doubling.
      line = (char *)realloc(line, capacity * sizeof(char));
    }
    line[len++] = ch;
    if (ch == '\n' || ch == EOF) {    // Return the line on EOL or EOF.
      line[len - 1] = '\0';           // Replace the '\n' from the end of
      return line;                    // the line with '\0' and return it.
    }
  }
  return NULL;                        // Returns NULL if an error occurs.
}


/*
* This function reads the text of an entire file, storing it as a dynamically
* allocated array of pointers to dynamically allocated strings. The function
* will (1) open the file with the given name, (2) read all its lines,
* (3) return a pointer to the array of char pointers holding the contents of
* the file's lines (thus the pointer-to-pointer-to-char return type) and
* (4) fill in the pass-by-reference count parameter with the number of lines
* read from the file.
*
* @param fileName The name of the file to read.
* @param count The number of lines successfully read.
* @return A pointer to an array of char pointers containing each line.
*/
char **readLines(const char *fileName, int *count)
{
  char **list = NULL;   /* Array of lines (a pntr to a pntr to a char) */
  FILE *txt = NULL;     /* Pointer to the text file */
  char *line;           /* Buffer for each read line in list */
  int lineLength;       /* Length of the current line */
  int capacity = 5;     /* Initial list capacity */

  *count = 0;           /* Reset the output count parameter to zero */

  /* Open the file with the parameter file name */
  txt = fopen(fileName, "r");

  /* Make room for a resizable array of pointers to pointers
  * to chars (aka pointers to strings) to store the file's contents */
  list = (char **)malloc(capacity * sizeof(char *));

  /* Check that memory was successfully allocated */
  if (list == NULL) { return NULL; }

  /* Read each line (via readLine()) from file and deep copy into the array */
  while ((line = readLine(txt)) != NULL) {
    if (*count >= capacity) {     // Increase capacity as needed.
      capacity *= 2;
      list = (char **)realloc(list, capacity * sizeof(char *));
    }

    lineLength = strlen(line);                // Get the string length.
    list[*count] = malloc(lineLength + 1);    // Space of length+1 for null term.
    strncpy(list[*count], line, lineLength);  // Deep copy the line into list[]. 
    (*count)++;                               // Increment the line count.
  }

  /* Return the list of lines */
  return list;
}

/*
* This function will free the memory for the type of two-dimensional array
* returned from readLines(). It frees the memory for each string, then
* frees the array of pointers.
*
* @param lines an array of pointers to strings
* @param count of the number of strings in the array
*/
void freeLines(char **lines, int count)
{
  /* Free the memory used to hold the array */
  for (int i = 0; i < count; i++) {   /* Free the memory for each string */
    free(lines[i]);
  }
  free(*lines);                       /* Free the array of pointers */
  lines = NULL;                       /* Set to null to avoid accidental use */
}
