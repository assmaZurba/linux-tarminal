//ex3
//written by Assma Zurba 323139782
//the program scan a command from the user and the program try to run the commands and save all this commands in the history file.
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/prctl.h>
#define TRUE 1
#define MAX_LENGTH_OF_SENTENCE 512
#define MAX_LENGTH_OF_PATH 300
#define SPACE_CHAR ' '
#define EMPTY_SENTENCE ""
#define HISTORY_FILE_PATH "file.txt"
int numOfCommands=0;//the num of commands that the user write in all the program.
int numOfWords=0;//the num of words that the user write in all the program.


/*
 * parameters:
 * commandArray: the dynamic array that contains the command.
 * numOfWords: the num of items in the command array.
 * free the dynamic array.
 */
void freeTheArray(char * commandArray[],int numOfWords);

/*
 * parameters:
 * sentence: string
 * return the num of words in the sentence.
 */
int countNumOfWords (char sentence []);

/*
 * parameters:
 * sentence: string
 * indexOfWord: the index of the word in the sentence.
 * return the num of chars in the word.
 */
int countNumOfCharsInOneWord (char sentence [],int indexOfWord,int hasCab);

/*
 * parameters:
 * sentence: string
 * preWordIndex: the index of the previous word  in the sentence.
 * return the index of the next word in the sentence.
 */
int nextWordIndex(char sentence [],int preWordIndex);

/*
 * parameters:
 * sentence: string
 * running the command.
 * return -1 if the memory allocate fail.
 * return 0 if the function succeed
 */
int runningCommand(char sentence[]);

/*
 * the function prints the history
 * return -1 if there is an error in opening the file.
 * return 0 if the function succeed
 */
int printHistory();

/*
 * parameters:
 * sentence: string
 * num: the line of the command in the history file.
 * return -1 if the command is not in the history.
 * return 0 if the command is in the history.
 * find command from the history and changed the sentence to become the command that the function find.
 */
int commendFromHistory( char sentence[],int num);

/*
 * parameters:
 * sentence: string
 * the function adds the command to the history file.
 * return -1 if there is an error in opening the file.
 * return 0 if the function succeed
 */
int addToHistory(char sentence []);

/*
 * parameters:
 * sentence: string
 * the function checks if the command is !<num> and if yes the function runs the command number <num> in the history file.
 * return -1 if it was not calling a command in the history.
 * return 1 if the command in the history and the command !<num> succeeded.
 * return 0 if the command is not in the history.
 */
int checkIfCallCommandFromHistory(char sentence[]);

/*
 * parameters:
 * command: string
 * command Array: array that we want to fill.
 * numOfWords: the num of words in the command.
 * fill the command array to send to exevp function.
 */
int fillCommandArray(char command [],char * commandArray[],int numOfWords);

/*
 * parameters:
 * sentence: string
 * check the edge conditions
 * return 1 if the function did changes.
 * return -1 if the function did not do anything.
 * return 0 if we should leave the while loop.
 */
int badConditions (char sentence []);

/*
 * parameters:
 * sentence: string
 * ifPlayInBackGround: 1:if the command should run in the background,0:otherwise.
 * the function runs the one pipe command.
 * return -1 if the something went wrong.
 * return 1 if the function succeeded.
 */
int runOnePipe(char sentence []);

/*
 * parameters:
 * sentence: string
 * ifPlayInBackGround: 1:if the command should run in the background,0:otherwise.
 * the function runs the two pipes command.
 * return -1 if the something went wrong.
 * return 1 if the function succeeded.
 */
int runTwoPipe(char sentence []);

/*
 * parameters:
 * sentence: string
 * ifPlayInBackGround:1:if the command should run in the background,0:otherwise.
 * the function runs the zero pipes command.
 * return -1 if the something went wrong.
 * return 1 if the function succeeded.
 */
int runZeroPipe(char sentence []);

/*
 * parameters:
 * sentence: string
 * return 1 if the sentence start with word "nohup"
 * return 0 otherwise.
 */
int ifContainsNohup(char sentence []);

/*
 * parameters:
 * signal: the signal that called the function.
 * the handler that the signal function called when it receive child signal.
 * the function kills the child to avoid the zombie state when the child run in the background.
 */
void proc_exit(int signal);

