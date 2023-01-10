#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<string.h>

//semaphores to control the threads
sem_t stop, go;

//global variables
char* globguess;
char globres[20];
int TRIES = 2500;
char pssw1[20] = "Hunter2";

//list initialization
typedef char data;
struct node {
    data el;
    struct node *next;
};

typedef struct node *list;

//functions relative to the list data structure
int length(list l);
list insert_head (list l, data el);
list insert_tail (list l, data el);
void print_list (list l);
int count_list (list l);

//functions
int updateChar(char charact);
int checkPssw();
void* vault();
void* guesser();

int main() {
    int exitVault, exitGuesser;
    pthread_t thguesser, thvault;
    sem_init(&stop, 0, 0);
    sem_init(&go, 0, 0);
    pthread_create(&thguesser,NULL,&guesser,NULL);
    pthread_create(&thvault,NULL,&vault,NULL);
    pthread_join(thguesser, NULL);
    pthread_join(thvault, NULL);
    return 0;
}

//returns the length of the lsit
int length(list l) {
    if (l==NULL) { return 0;}
    else {return 1 + length (l->next);}
}

//inserts an element to as the new head of the list
list insert_head (list l, data el) {
    struct node *temp = malloc(sizeof(struct node));
    temp->el = el;
    temp->next = l;
    return temp;
}

//appends an element to the tail of the list
list insert_tail (list l, data el) {
    if (l == NULL) {return insert_head(l,el);}
    else {
        l->next = insert_tail(l->next,el);
        return l;
    }
}

//prints the list
void print_list (list l) {
    if(l != NULL) {
        printf("%c",l->el);
        print_list(l->next);
    }
    else {printf("\n");}
}

//updates the current character to be an upper letter, a lower letter or a punctuation symbol
int updateChar(char charact) {
    int num = (int) charact;
    if (num == 57){return 65;}
    if (num == 90){return 97;}
    if (num == 122){return 48;}
    return num+1;
}

//checks if the password is correct
int checkPssw() {
    int time = 0;
    time = time + 4;
    if(strlen(pssw1)!=strlen(globguess)) {        
        time++;
        return time;
    }
    for(int j=0;j<strlen(pssw1);j++) {
        time = time + 9;
        if(pssw1[j]!=globguess[j]) { 
            time++;
            return time;
        }   
    }    
    free(globguess);
    time++;
    return 0;
}

//checks if the password is correct simulating the time constraints of the assignment (kattis problem)
void* vault() {
    int tries = TRIES;
    int time = 1;
    for(int i=0;i<tries && time != 0;i++) {
        sem_wait(&stop);
        time = checkPssw();
        if(time != 0) {sprintf(globres, "ACCES DENIED (%d ms)",time);}
        else {strcpy(globres, "ACCESS GRANTED");
            printf("ACCESS GRANTED\n");}
        sem_post(&go);
    }
}

//checks the lenght of the password and then starts to guess the password updating the characters every time
void* guesser() {
    char start = '0';
    list guess = NULL;
    guess = insert_head(guess, start);
    for (int h=0;h<TRIES;h++) {
        int found = -1;
        int reslen = 0;
        char* time = malloc(sizeof(char));
        list idx = guess;
        while(idx != NULL) {
            if(reslen == 0) {
                globguess = malloc(sizeof(char));
            }
            else {
                globguess = realloc(globguess, (strlen(globguess)+sizeof(char)));
            }
            globguess[reslen] = idx->el;
            reslen++;
            idx = idx->next;
        }
        globguess[reslen] = '\0';
        printf("GUESS: %s\n",globguess);
        sem_post(&stop);
        sem_wait(&go);
        if(strcmp(globres, "ACCESS GRANTED")==0) {return NULL;}
        for(int i=0;i<strlen(globres) && found!=-2;i++) {
            if (found>=0) {
                if(globres[i] == ' ') {
                    found = -2;
                    time[found] = '\0';
                }
                else {
                    time[found] = globres[i];
                    found++;
                    time = (char*)realloc(time, sizeof(char));
                }
            }
            if (globres[i]=='(') {found = 0;}
        }
        if(atoi(time)==5) {
            guess = insert_tail(guess, start);
        }
        else if((atoi(time)-14)%9 == 0) {
            int correct = (atoi(time)-14)/9;
            list new = guess;
            int count = 0;
            while (new != NULL) {
                if(count >= correct) {
                    new->el = (char)(updateChar(new->el));
                }
                count++;
                new = new->next;
            }
        }
    }
}

//counts the elements inside a list
int count_list (list l) {
    if (l != NULL) {
        return 1 + count_list(l->next);
    }
    else {
        return 0;
    }
}
