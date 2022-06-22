#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

#define POLYNOMIAL_LENGTH 100
#define MAX_DEGREE 3
#define STOP "done"
//global values to use in the program.
int powers[MAX_DEGREE];
int parameters[MAX_DEGREE];
int polynomialDeg = 0, freeValue = 0, variablesNum = 0, valueToPlace = 0;
bool inValid = false;
char reason[50];
//defining functions.
void PolynomialsSolve();
void isValid(const char *);
void parsePolynomial(const char *);
void parsePart(char *);
int calculateEquation();
int calculatePart(int);
/*
 * the main function that contains the main loop of the program.
 * receiving polynomials from the user until identify the word "done".
 */
void PolynomialsSolve(){
    char input[POLYNOMIAL_LENGTH];
    while(1){
        puts("Please enter a Polynomial and a value in format \"Polynomial, Value\" or done to exit: ");
        if(fgets(input,POLYNOMIAL_LENGTH,stdin) == NULL){ //receiving from the user.
            fprintf(stderr,"gets() failed");
            exit(EXIT_FAILURE);
        }
        fflush(stdin);//clear the buffer and accept the next string.
        input[strlen(input)-1] = '\0';//replacing the '\n' by '\0'.
        if(strcmp(input,STOP) == 0) break;
        isValid(input);
        if(inValid){//check if the polynomial is valid.
            fprintf(stderr,"%s\n",reason);
            inValid = false;
            continue;
        }
        parsePolynomial(input);
        if(variablesNum > 0)
           printf("Result: %d\n",  calculateEquation());
        else printf("Result: %d\n",  freeValue);
    }
}
/*
 * function that parse a part of the polynomial into the global arrays.
 * receives the part by parsePolynomial function.
 */
void parsePart(char *part){
    int power, parameter = 1, isVariable = 0;
    for(int i = 0; part[i] != '\0'; i++){
        if(part[i] == 'x'){
            variablesNum++;
            isVariable = 1;
            if(part[i+1] != '^' ) power = 1; //case if x without degree.
            else power = (int)strtol(part+i+2,NULL,10);
            if(polynomialDeg == 0) polynomialDeg = power;
            powers[power-1] = power;//saving the degree at the equal index.
            parameters[power-1] = parameter;//saving the factor at the equal index.
            parameter = 1;
        }
        else if(part[i] == '*')
            parameter = (int)strtol(part,NULL,10);
    }
    if(!isVariable)
        freeValue = (int)strtol(part,NULL,10);
}
/*
 * function that cut each part of the polynomial and send to parsePart function.
 * receives all the equation.
 */
void parsePolynomial(const char * equation){
    char *pol;
    for(int i = 0; i < MAX_DEGREE; i++){ //in the beginning the powers is 0 and the parameters is 1.
        powers[i] = 0; parameters[i] = 1;
    }
    valueToPlace = (int)strtol(strchr((char*)equation,',')+2,NULL,10);
    pol = strtok((char*)equation, ",");
    if(strchr(pol,'+') != NULL){
        char *part = strtok(pol,"+");;
        while(part != NULL){
            parsePart(part);
            part = strtok(NULL,"+");
        }
    } else parsePart(pol);
}
/*
 *function that checks the polynomial and decided if the input is valid.
 * checks the spaces and if contains a degree that larger than 3.
 */
void isValid(const char *equation){
    int power;
    for(int i = 0; equation[i] != '\0'; i++) {//passing on the polynomial.
        if ((equation[i] == ' ' && equation[i - 1] != ',') || (equation[i] == ',' && equation[i + 1] != ' ')) {//check spaces.
            inValid = true;
            strcpy(reason,"Spaces that not allowed!");
            break;
        }
        if(equation[i] == '^'){//check degree.
            power = (int)strtol(equation+i+1,NULL,10);
            if(!(power > 0 && power < 4)){
                inValid = true;
                strcpy(reason,"Polynomial degree is not allowed!");
                break;
            }
        }
    }
}
/*
 * function that calculate the value of a specific part from the polynomial.
 * receives index that
 */
int calculatePart(int index){
    int res = parameters[index];
    for(int i = 0; i < powers[index]; i++) res *= valueToPlace;
    return res;
}
/*
 * function that do the calculation of the result by initialize a shared memory, new childes processes  to calculate each part and save into the shared memory.
 */
int calculateEquation(){
    int shm_id, *ptr, result = freeValue;
    pid_t child;
    key_t key = ftok("/tmp",'y'); // get the key for the memory.
    if((shm_id = shmget(key,variablesNum*sizeof(int),IPC_CREAT | 0600)) == -1){//initializing the shared memory.
        perror("failed to get memory");
        exit(EXIT_FAILURE);
    }
    if((ptr = (int*) shmat(shm_id,NULL,0)) < 0){//attaching to the shared memory.
        perror("failed to attach memory");
        exit(EXIT_FAILURE);
    }
    for(int i = 0, k = 0; i < variablesNum; i++){
        while(powers[k] == 0 && k < MAX_DEGREE) k++;
        if((child = fork()) < 0){//child process.
            perror("fork() failed");
            exit(EXIT_FAILURE);
        }
        else if(child == 0){
            ptr[i] = calculatePart(k);//calculating the polynomial part.
            exit(EXIT_SUCCESS);
        }
        else wait(NULL);
        k++;
    }
    for(int i = 0; i < variablesNum; i++){
        result += ptr[i];
    }
    shmdt(ptr);//detach the shared memory(delete the pointer).
    if(shmctl(shm_id,IPC_RMID, NULL) == -1){//free the shared memory.
        perror("shm ctl() failed");
        exit(EXIT_FAILURE);
    }
    variablesNum = 0; polynomialDeg = 0, freeValue = 0;
    return result;
}
int main(){
    PolynomialsSolve();
    return 0;
}