/*
 * parameters:
 * sentence: string.
 * the function checks if the sentence should play in the background, if yes, it changes the sentence.
 * return 1 if the sentence should play in the background.
 * return 0 otherwise.
 */
int checkBackground(char sentence []);

//the main.
int main() {
    signal (SIGCHLD,proc_exit );//detect the child signal
    char sentence[MAX_LENGTH_OF_SENTENCE] ;
    while(TRUE) {
        char path [MAX_LENGTH_OF_PATH];//the path of the folder.
        getcwd(path,MAX_LENGTH_OF_PATH);//get the path of the folder.
        strcat(path, ">");
        printf("%s" ,path);
        fgets(sentence, MAX_LENGTH_OF_SENTENCE, stdin);
        int badConditionsResult;
        int u=0;
        while (sentence[u]!='\n'){
            if (sentence[u]==SPACE_CHAR)
                u++;
            else
                break;
        }
        if(u==strlen(sentence)-1){//if all the sentence is a spaces
            fprintf(stderr,"ERROR, this is not a command!\n");
        }
        else if(sentence[0]==' ' || sentence[0]=='\n' || (sentence[strlen(sentence)-2]==' '&& sentence[strlen(sentence)-1]=='\n' )){
            numOfWords+=countNumOfWords(sentence);
            numOfCommands++;
            addToHistory(sentence);
            fprintf(stderr,"ERROR, this is not a command!\n");
        }
        else if ((badConditionsResult=badConditions(sentence))==0){
            return 0;//stop
        }
        else if(badConditionsResult==-1){//did not do anything
            if(runningCommand(sentence)==-1){//stop the problem.
                return -1;
            }
        }
    }
}

//return 1 if the sentence start with word "nohup"
int ifContainsNohup(char sentence []){
    if(strncmp(sentence,"nohup ",6)==0){
        int i=5;
        int length= strlen(sentence);
        for(;sentence[i]==' ';i++);
        int k=i;
        int j=0;
        for(;j<length-i;j++,k++){
            sentence[j]=sentence[k];
        }
        sentence[j]='\0';
        return 1;
    }
    return 0;
    //remove the nohup word from the sentence.
}

//check the edge conditions
//return 1 if the function did changes.
//return -1 if the function did not do anything.
//return 0 if we should leave the while loop.
int badConditions (char sentence []){
    if (strncmp(sentence,"done\n",5)==0) {
        numOfCommands++;
        printf("Num of commands: %d\n",numOfCommands);
        printf("number of words: %d\n",numOfWords);
        printf("see you Next time !\n");
        return 0;//stop the program.
    }
    else if (strncmp(sentence,"cd\n",3)==0) {
        numOfCommands++;
        numOfWords+=countNumOfWords(sentence);
        addToHistory(sentence);
        printf("command not supported (Yet)\n");
        return 1;//I did changes
    }
    else if (strncmp(sentence,"history\n",8)==0) {
        numOfWords+=countNumOfWords(sentence);
        numOfCommands++;
        addToHistory(sentence);
        if(printHistory()==-1)
            return 0;//stop the program.
        return 1;//I did changes
    }
    return -1;//I did not do anything.
}

//return the num of words in the sentence.
int countNumOfWords (char sentence []){
    int wordCounter=0;
    int wordIndex=0;
    int iFindCot=0;
    for(int t=0;sentence[t]!='\0';t++) {
        if(sentence[t]!=SPACE_CHAR && sentence[t]!='\n'){
            if(sentence[t]=='\"' && iFindCot==0 )
                iFindCot=1;
            else if(sentence[t]=='\"' && iFindCot==1 ) {
                iFindCot=0;
            }
            if(wordIndex==0){
                wordCounter++;//when we found the start of new word add 1 to the words counter.
            }
            wordIndex++;
        }
        else if((sentence[t]==SPACE_CHAR || sentence[t]=='\n') && iFindCot==0){
            wordIndex=0;
        }
    }
    return wordCounter;
}

//return the num of chars in the word.
int countNumOfCharsInOneWord (char sentence [],int indexOfWord, int hasCab){
    int charCounter=0;
    for(int i=indexOfWord; (sentence[i]!=SPACE_CHAR && sentence[i]!='\n'&& sentence[i]!='\0' && hasCab==0)|| hasCab==1;i++){
        if (sentence[i]=='\"' && hasCab==1)
            break;
        charCounter++;
    }
    return charCounter;
}

