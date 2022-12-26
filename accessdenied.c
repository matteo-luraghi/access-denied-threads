#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<string.h>

sem_t stop, go;
char* globguess;
char globres[20];
int TRIES = 2500;
char pssw1[20] = "Hunter2";

typedef char data;
struct nodo {
    data el;
    struct nodo *next;
};

typedef struct nodo *lista;

int lunghezza(lista l);
lista inserisci_testa (lista l, data el);
lista inserisci_coda (lista l, data el);
void stampa_lista (lista l);
int conta_lista (lista l);

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

int lunghezza(lista l) {
    if (l==NULL) { return 0;}
    else {return 1 + lunghezza (l->next);}
}

lista inserisci_testa (lista l, data el) {
    struct nodo *temp = malloc(sizeof(struct nodo));
    temp->el = el;
    temp->next = l;
    return temp;
}

lista inserisci_coda (lista l, data el) {
    if (l == NULL) {return inserisci_testa(l,el);}
    else {
        l->next = inserisci_coda(l->next,el);
        return l;
    }
}

void stampa_lista (lista l) {
    if(l != NULL) {
        printf("%c",l->el);
        stampa_lista(l->next);
    }
    else {printf("\n");}
}

int updateChar(char charact) {
    int num = (int) charact;
    if (num == 57){return 65;}
    if (num == 90){return 97;}
    if (num == 122){return 48;}
    return num+1;
}

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

void* guesser() {
    char start = '0';
    lista guess = NULL;
    guess = inserisci_testa(guess, start);
    //stampa_lista(guess);
    for (int h=0;h<TRIES;h++) {
        int found = -1;
        int reslen = 0;
        char* time = malloc(sizeof(char));
        lista idx = guess;
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
        //stampa_lista(guess);
        sem_post(&stop);
        sem_wait(&go);
        //scanf("%[^\n]%*c",res);
        if(strcmp(globres, "ACCESS GRANTED")==0) {return NULL;}
        //reading the time in the ACCESS DENIED
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
            guess = inserisci_coda(guess, start);
        }
        else if((atoi(time)-14)%9 == 0) {
            int correct = (atoi(time)-14)/9;
            lista new = guess;
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

int conta_lista (lista l) {
    if (l != NULL) {
        return 1 + conta_lista(l->next);
    }
    else {
        return 0;
    }
}
