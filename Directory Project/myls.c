#include"myls.h"

//This is our myls function that will take in a directory name 
//and integer hidden to print out the necessary file names.
void _myls(const char *d, int hidden)
{
    struct dirent *directory;

    DIR *dir;
    dir = opendir(d);

    // Checks if directory is valid. If not, prints "Cannot access said directory."
    if(!dir)
    {
        fprintf(stderr, "Cannot acess %s\n", d);
        return;
    }

    //This will print out the directory name before the files inside are printed.
    //If there were no specific directories given, it will print out the current path.
    if(d != "."){
        printf("%s\n", d);
    }
    else{
        char cwd[256];

        if(getcwd(cwd, sizeof(cwd)) != NULL){
            printf("%s\n", cwd);
        }
    }
    
    //Reading files inside the specific directory. Checks to see if user flagged -h to show hidden files.
    while((directory = readdir(dir)) != NULL)
    {
        if(hidden == 0){
            if(directory->d_name[0] == '.'){
                continue;
            }
        }
        printf("%s\n", directory->d_name);
    }
    
    printf("\n");

    if(closedir(dir) == -1){
        printf("Cannot close current");
    }
}

//This is our main function that will check through the command line arguments to check whether the flag
//-h was used, and will call the _myls function.
int main(int argc, char **argv) 
{
    int hidden = 0;

    //Checks to see is myls was the only thing typed, and will read through the current directory if so.
    if (argc == 1){
        _myls(".",0);
    }
    
    //Checks to see if the flag -h was used, and will change the hidden variable to 1 if true.
    if(argc >=2 && (strcmp(argv[1] , "-h") == 0)){
        hidden = 1;
    }

    //Checks to see if the flag-h was used and that there were no specific directories listed to read.
    //This will find all files hidden and visible in the current directory.
    if(argc ==2 && (strcmp(argv[1] , "-h") == 0)){
        _myls(".", hidden);
    }

    //This will skip the first argument if flag -h was seen, but otherwise loop through the arguments
    //that were given in the command line, and find the filenames in each directory given.
    for(int i=1; i < argc; i++){
        if(hidden == 1 && i == 1){
            continue;
        }
        _myls(argv[i], hidden);
    }
    return 0;
}