//the function finds the index of the next word in the sentence.
int nextWordIndex(char sentence [],int preWordIndex){
    int i;
    for(i=preWordIndex;sentence[i]!='\0';i++) {
        if(sentence[i]!=SPACE_CHAR && sentence[i]!='\n'){
            break;
        }
    }
    return i;
}

//find command from the history and changed the sentence to become the command that the function find.
//return -1 if the command is not in the history.
//return 0 if the command is in the history.
int commendFromHistory(char sentence[],int num) {
    char fileSentence[MAX_LENGTH_OF_SENTENCE] ;
    FILE *fPintHistory=fopen(HISTORY_FILE_PATH,"r");
    if(fPintHistory==NULL){
        FILE *f=fopen(HISTORY_FILE_PATH,"a");
        if(f==NULL){
            fprintf(stderr,"Error in opening file\n");
            strncpy(sentence,EMPTY_SENTENCE,2);
            exit(-1);
        }
        fclose(f);
    }
    else {
        int k;
        for (k = 1; !feof(fPintHistory); k++) {
            fgets(fileSentence, MAX_LENGTH_OF_SENTENCE, fPintHistory);
            if(k==num)
                break;
        }
        if(k<num ||num<=0){
            fprintf(stderr,"The command is not in the history!\n");
            fclose(fPintHistory);
            return -1;
        }
    }
    fclose(fPintHistory);
    strncpy(sentence,fileSentence,MAX_LENGTH_OF_SENTENCE);
    return 0;
}

//the function adds the command to the history file.
int addToHistory(char sentence []){
    FILE *fWriteHistory=fopen(HISTORY_FILE_PATH,"a");
    if(fWriteHistory==NULL){
        printf("Error in opening file\n");
        return -1;
    }
    fprintf(fWriteHistory,"%s",sentence);
    fclose(fWriteHistory);
    return 0;
}

//the function runs the command.
int runningCommand(char sentence[]){
    int numOfPipes=0;
    for(int t=0;sentence[t]!='\n';t++) {
        if(sentence[t] =='|')
            numOfPipes++;
    }
    if(numOfPipes==1){
        return runOnePipe(sentence);
    }
    else if(numOfPipes==2){
        return runTwoPipe(sentence);
    }
    else {
        return runZeroPipe(sentence);
    }
}

//the function checks if the sentence should play in the background, if yes, it changes the sentence.
//return 1 if the sentence should play in the background.
//return 0 otherwise.
int checkBackground(char sentence []){
    int ifPlayInBackGround=0;
    if(sentence[strlen(sentence)-2]=='&' )
        ifPlayInBackGround=1;
    if(ifPlayInBackGround==1){
        int length=strlen(sentence);
        length-=2;
        for(int j=length-1;sentence[j]==' ';j--)
            length--;
        sentence[length]='\n';
        sentence[length+1]='\0';
    }
    return ifPlayInBackGround;
}

