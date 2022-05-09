ex3
written by Assma Zurba 323139782

==description==
the program receives a command from the user,
and checks if the command is legal, if it is legal
the program runs the command,else the program prints
an error message. to end the program the user should write done,
so the program prints the num of commands that
the user writes (everything that the user write count
as a command, if the user writes space or enter
it does not count as a command if there is a sentence in a
quotations mark the program will count it as one word.
the program saves all these commands in the history file.
and if the user writes!<number> the program runs the command
that has been saved in the line number <number>.
(<number> should be bigger than 0).
the program support running a command in the background.
the program supports the nohup command:
if nohup is written before a command, the command result will
be written in a file called nohup.txt, and the command will run even
if we close the shell.

==functions==

void freeTheArray(char * commandArray[],int numOfWords)
parameters:
commandArray- the dynamic array that contains the command.
numOfWords- the num of items in the command array.
free the dynamic array.

int countNumOfWords (char sentence [])
parameters:
sentence-the command that the user write.
return the num of words in the sentence.

int countNumOfCharsInOneWord (char sentence [],int indexOfWord)
parameters:
sentence-the command that the user write.
indexOfWord-the index of the word in the sentence.
return the num of chars in the word.

int nextWordIndex(char sentence [],int preWordIndex)
parameters:
sentence-the command that the user write.
preWordIndex-the index of the previous word  in the sentence.
return the index of the next word in the sentence.

int runningCommand(char sentence[])
parameters:
sentence-the command that the user write.
the function is running the command.
return -1 if the program should end.
return 0 if the function succeed.

int printHistory()
the function prints the history
return -1 if there is an error in opening the file.
return 0 if the function succeed.

int commendFromHistory( char sentence[],int num)
parameters:
sentence-the command that the user write.
num-the line of the command in the history file.
return -1 if the command is not in the history.
return 0 if the command is in the history.
find the command from the history and changed the sentence
to become the command that the function find.

int addToHistory(char sentence [])
parameters:
sentence-the command that the user write.
the function adds the command to the history file.
return -1 if there is an error in opening the file.
return 0 if the function succeed

int checkIfCallCommandFromHistory(char sentence[])
parameters:
sentence-the command that the user write.
the function checks if the command is !<num> and if yes the function runs the command number <num> in the history file.
return -1 if it was not calling a command in the history.
return 1 if the command in the history and the command !<num> succeeded.
return 0 if the command is not in the history.

int fillCommandArray(char command [],char * commandArray[],int numOfWords)
parameters:
command- string
command Array- array that we want to fill.
numOfWords- the num of words in the command.
fill the command array to send to exevp function.

int badConditions (char sentence [])
parameters:
sentence- string
check the edge conditions
return 1 if the function did changes.
return -1 if the function did not do anything.
return 0 if we should leave the while loop.

int runOnePipe(char sentence [],int ifPlayInBackGround)
parameters:
sentence- string
ifPlayInBackGround- 1:if the command should run in the background,0:otherwise.
the function runs the one pipe command.
return -1 if the something went wrong.
return 0 if the function succeeded.

int runTwoPipe(char sentence [],int ifPlayInBackGround)
parameters:
sentence- string
ifPlayInBackGround- 1:if the command should run in the background,0:otherwise.
the function runs the two pipes command.
return -1 if the something went wrong.
return 0 if the function succeeded.

int runZeroPipe(char sentence [],int ifPlayInBackGround)
parameters:
sentence- string
ifPlayInBackGround- 1:if the command should run in the background,0:otherwise.
the function runs the zero pipes command.
return -1 if the something went wrong.
return 0 if the function succeeded.

int ifContainsNohup(char sentence [])
parameters:
sentence- string
return 1 if the sentence start with word "nohup"
return 0 otherwise.

void proc_exit(int signal)
parameters:
signal- the signal that called the function.
the handler that the signal function called when it receive child signal.
the function kills the child to avoid the zombie state when the child run in the background.

int checkBackground(char sentence [])
parameters:
sentence: string.
the function checks if the sentence should play in the background, if yes, it changes the sentence.
return 1 if the sentence should play in the background.
return 0 otherwise.

==Program files==
main.c

==How yo compile==
compile: gcc main.c -o main

==input==
string from user

==output==
file that contains the history: file.txt
file that contains the nohup commands result: nohup.txt
the num of commands that the user writes
the num of words that the user writes

