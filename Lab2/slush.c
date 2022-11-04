#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

/** CHANGES MADE -zo
 * Added comments to explain general lines
 * The commands are executed right to left but when we add them to my_argv it is left to right
 * so they need to be reversed. I made a commands array and looped to add them in reverse.
 * ^ Now when forking and executing, we can loop through them in correct order
 * Command shell starts with "$" so I just printed that before we accept arguments
 * Not sure if we need to add an ending character to arguments array since we are executing the arguments ourselves.
 **/

int main(int argc, char *argv[])
{

    // input string size
    int buff = 128;
    int max_args = 15; // # of arguments
    int max_argv_size = max_args + 2;
    char *cmd;
    char *my_argv[max_argv_size]; // max 17 arguments
    char *commands[max_argv_size];
    char input_string[buff];

    // get input
    printf("$ "); // terminal character
    char *ret = fgets(input_string, buff, stdin);
    if (ret == NULL)
    {
        // if input is bad end program
        printf("%s: Not found", argv[1]);
        exit(-1);
    }

    input_string[strcspn(input_string, "\n")] = 0; // strip off newline character

    char *token;
    const char end[2] = "(";           // ending character
    token = strtok(input_string, end); // tokenize till ending character

    // first argument will be first token
    cmd = token;
    my_argv[0] = cmd;

    int pos = 0; // initialize a counter
    while (token != NULL)
    { // parse
        token = strtok(NULL, end);
        pos += 1;
        my_argv[pos] = token;
    }
    // my_argv[pos] = '\0'; // does not compile not sure why

    // reverse the arguments because they'd execute backwards
    int reverseindex = pos;
    for (int i = 0; i <= pos - 1; i++)
    {
        commands[i] = my_argv[reverseindex];
        printf("%s", commands[i]);
        reverseindex--;
    }

    // execvp(cmd, my_argv);

    return 0;
}