//the function runs the one pipes command.
//return -1 if the something went wrong.
//return 1 if the function succeeded.
int runOnePipe(char sentence []){
    char command1[MAX_LENGTH_OF_SENTENCE] ;
    char command2[MAX_LENGTH_OF_SENTENCE] ;
    int index1=0;
    int index2=0;
    int i;
    for(i=0;sentence[i]!='|';i++){
        command1[index1]=sentence[i];
        index1++;
    }
    command1[index1]='\n';
    command1[index1+1]='\0';
    for(i=i+1;sentence[i]!='\n';i++){
        command2[index2]=sentence[i];
        index2++;
    }
    command2[index2]='\n';
    command2[index2+1]='\0';
    int check1=checkIfCallCommandFromHistory(command1);
    int check2=checkIfCallCommandFromHistory(command2);
    if(check1==1 || check2==1){
        int k=0;
        for(;k< strlen(command1)-1;k++){
            sentence[k]=command1[k];
        }
        sentence[k]='|';
        k++;
        int f=0;
        for(;f< strlen(command2);f++){
            sentence[k+f]=command2[f];
        }
        sentence[k+f]='\0';
        printf("%s",sentence);
    }
    int numOfPipes=0;
    for(int t=0;sentence[t]!='\n';t++) {
        if(sentence[t] =='|')
            numOfPipes++;
    }
    if(numOfPipes==2){
        return runTwoPipe(sentence);
    }
    else if(numOfPipes==0){
        return runZeroPipe(sentence);
    }
    else{
        numOfCommands++;
        numOfWords+= countNumOfWords(sentence);
        addToHistory(sentence);
        ifContainsNohup(command1);// if there a nohup word in the head of the sentence, remove it.
        int ifPlayInBackGround= checkBackground(command2);
        int numOfWordsInCommand1=countNumOfWords(command1);
        char * command1Array[numOfWordsInCommand1+1];
        if(fillCommandArray(command1,command1Array,numOfWordsInCommand1)==-1){
            freeTheArray(command1Array,numOfWordsInCommand1);
            fprintf(stderr,"error in filling the array!");
            return -1;
        }

        int numOfWordsInCommand2=countNumOfWords(command2);
        char * command2Array[numOfWordsInCommand2+1];
        if(fillCommandArray(command2,command2Array,numOfWordsInCommand2)==-1){
            freeTheArray(command1Array,numOfWordsInCommand1);
            freeTheArray(command2Array,numOfWordsInCommand2);
            return -1;
        }
        int pipefd[2];
        int status;
        pid_t leftpid, rightpid;
        pipe(pipefd);
        if ((leftpid = fork()) == 0) {
            prctl(PR_SET_PDEATHSIG,SIGHUP);
            dup2(pipefd[1], STDOUT_FILENO);
            if(execvp(command1Array[0], command1Array)==-1){
                if(strncmp(command1Array[0],"history",6)==0){
                    if(printHistory()==-1)
                        return -1;//stop the program.
                }
                else
                    fprintf(stderr,"ERROR IN THE FIRST COMMAND!\n");
            }
            close(pipefd[0]);
            close(pipefd[1]);
            exit(0);
        }
        else if (leftpid == -1) {
            close(pipefd[0]);
            close(pipefd[1]);
            perror("ERROR IN SYSTEM CALL");
            freeTheArray(command1Array,numOfWordsInCommand1);
            freeTheArray(command2Array,numOfWordsInCommand2);
            return -1;
        }
        else if ((rightpid = fork()) == 0) {
            prctl(PR_SET_PDEATHSIG,SIGHUP);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            close(pipefd[1]);
            if(execvp(command2Array[0], command2Array)==-1){
                if(strncmp(command2Array[0],"history",6)==0){
                    if(printHistory()==-1)
                        return -1;//stop the program.
                }
                else
                    fprintf(stderr,"ERROR IN THE SECOND COMMAND!\n");
            }
            exit(0);
        }
        else if (rightpid == -1) {
            perror("ERROR IN SYSTEM CALL");
            freeTheArray(command1Array,numOfWordsInCommand1);
            freeTheArray(command2Array,numOfWordsInCommand2);
            return -1;
        }
        else {
            close(pipefd[0]);
            close(pipefd[1]);
            if(!ifPlayInBackGround){
                wait(&status);
                wait(&status);
            }
        }
        freeTheArray(command1Array,numOfWordsInCommand1);
        freeTheArray(command2Array,numOfWordsInCommand2);
    }

    return 0;
}

