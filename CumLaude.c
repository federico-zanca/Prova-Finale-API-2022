#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
typedef struct nodo {
    bool candidate;
    struct nodo *left, *right, *next, *prossimo;
    char *word;
} node_t;
typedef node_t *node;
typedef struct elemento{
    struct elemento *next;
    node nodo;
    char* guess;
}elem_t;
typedef elem_t *elem;
int occ[78]={0};
void mystrcpy(char *s, char const *t){
    int i=0;
    while(t[i]!='\0') {
        s[i]=t[i];
        i++;
    }
    s[i]='\0';
}
elem headInsert(elem list, node z, int k, char *sign){
    elem head;
    head=(elem)malloc(sizeof(*head)+k+1);
    head->guess=(char*)(head+1);
    head->nodo=z;
    mystrcpy(head->guess, sign);
    head->next=list;
    return head;
}
bool precede(const char *s, const char *t, int k) {
    int i;
    for (i = 0; i < k; i++) {
        if (s[i] > t[i]) return false;
        if (s[i] < t[i]) return true;
    }
    return true;
}
node rbInsert(node *root, node z, int k, bool candy) {
    z->candidate = candy;
    z->left = z->right = NULL;
    node y = NULL;
    node x = *root;
    while (x) {
        y = x;
        if (precede(z->word, x->word, k)) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    if (y == NULL) (*root) = z;
    else if (precede(z->word, y->word, k))  y->left = z;
    else    y->right = z;
    z->left = NULL;
    z->right = NULL;
    return (*root);
}
void treeRewind(node last){
    if(!last) return;
    last->candidate=true;
    treeRewind(last->next);
}
void inOrderPrint(node root){
    if(!root) return;

    inOrderPrint(root->left);
    if(root->candidate) printf("%s\n", root->word);
    inOrderPrint(root->right);
}
node treeFind(node root, char* tmp, int k){
    node radice;
    radice=root;
    while(radice && strcmp(radice->word, tmp)!=0){
        if(precede(tmp, radice->word, k))   radice=radice->left;
        else    radice = radice->right;
    }
    return radice;
}
void treeReset(node root) { //falla iterativa
    if (root)   root->candidate = true;
    if (root->left) treeReset(root->left);
    if (root->right) treeReset(root->right);
}
void check(char const *rif, char const *tmp, char *sign, int k ) {
    int i;
    for (i = 0; i < k; i++) {
        if (tmp[i] == rif[i])   sign[i] = '+';
        else {
            sign[i] = '/';
            occ[rif[i] - 45] += 1;
        }
    }
    for (i = 0; i < k; i++) {
        if (sign[i] == '/' && (occ[tmp[i] - 45] > 0) ) {
                sign[i] = '|';
                occ[tmp[i] - 45] -= 1;
        }
    }
    for (i = 0; i < k; i++) occ[rif[i] - 45] = 0;
    sign[k] = '\0';
}
void azzeraOccorrenze(const int *usati, int x){
    for(int j = 0; j < x; j++)  occ[usati[j]] = 0;
}
bool checkfast(const char *rif, const char *word, const char* sign, int k){
    int usati[k], x=0;
    for(int i = 0; i < k; i++) {
        if(sign[i] == '+' && (word[i] != rif[i])) {
                azzeraOccorrenze(usati, x);
                return false;
        }
        else{
            if(word[i] == rif[i]){
                azzeraOccorrenze(usati, x);
                return false;
            }
            usati[x] = rif[i] - 45;
            ++x;
            occ[rif[i] - 45]++;
        }
    }
    for(int i = 0; i < k; i++) {
        if((sign[i] == '/' && occ[word[i] - 45] > 0) || (sign[i] == '|' && occ[word[i] - 45] == 0)){
            azzeraOccorrenze(usati, x);
            return false;
        }
        else if(sign[i] == '|' && occ[word[i] - 45] > 0)    occ[word[i] - 45]--;
    }
    azzeraOccorrenze(usati, x);
    return true;
}
bool filtraLista(elem list, char *dainserire, int k){
    elem z=list;
    while(z){
        if(!checkfast(dainserire, z->nodo->word, z->guess, k)) return false;
        z=z->next;
    }
    return true;
}
void listDestroy(elem list){
    if(list->next) listDestroy(list->next);
    free(list);
    list=NULL;
}
void treeKill(node root) {
    if (!root) {
        free(root);
        root = NULL;
        return;
    }
    treeKill(root->next);
}
int filtraParole(char* tmp, char *sign, int k, node *last) {
    node z, x;
    int count=0;
    z= (*last);
    x=NULL;
    while(z){
        if(z->candidate){
            if(z->candidate){
                if(!checkfast(z->word, tmp, sign, k)){
                    z->candidate=false;
                    if(x) x->prossimo=z->prossimo;
                }
                else{
                    count++;
                }
            }
        }
        if(z->candidate) x=z;
        z= z->prossimo;
    }
    return count;
}

void inserisciInizio(node *root, char *tmp, int k, elem list, bool special, node *last){
    int trash;
    bool candy=true;
    node z;
    trash=scanf("%s\n", tmp);
    while(tmp[0]!='+'){
        if(special)     candy=filtraLista(list, tmp, k);
        z=malloc(sizeof(*z)+k+1);
        z->word=(char *)(z+1);
        mystrcpy(z->word, tmp);
        z->next=(*last);
        z->prossimo=(*last);
        (*root)=rbInsert(root, z, k, candy);
        (*last)=z;
        trash=(scanf("%s\n", tmp));
    }
    trash++;
}

void restore(node z){
    while(z){
        z->prossimo=z->next;
        z=z->next;
    }
}
void nuovaPartita(node *root, char *rif, char *tmp, char* sign, int k, elem list, node *last) {
    treeRewind(*last);
    int n, count, trash;
    node z;
    trash=scanf("%s\n", rif);
    if(scanf("%d\n", &n))
        while(n>0 && scanf("%s\n", tmp)!=EOF){
            if(tmp[0]=='+'){
                if(tmp[1]=='i')         inserisciInizio(root, tmp, k, list, true, last);
                else if(tmp[1]=='s')    inOrderPrint(*root);
            }
            else{
                z=treeFind((*root), tmp, k);
                if(z){
                    check(rif, tmp, sign, k); //ottimizzabile
                    list=headInsert(list, z, k, sign);
                    for(int i=0; i<=k; i++){
                        if(i==k){
                            printf("ok\n");
                            listDestroy(list);
                            restore((*last));
                            return;
                        }
                        if(sign[i]!='+') break;
                    }
                    count=filtraParole(tmp, sign, k, last);
                    printf("%s\n%d\n", sign, count);
                    n--;
                }
                else{
                    printf("not_exists\n");
                }
            }
            if(n<=0){
                printf("ko\n");
            }
        }
    trash++;
    listDestroy(list);
    restore((*last));
}

void eradicate(node z){
    if(!z) return;
    eradicate(z->next);
    free(z);
    z=NULL;
}
int main(){
    char *rif, *sign, *buffer;
    int k, trash;
    node tree=NULL, z, last=NULL;
    elem list=NULL;
    trash=scanf("%d\n", &k);

    for(int i=0; i<78; i++){
        occ[i]=0;
    }
    buffer=(char *)malloc(20+k+1+k+1);
    rif = (char *) (buffer+20);
    sign = (char*)(buffer+20+k+1);
    while(scanf("%s\n", buffer)!=EOF){
        if(buffer[0]=='+'){
            if(buffer[1]=='i'){
                inserisciInizio(&tree, buffer, k, list, false, &last);
            }
            else if(buffer[1]=='s'){
                inOrderPrint(tree);
            }
            else if(buffer[1]=='n'){
                nuovaPartita(&tree, rif, buffer, sign, k, list, &last);
            }
        }
        else{
            z=malloc(sizeof(*z)+k+1);
            z->word=(char *)(z+1);
            mystrcpy(z->word, buffer);
            z->next=last;
            z->prossimo=last;
            tree=rbInsert(&tree, z, k, true);
            last=z;
        }
    }
    free(buffer);
    eradicate(last);
    trash++;
    return 0;
}