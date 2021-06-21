/*
James Lee (RedID: 820655947)
James Romanov (RedID: 820821671)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/types.h>

//Progress Struct
typedef struct 
{
    long *CurrentStatus;
    long InitialValue;
    long TerminationValue;
} PROGRESS_STATUS;

//This function will be deciding when and how many "-" or "+" need to printed out to keep the progress bar updated.
void * progress_monitor(void *ptr){

    //Variable Declarations
    PROGRESS_STATUS *progStat = (PROGRESS_STATUS *)ptr;
    long currentPercent = 0;
    long prevPercent = 0;
    long i;
    char progBar;

    //This while loop will check to see if the current number of hyphens need to be printed by checking to see if the
    //progress has changed at all. 
    while(currentPercent < 49){
        prevPercent = currentPercent;
        currentPercent = (*progStat->CurrentStatus - progStat->InitialValue) * 50 / progStat->TerminationValue;

        //This for loop will check to see if the progress has changed, and we will know when to print "-" or "+"
        //based on whether the modulo is 0 or not.
        for(i = prevPercent + 1; i <= currentPercent; i++){
            progBar = (i % 10) ? '-' : '+';
            printf("%c", progBar);
            fflush(stdout);
        }
        if(currentPercent == 49){
            printf("+");
            fflush(stdout);
            currentPercent++;
            break;
        }
    }
    pthread_exit(0);
    return NULL;
}

void wordCount(const char *d)
{
    //Variable Declarations
    FILE *fp;
    char *letter;
    char c;
    int r;
    int progM;
    int lastword = 0;
    struct stat st;
    PROGRESS_STATUS *progStat;
    progStat = malloc(sizeof(PROGRESS_STATUS));
    pthread_t thread1;
    long int wordCount = 0;
    long int byteCount = 0;
    
    //This will open the file in the reading format, and will check to see if it is a valid file or not,
    //and will return an error message if not file could be found.
    fp = fopen(d, "r");
    if(fp == NULL){
        printf("Could not open file.");
        return;
    }


    //This will get us the total amount of bytes inside of the file that was opened.
    if (stat(d, &st) == 0){
        progStat->TerminationValue = st.st_size;
    }
    else{
        printf("could not determine file size.");
    }

    progM = pthread_create(&thread1,NULL,progress_monitor,(void*) progStat);

    c = fgetc(fp);
    progStat->CurrentStatus = &byteCount;

    //Counting for words.
    if(isspace(c) == 0){
        wordCount++;
    }
    do{
        byteCount++;

        if(isspace(c)){
            lastword = 1;
        }
        else{
            if(lastword == 1){
                wordCount++;
                lastword = 0;
            }
        }
        c = fgetc(fp);
    } while(c != EOF);

    //Prints the word count along with the statment.
    printf("\nThere are %ld words in %s\n", wordCount, d);
    pthread_join(thread1, NULL);
    
}

//Checks if user imputed file.
int main(int argc, char **argv){
    if(argc == 2){
        wordCount(argv[1]);
    }
    if(argc == 1)
    {
        printf("no file specified");
    }
}