//the function runs the two pipes command.
//return -1 if the something went wrong.
//return 1 if the function succeeded.
int runTwoPipe(char sentence []){
    char command1[MAX_LENGTH_OF_SENTENCE] ;
    char command2[MAX_LENGTH_OF_SENTENCE] ;
    char command3[MAX_LENGTH_OF_SENTENCE] ;
    int index1=0;
    int index2=0;
    int index3=0;
    int i;
    for(i=0;sentence[i]!='|';i++) {
        command1[index1]=sentence[i];
        index1++;
    }
    command1[index1]='\n';
    command1[index1+1]='\0';
    for(i=i+1;sentence[i]!='|';i++) {
        command2[index2]=sentence[i];
        index2++;
    }
    command2[index2]='\n';
    command2[index2+1]='\0';
    for(i=i+1;sentence[i]!='\n';i++) {
        command3[index3]=sentence[i];
        index3++;
    }
    command3[index3]='\n';
    command3[index3+1]='\0';
    int check1=checkIfCallCommandFromHistory(command1);
    int check2=checkIfCallCommandFromHistory(command2);
    int check3=checkIfCallCommandFromHistory(command3);
    if(check1==1 || check2==1 || check3==1 ){
        int k=0;
        for(;k< strlen(command1)-1;k++){
            sentence[k]=command1[k];
        }
        sentence[k]='|';
        k++;
        int l=0;
        for(;l< strlen(command2)-1;l++){
            sentence[l+k]=command2[l];
        }
        sentence[l+k]='|';
        l++;
        int f=0;
        for(;f< strlen(command3);f++){
            sentence[l+k+f]=command3[f];
        }
        sentence[k+f+l]='\0';
        printf("%s",sentence);
    }
    int numOfPipes=0;
    for(int t=0;sentence[t]!='\n';t++) {
        if(sentence[t] =='|')
            numOfPipes++;
    }
    if(numOfPipes==1){
        return runOnePipe(sentence);
    }
    else if(numOfPipes==0){
        return runZeroPipe(sentence);
    }
    else{
        numOfCommands++;
        numOfWords+= countNumOfWords(sentence);
        addToHistory(sentence);
        ifContainsNohup(command1);// if there a nohup word in the head of the sentence, remove it.
        int ifPlayInBackGround= checkBackground(command3);
        int numOfWordsInCommand1=countNumOfWords(command1);
        char * command1Array[numOfWordsInCommand1+1];
        if(fillCommandArray(command1,command1Array,numOfWordsInCommand1)==-1){
            freeTheArray(command1Array,numOfWordsInCommand1);
            return -1;
        }
        int numOfWordsInCommand2=countNumOfWords(command2);
        char * command2Array[numOfWordsInCommand2+1];
        if(fillCommandArray(command2,command2Array,numOfWordsInCommand2)==-1){
            freeTheArray(command1Array,numOfWordsInCommand1);
            freeTheArray(command2Array,numOfWordsInCommand2);
            return -1;
        }

        int numOfWordsInCommand3=countNumOfWords(command3);
        char * command3Array[numOfWordsInCommand3+1];
        if(fillCommandArray(command3,command3Array,numOfWordsInCommand3)==-1){
            freeTheArray(command1Array,numOfWordsInCommand1);
            freeTheArray(command2Array,numOfWordsInCommand2);
            freeTheArray(command3Array,numOfWordsInCommand3);
            return -1;
        }

        int pipefd1[2];
        int pipefd2[2];
        int status;
        pid_t leftpid, middlepid ,rightpid;
        pipe(pipefd1);
        pipe(pipefd2);
        if ((leftpid = fork()) == 0) {
            prctl(PR_SET_PDEATHSIG,SIGHUP);
            dup2(pipefd1[1], STDOUT_FILENO);
            close(pipefd2[0]);
            close(pipefd2[1]);
            close(pipefd1[0]);
            close(pipefd1[1]);
            if(execvp(command1Array[0], command1Array)==-1){
                if(strncmp(command1Array[0],"history",6)==0){
                    if(printHistory()==-1)
                        return -1;//stop the program.
                }
                else
                    fprintf(stderr,"ERROR IN THE FIRST COMMAND!\n");
            }
            exit(1);
        }
        else if (leftpid == -1) {
            close(pipefd2[0]);
            close(pipefd2[1]);
            close(pipefd1[0]);
            close(pipefd1[1]);
            perror("ERROR IN SYSTEM CALL");
            freeTheArray(command1Array,numOfWordsInCommand1);
            freeTheArray(command2Array,numOfWordsInCommand2);
            freeTheArray(command3Array,numOfWordsInCommand3);
            return -1;
        }
        else if ((middlepid = fork()) == 0) {
            prctl(PR_SET_PDEATHSIG,SIGHUP);
            dup2(pipefd1[0], STDIN_FILENO);
            dup2(pipefd2[1], STDOUT_FILENO);
            close(pipefd1[0]);
            close(pipefd1[1]);
            close(pipefd2[0]);
            close(pipefd2[1]);
            if (execvp(command2Array[0], command2Array)==-1) {
                if(strncmp(command2Array[0],"history",6)==0){
                    if(printHistory()==-1)
                        return -1;//stop the program.
                }
                else
                    fprintf(stderr,"ERROR IN THE SECOND COMMAND!\n");
            }
            exit(1);
        }
        else if (middlepid == -1) {
            close(pipefd1[0]);
            close(pipefd1[1]);
            close(pipefd2[0]);
            close(pipefd2[1]);
            perror("ERROR IN SYSTEM CALL");
            freeTheArray(command1Array,numOfWordsInCommand1);
            freeTheArray(command2Array,numOfWordsInCommand2);
            freeTheArray(command3Array,numOfWordsInCommand3);
            return -1;
        }
        else if ((rightpid = fork()) == 0) {
            prctl(PR_SET_PDEATHSIG,SIGHUP);
            close(pipefd1[0]);
            close(pipefd1[1]);
            dup2(pipefd2[0], STDIN_FILENO);
            close(pipefd2[0]);
            close(pipefd2[1]);
            if(execvp(command3Array[0], command3Array)==-1) {
                if(strncmp(command3Array[0],"history",6)==0){
                    if(printHistory()==-1)
                        return -1;//stop the program.
                }
                else
                    fprintf(stderr,"ERROR IN THE Third COMMAND!\n");
            }
            exit(0);
        }
        else if (rightpid == -1) {
            perror("ERROR IN SYSTEM CALL");
            freeTheArray(command1Array,numOfWordsInCommand1);
            freeTheArray(command2Array,numOfWordsInCommand2);
            freeTheArray(command3Array,numOfWordsInCommand3);
            return -1;
        }
        else {
            close(pipefd1[0]);
            close(pipefd1[1]);
            close(pipefd2[0]);
            close(pipefd2[1]);
            if(!ifPlayInBackGround){
                wait(&status);
                wait(&status);
                wait(&status);
            }
        }
        freeTheArray(command1Array,numOfWordsInCommand1);
        freeTheArray(command2Array,numOfWordsInCommand2);
        freeTheArray(command3Array,numOfWordsInCommand3);
    }
    return 0;
}

