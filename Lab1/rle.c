// Zohaib Ahmed, Traison Dietrich
// 09/02/2022
// Write a program that compresses and decompresses files.

// Include file goes here
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void encode(int readFile, int writeFile, int runLength)
{
    // create counter
    unsigned char counter = 1;
    char buffer[runLength];

    // check if file can be read
    ssize_t retVal = read(readFile, buffer, runLength);
    if (retVal == -1)
    {
        perror("Could not read from file");
        exit(-1);
    }

    // end line character
    buffer[retVal] = '\0';

    while (1)
    {
        char prev[runLength];
        memcpy(prev, buffer, strlen(buffer) + 1); // copy the buffer into previous byte

        // see current value
        ssize_t retVal = read(readFile, buffer, runLength);

        // if no return error
        if (retVal == -1)
        {
            perror("Could not read from file");
            exit(-1);
        }

        // end of file cancel
        if (retVal == 0)
        {
            break;
        }

        // call current value next
        char *next = buffer;

        // compare strings between previous and next, 0 if same
        int ret = strncmp(prev, next, runLength);

        // max is 255
        if (counter == 255)
        {
            retVal = write(writeFile, &counter, 1); // when 255 is hit, write it to file
            if (retVal == -1)                       // if file failure, error
            {
                perror("Could not write to file");
                exit(-1);
            }

            retVal = write(writeFile, &prev, strlen(prev)); // write previous char to file
            if (retVal == -1)                               // if file failure, error
            {
                perror("Could not write to file");
                exit(-1);
            }
            counter = 1; // reset counter
        }
        // If the strings are the same, increase counter
        else if (ret == 0)
        {
            counter++;
        }
        // If strings are different, write the counter and previous string to file, reset counter
        else if (ret != 0)
        {
            retVal = write(writeFile, &counter, 1);
            if (retVal == -1) // if file fails, error
            {
                perror("Could not write to file");
                exit(-1);
            }

            printf("%x",  prev);

            retVal = write(writeFile, &prev, strlen(prev));
            if (retVal == -1) // if file fails, error
            {
                perror("Could not write to file");
                exit(-1);
            }
            counter = 1;
        }
    }
    return;
}

void decode(int readFile, int writeFile, int runLength)
{
    // to decode, basically loop through the "counter" number of times writing the character, then move to next char
    // end if end character found
    while (1)
    {
        unsigned char counter;
        ssize_t retVal = read(readFile, &counter, 1); // set counter for each character
        if (retVal == -1)                             // if file fails, error
        {
            perror("Could not read from file");
            exit(-1);
        }
        if (retVal == 0) // if counter is end of line char
        {
            break;
        }

        // char str[2] = {0};
        // str[0] = counter;

        char buffer[runLength]; // Read in the actual character
        retVal = read(readFile, buffer, runLength);
        if (retVal == -1) // if file fails, error
        {
            perror("Could not write to file");
            exit(-1);
        }

        for (int i = 0; i < counter; i++)
        {
            retVal = write(writeFile, buffer, runLength);
            if (retVal == -1) // if file fails, error
            {
                perror("Could not write to file");
                exit(-1);
            }
        }
    }
    return;
}

int main(int argc, char *argv[])
{
    // create variables for arguments
    char *readFile;
    char *writeFile;
    char *runLength;
    int mode;

    // Read in the arguments
    readFile = argv[1];
    writeFile = argv[2];
    runLength = argv[3];
    mode = atoi(argv[4]);

    // Error checking
    if (argc == 1)
    {
        printf("rle <input file> <output file> <compression length> <mode bit: 0, 1> \n");
        exit(0);
    }

    if (argc != 5)
    {
        printf("Incorrect useage!  \n rle <input file> <output file> <compression length> <mode bit: 0, 1> \n");
        exit(0);
    }

    if (atoi(runLength) < 1)
    {
        printf("Compression length must be >= 1! \n");
        exit(0);
    }

    // test read file
    int readFD = open(readFile, O_RDONLY);
    if (readFD == -1)
    {
        perror("Could not open input file");
        exit(-1);
    }

    // test write file, create if it hasnt been or truncate whatever is in it with read/write permission
    int writeFD = open(writeFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (writeFD == -1)
    {
        perror("Could not open output file");
        exit(-1);
    }

    // off input compress or decompress
    if (mode == 1)
    {
        decode(readFD, writeFD, atoi(runLength));
    }
    else
    {
        encode(readFD, writeFD, atoi(runLength));
    }

    return 0;
}
