/*
 * @file text.h
 * @author Stephen Hildebrand (sfhildeb@ncsu.edu)
 *
 * Header file for access to text.c functions.
 */

/* Macros */
//#ifdef DEBUG
//// Print a string's label and value
//#define REPORT_STR(X) printf("%s = %s\n", #X, (X))
//// Print a char's label and value
//#define REPORT_CH(X) printf("%s = %d\n", #X, (X))
//// Print an integer's label and value
//#define REPORT_INT(X) printf("%s = %d\n", #X, (X))
//// Print a hex's label and value
//#define REPORT_HEX(X) printf("%s = %X\n", #X, (X))
//#else
//// Define the macros to do nothing
//#define REPORT_STR(X)
//#define REPORT_CH(X)
//#define REPORT_INT(X)
//#define REPORT_HEX(X)
//#endif // DEBUG

/* Prototypes */
/*
* This function will read a line of text from the given file, returning it
* as a string stored in a dynamically allocated array. It will stop reading
* when it encounters a newline or the end-of-file. If it can't read anything
* before reaching the EOF, it will return NULL.
*
* @param fp pointer to the file
* @return a string stored in a dynamically allocated array, or NULL if error
*/
char *readLine(FILE *);
/*
* This function reads the text of an entire file, storing it as a dynamically
* allocated array of pointers to dynamically allocated strings. The function
* will (1) open the file with the given name, (2) read all its lines,
* (3) return a pointer to the array of char pointers holding the contents of
* the file's lines (thus the pointer-to-pointer-to-char return type) and
* (4) fill in the pass-by-reference count parameter with the number of lines
* read from the file.
*
* @param fileName name of the file to read.
* @param count the number of lines successfully read.
* @return pointer to an array of char pointers containing each line.
*/
char **readLines(const char *, int *);
/*
* This function will free the memory for the type of two-dimensional array
* returned from readLines(). It frees the memory for each string, then
* frees the array of pointers.
*
* @param lines an array of pointers to strings
* @param count of the number of strings in the array
*/
void freeLines(char **, int count);