//the function runs the zero pipes command.
//return -1 if the something went wrong.
//return 1 if the function succeeded.
int runZeroPipe(char sentence []){
    int status;
    int check1=checkIfCallCommandFromHistory(sentence);
    if(check1==1){
        printf("%s",sentence);
    }
    int numOfPipes=0;
    for(int t=0;sentence[t]!='\n';t++) {
        if(sentence[t] =='|')
            numOfPipes++;
    }
    if(numOfPipes==1){
        return runOnePipe(sentence);
    }
    if(numOfPipes==2){
        return runTwoPipe(sentence);
    }
    else{
        numOfCommands++;
        numOfWords+= countNumOfWords(sentence);
        addToHistory(sentence);
        int ifPlayInBackGround = checkBackground(sentence);
        int ifPlayNoHup= ifContainsNohup(sentence);
        int numOfWordsInCommand = countNumOfWords(sentence);
        char *commandArray[numOfWordsInCommand + 1];
        if(fillCommandArray(sentence,commandArray,numOfWordsInCommand)==-1){
            freeTheArray(commandArray,numOfWordsInCommand);
            return -1;
        }

        if(ifPlayNoHup){
            signal(SIGHUP,SIG_IGN);
            int fd=open("nohup.txt",O_WRONLY | O_CREAT | O_APPEND ,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
            pid_t x = fork();
            if (x == -1) {
                perror("ERROR IN SYSTEM CALL");
                freeTheArray(commandArray,numOfWordsInCommand);
                close(fd);
                return -1;
            }
            else if (x == 0) {//if I am in the child process run the command.
                prctl(PR_SET_PDEATHSIG,SIGHUP);
                if(dup2(fd, STDOUT_FILENO)==-1){
                    perror("DUP2 FAILLED\n");
                    exit(-1);
                }
                if (execvp(commandArray[0], commandArray) == -1) {//if the command did not run
                    if(strncmp(commandArray[0],"history",6)==0){
                        if(printHistory()==-1)
                            return -1;//stop the program.
                    }
                    else
                        fprintf(stderr,"ERROR IN THE COMMAND!\n");
                }
                exit(0);
            }
            else{
                freeTheArray(commandArray,numOfWordsInCommand);
            }
            close (fd);
        }
        else{
            pid_t x = fork();
            if (x == -1) {
                perror("ERROR IN SYSTEM CALL");
                freeTheArray(commandArray,numOfWordsInCommand);
                return -1;
            }
            else if (x == 0) {//if I am in the child process run the command.
                prctl(PR_SET_PDEATHSIG,SIGHUP);
                if (execvp(commandArray[0], commandArray) == -1) {//if the command did not run
                    if(strncmp(commandArray[0],"history",6)==0){
                        if(printHistory()==-1)
                            return -1;//stop the program.
                    }
                    else
                        fprintf(stderr,"ERROR IN THE COMMAND!\n");
                }
                exit(0);
            }
            else{
                if(!ifPlayInBackGround) {
                    wait(&status);//the dad process wait the child process
                }
                freeTheArray(commandArray,numOfWordsInCommand);
            }
        }
    }
    return 0;
}

//the function prints the history
int printHistory (){
    char fileSentence[MAX_LENGTH_OF_SENTENCE] ;
    FILE *fPintHistory=fopen(HISTORY_FILE_PATH,"r");
    if(fPintHistory==NULL){
        FILE *f=fopen(HISTORY_FILE_PATH,"a");
        if(f==NULL){
            fprintf(stderr,"Error in opening file\n");
            return -1;
        }
        fclose(f);
    }
    else {
        for (int k = 1; !feof(fPintHistory); k++) {
            strcpy(fileSentence,EMPTY_SENTENCE);
            fgets(fileSentence, MAX_LENGTH_OF_SENTENCE, fPintHistory);
            if(strncmp(fileSentence,EMPTY_SENTENCE,1)!=0)
                printf("%d: %s",k,fileSentence);//if the string is not an empty string then add the string to the history
        }
    }
    fclose(fPintHistory);
    return 0;
}

//the function checks if the command is !<num> and if yes the function runs the command.
int checkIfCallCommandFromHistory(char sentence[]){
    int i=0;
    int l=0;
    int length=0;
    for(;sentence[i]==' ';i++);
    if(sentence[i]=='!' ) {
        for (l = i+1; l < strlen(sentence); l++) {
            if ((sentence[l] <= '9' && sentence[l] >= '0')) {
                length++;
                if (sentence[l+1] =='\n'||sentence[l+1]==' ') {
                    break;
                }
            }
            else
                return -1;
        }
        char num[length+1];
        int f;
        for (f = 0; f<length; f++) {
            num[f] = sentence[i+1];
            i++;
        }
        num[length] = '\0';
        int intNum = atoi(num); // NOLINT(cert-err34-c)
        if(commendFromHistory(sentence, intNum)==-1) {//the command is not in the history.
            return 0;
        }
        else {
            return 1;
        }
    }
    else{
        return  -1;
    }
}
//fill the command array
int fillCommandArray(char command [],char * commandArray[],int numOfWords){
    int wordIndex;
    int preWordIndex=0;
    int numOfChars=0;
    int hasCab=0;
    commandArray[numOfWords]=NULL;
    for(int j=0;j<numOfWords;j++) {
        wordIndex=nextWordIndex(command,preWordIndex+numOfChars);
        if(command[wordIndex]=='\"'){
            hasCab=1;
            wordIndex++;
        }
        numOfChars=countNumOfCharsInOneWord(command,wordIndex,hasCab);
        commandArray[j]=(char*) calloc(numOfChars+1 ,sizeof (char));
        if(commandArray[j]==NULL){//if the memory allocate fail.
            for(int k=0;k<=j;k++){
                free(commandArray[k]);
                return -1;
            }
        }
        for(int k=0;k<numOfChars;k++)
            commandArray[j][k]=command[wordIndex+k];
        preWordIndex=wordIndex;
        commandArray[j][numOfChars]='\0';
    }
    return 0;
}

//free the dynamic array.
void freeTheArray(char * commandArray[],int numOfWords){
    for (int j = 0; j < numOfWords; j++) {//free the dynamic array
        free(commandArray[j]);
    }
}

//the handler that the signal function called when it receive child signal.
//the function kills the child to avoid the zombie state when the child run in the background.
void proc_exit(int signal) {
    int waitPidR=1;
    while(waitPidR>0){
        waitPidR=waitpid(-1,&signal,WNOHANG);
    }
}
