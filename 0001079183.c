/*  Baiocchi   Lucio   0001079183   gruppo A   lucio.baiocchi@studio.unibo.it*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
/*
 * La struttura arco contiene o blocchi che compongono il grafo,
 * val è un valore che identifica ogni nodo, 
 * wet indica se il nodo è bagnato o meno (0 -> asciutto, 1 -> bagnato)
 * I vari puntatori nord, sud, est, ovest servono per collegare il nodo all'interno della mappa
 * color viene utilizzato per capire se il nodo è già stato visitato
 * dist_o contiene la distanza di ogni nodo dall'origine.
 * dist_f contiene la distanza di ogni nodo dalla fine.
 * c_wet contiene il conteggio dei noti nbagnati precedenti.
 * prev viene utilizzato per collegare tutti i nodi del percorso migliore
 */

typedef struct Edge
{
    int val;
    int wet;
    struct Edge *sud;
    struct Edge *nord;
    struct Edge *est;
    struct Edge *ovest;
    int color;          /* 0 = bianco  1 = grigio  2 = nero */
    int dist_o;
    int dist_f;
    int c_wet;
    struct Edge *prev;
} Edge;

/*
 * la struttura lista è collegata tramite il puntatore next, e permette di contenere i blocchi del
 * grafo durante l'esecuzione del bfs.
*/
typedef struct Lista   /*elemento della coda*/
{
    struct Lista *next;
    struct Edge *elemento;
}Lista;

/*crea un nuovo nodo del grafo, con tutti i puntatori a NULL, e i valori inizializzati a zero,
 * escluso val, dry e c_wet che viene impostato a -1 per indicare infinito*/
Edge *new_edge(int val, int dry);

/*calcola la distanza di ogni nodo dall'origine e la distanza dalla destinazione,
* elimina tutti i nodi che hanno hanno dist_o + dist_f > cammino minimo, perchè sono
* inutili. */
Edge **bfs(Edge **lista_nodi, int len_lista);

/*push di un blocco del grafo dentro la lista, viene usato nel bfs*/
int push(Lista **testa_lista, Edge *elemento);

/*pop di un blocco del grafo, viene usato nel bfs*/
Edge *pop(Lista **testa_lista);

/*crea un nuovo elemento della lista*/
Lista *new_elem(Edge *elem);

/*elimina tutti i nodi dalla lista di nodi*/
void list_destroy(Edge **testa_mappa);

/*parte dall'ultimo elemento e trova il percorso migliore*/
char *find_best_path(Edge *destinazione,char *percorso);

void rimuovi_nodo(Edge *nodo);

/*passato l'array contenente la mappa letta da file crea la mappa con ogni nodo 
* di tipologia Edge*/
Edge **creo_mappa(char mappa[], int n, int m);

/*scrive il percorso partendo dalla destinazione passando per i puntatori prev*/
char *find_path(Edge *destinazione, char *percorso);
/*aggiorna c_wet partendo dal primo nodo. Dopo che bfs ha eliminato tutti i nodi che 
* non stanno sul percorso minimo*/
Edge *conta_bagnate(Edge *testa);

/*
 * legge il numero di colonne e righe dal file di input e alloca un array della dimensione degli elementi
 * poi inserisce i valori uno a uno, rimovendo il carattere di ritorno a capo, l'ultimo elemento è sostituito con #
 * per capire che è la destinazione. Crea il grafo con crea_percorso, ed elimina l'array mappa dalla memoria.
 * chiama il bfs, che restituisce NULL se non esite il percorso, mentre restituisce il puntatore alla destinazione
 * se esiste, durante l'esecuzione del bfs, il valore dist_o e dist_f dei nodi viene aggiornato, con dist+1 del nodo padre.
 * se destinazione = NULL restituisce -1 -1 e termina il programma. Successivamente con conta_bagnate aggiorna c_wet e prev 
 * di ogni cella e trova il percorso con find_path e lo stampa a video. 
 * L'ultima operazione è quella di deallocare la memoria riservata al grafo con list_destroy.
 */

int main( int argc, char *argv[] ) {
    Edge **lista_nodi, *dest;
    int n, m, k, len;
    char t, *mappa, *percorso;
    FILE *filein = stdin;

    if (argc != 2) {
        fprintf(stderr, "Invocare il programma con: %s 0001079183\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "-") != 0) {
        filein = fopen(argv[1], "r");
        if (filein == NULL) {
            fprintf(stderr, "Can not open %s\n", argv[1]);
            return EXIT_FAILURE;
        }
    }

    fscanf(filein,"%d %d", &n, &m);

    t = 0;
    k = 0;
    len = n*m;

    mappa = (char*)malloc(sizeof (char) * len + 1);
    assert(mappa != NULL);

    while (k < (n*m))
    {
        fscanf(filein, "%c", &t);
        if (t != 10) 
        {       /* diverso dall'andata a capo*/
            mappa[k] = t;
            k++;
        }
    }
    if (mappa[k - 1] == '0')
    {
        mappa[k - 1] = '#';
        mappa[k] = '\0';
        
    }
    fclose(filein);

    lista_nodi = creo_mappa(mappa, n, m);
    free(mappa);
    lista_nodi = bfs(lista_nodi, len);
    
    if (lista_nodi == NULL)
    {
        printf("-1 -1");

        return EXIT_SUCCESS;
    }

    dest = conta_bagnate(lista_nodi[0]);
    percorso = (char*)calloc(lista_nodi[0]->dist_f, sizeof (char));
    percorso = find_path(dest, percorso);
    printf("%s\n", percorso);

    list_destroy(lista_nodi);
    free(lista_nodi);
    free(percorso);
    return EXIT_SUCCESS;
}


Edge *new_edge(int val, int wet)
{
    Edge *edge;
    edge = (Edge*)malloc(sizeof(Edge));
    assert(edge != NULL);

    edge->val = val;
    edge->wet = wet;

    edge->sud = NULL;
    edge->nord = NULL;
    edge->est = NULL;
    edge->ovest = NULL;
    edge->prev = NULL;
    edge->color = 0;
    edge->dist_o = 0;
    edge->dist_f = 0;
    edge->c_wet = -1;
    return edge;
}

int push(Lista **testa_lista, Edge *elemento)
{
    Lista *tmp, *a;
    
    if (*testa_lista == NULL)
    {
        *testa_lista = new_elem(elemento);
        return 0;
    }

    tmp = *testa_lista;
    while (tmp->next != NULL)   /* scorro fino a che arrivo all'ultimo elemento*/
    {
        tmp = tmp->next;
    }
    a = new_elem(elemento);
    tmp->next = a;
    return 0;
}

Edge *pop(Lista **testa_lista)
{
    Edge *a;
    Lista *tmp;

    if (testa_lista == NULL)
    {
        return NULL;
    }

    a = (*testa_lista)->elemento;
    tmp = *testa_lista;
    tmp = tmp->next;
    free(*testa_lista);
    *testa_lista = tmp;
    return a;
}

Lista *new_elem(Edge *val)
{
    Lista *elem;
    elem = (Lista*)malloc(sizeof (Lista));
    assert(elem != NULL);

    elem->elemento = val;
    elem->next = NULL;
    return elem;
}

Edge **bfs(Edge **lista_nodi, int len_lista)
{
    int stop, trovato, k, l, c_attraversati, distanza;
    Edge *tmp, **lista_tmp;
    Lista *testa_lista;

    trovato = 0;
    stop = 0;
    testa_lista= new_elem(lista_nodi[0]);  
    c_attraversati = 0;

    /*inserisco il primo elemento dentro la coda 
    * dopo averlo inserito nella coda diventa grigio*/
    
    testa_lista->elemento->color = 1; 
    testa_lista->elemento->dist_o = 1;
    
    /*primo bfs, dove vengono aggiornati i valori di dist_o*/
    while (stop != 1)
    {
        tmp = pop(&testa_lista);
        tmp->color = 2;
        c_attraversati++;

        if (tmp->val == '#')
        {
            trovato = 1;
        }

        if (tmp->sud != NULL && tmp->sud->color == 0 && (tmp->sud->val == '0' || tmp->sud->val == '#'))
        {
            push(&testa_lista, tmp->sud);
            tmp->sud->color = 1;
            tmp->sud->dist_o = tmp->dist_o +1;
        }
        if (tmp->est != NULL && tmp->est->color == 0 && (tmp->est->val == '0' || tmp->est->val == '#'))
        {
            push(&testa_lista, tmp->est);
            tmp->est->color = 1;
            tmp->est->dist_o = tmp->dist_o +1;  
        }
        if (tmp->ovest != NULL && tmp->ovest->color == 0 && tmp->ovest->val == '0')
        {
            push(&testa_lista, tmp->ovest);
            tmp->ovest->color = 1;
            tmp->ovest->dist_o = tmp->dist_o +1;
        }
        if (tmp->nord != NULL && tmp->nord->color == 0 && tmp->nord->val == '0')
        {
            push(&testa_lista, tmp->nord);
            tmp->nord->color = 1;
            tmp->nord->dist_o = tmp->dist_o +1;
        }     
        if (testa_lista == NULL)
        {
            stop = 1;
        }
    }

    /*se non è stato trovato l'ultimo nodo, elimina la lista_nodi perchè non esiste 
    * un percorso e ritorna NULL*/
    if (trovato == 0)
    {
        list_destroy (lista_nodi);
        free(lista_nodi);
        return NULL;
    }

    /*se esiste un percorso che porta alla destinazione, 
    * viene creata una nuova lista elementi, contenente solo 
    * i nodi attraversati dal bfs ( con color = 2), gli altri vengono eliminati
    */

    lista_tmp = (Edge**) malloc(sizeof(Edge*)*(c_attraversati));
    assert(lista_tmp != NULL);
    l = 0;
    k = 0;

    while (l < len_lista)
    {   
        /*se il nodo dentro lista_nodi è stato attraversato, lo aggiungo 
        * a lista_tmp, altrimenti lo elimino e imposto tutti i 
        * nodi in entrata al nodo a NULL*/
        if (lista_nodi[l]->color == 2)
        {
            lista_tmp[k] = lista_nodi[l];
            k++;
        }
        else 
        {
            rimuovi_nodo(lista_nodi[l]);
        }
        l++;
    }
    
    /*elimina il puntatore all'attuale lista, che viene 
    * sostituito con la lista nuova*/
    
    free(lista_nodi);
    lista_nodi = lista_tmp;
    
    /*parte un ulteriore bfs dalla destinazione, e salva le distanze di ogni nodo 
    * dalla destinazione sul contatore dist_f*/
    
    stop = 0;
    testa_lista= new_elem(lista_nodi[c_attraversati - 1]);  

    /*inserisco il primo elemento dentro la coda 
    * dopo averlo inserito nella coda diventa grigio, dato che è appena stato fatto un 
    * bfs, la flag color è impostata a 2, utilizzo questo secondo bfs per resettare anche tutte le flag:
    * 2 = non visitato
    * 1 = inserito in lista
    * 0 = visitato*/
    
    testa_lista->elemento->color = 1; 
    testa_lista->elemento->dist_f = 1;

    while (stop != 1)
    {
        tmp = pop(&testa_lista);
        tmp->color = 0;

        if (tmp->sud != NULL && tmp->sud->color == 2)
        {
            push(&testa_lista, tmp->sud);
            tmp->sud->color = 1;
            tmp->sud->dist_f = tmp->dist_f +1;
        }
        if (tmp->est != NULL && tmp->est->color == 2)
        {
            push(&testa_lista, tmp->est);
            tmp->est->color = 1;
            tmp->est->dist_f = tmp->dist_f +1;  
        }
        if (tmp->ovest != NULL && tmp->ovest->color == 2)
        {
            push(&testa_lista, tmp->ovest);
            tmp->ovest->color = 1;
            tmp->ovest->dist_f = tmp->dist_f +1;         
        }
        if (tmp->nord != NULL && tmp->nord->color == 2)
        {
            push(&testa_lista, tmp->nord);
            tmp->nord->color = 1;
            tmp->nord->dist_f = tmp->dist_f +1;           
        }
        if (testa_lista == NULL)
        {
            stop = 1;
        }
    }

    /*se la somma delle due distanze > della distanza del percorso minimo, 
    * significa che il nodo è superfluo e può essere rimosso, liberando celle
    * superflue*/

    k = 0;
    distanza = lista_nodi[0]->dist_f;
    l = 0;
    while (k < c_attraversati)
    {
        if ((lista_nodi[k]->dist_f + lista_nodi[k]->dist_o) > distanza +1)
        {
            rimuovi_nodo(lista_nodi[k]);
            lista_nodi[k] = NULL;
            l++;
        }
        k++;
    }

    /*l tiene il conto degli elementi eliminati, in questo modo riesco a riallocare 
    * una lista con la dimensione degli elementi rimasti*/
    
    len_lista = c_attraversati - l;

    lista_tmp = (Edge**) malloc(sizeof(Edge*)*(len_lista));
    assert(lista_tmp != NULL);
    l = 0;
    k = 0;

    while (l < c_attraversati)
    {   
        /*se il nodo dentro lista_nodi è stato attraversato, lo aggiungo 
        * a lista_attraversati, altrimenti lo elimino e imposto tutti i 
        * nodi in entrata al nodo a NULL*/

        if (lista_nodi[l] != NULL)
        {
            lista_tmp[k] = lista_nodi[l];
            k++;
        }
        l++;
    }
    free(lista_nodi);
    lista_nodi = lista_tmp;
    return lista_nodi;
}

void list_destroy(Edge **lista_elem)
{   
    /*elimina tutti gli elementi della lista fino a che non trova
    * l'ultimo */
    int k, stop;
    k = 0;
    stop = 0;
    while (stop != 1)
    {   
        if (lista_elem[k] != NULL)
        {
            if (lista_elem[k]->val == '#')
            {
                stop = 1;
            }
            free(lista_elem[k]);            
        }   
    k++;    
    }
}

char *find_path(Edge *destinazione, char *percorso)
{
    int distanza, conto;
    Edge *src;
    char direzione;
    distanza = destinazione->dist_o;
    
    conto = 0;
    src = destinazione;

    printf("%d ", destinazione->dist_o);
    while (src != NULL)
    {
        if (src->wet == 1)
        {
            conto++;
        }
        if (src->prev == src->nord)
        {
            direzione = 'S';
        }
        if (src->prev == src->ovest)
        {
            direzione = 'E';
        }
        if (src->prev == src->est)
        {
            direzione = 'O';
        }
        if (src->prev == src->sud)
        {
            direzione = 'N';
        }
        
        if (distanza != 1)
        {
            percorso[distanza -2] = direzione;
        }
        src = src->prev;
        distanza--;
    }
    printf ("%d\n", conto);
    return percorso;
}

void rimuovi_nodo(Edge *nodo)
{
    if (nodo->nord != NULL)
    {
        nodo->nord->sud = NULL; 
    }
    
    if (nodo->sud != NULL)
    {
        nodo->sud->nord = NULL; 
    }

    if (nodo->est != NULL)
    {
        nodo->est->ovest = NULL; 
    }

    if (nodo->ovest != NULL)
    {
        nodo->ovest->est = NULL; 
    }

    free(nodo);
}

Edge **creo_mappa(char mappa[],int n, int m)
{   
    Edge **lista_nodi;
    int k, r, c_dry;
    k = 0;

    lista_nodi = (Edge**) malloc(sizeof(Edge*)*(n*m));

    while (mappa[k] != '\0')
    {
        lista_nodi[k] = new_edge (mappa[k], 1);
        k++;
    }
    
    k = 0;
    r = 0;
    c_dry = 0;

    while (k < ((n*m) - 1))
    {   

        if (c_dry > 0)
        {
            c_dry--;
        }

        /* se corrente == 0 e conto asciutti > 0 imposto la cella come asciutta*/
        if (lista_nodi[k]->val == '0' || lista_nodi[k]->val =='#')  
        {
            if (c_dry > 0)
            {
                lista_nodi[k]->wet = 0;
            }
        }

        /* se corrente != 0 e maggiore del conto asciutte, aggiorno il conteggio*/
        if ((lista_nodi[k]-> val - 48) >= c_dry  && lista_nodi[k]->val != '0')  
        {
            c_dry = (lista_nodi[k]-> val) - 47;
        }

        /* se è prima riga i puntatori a nord sono NULL, 
        * collego i restanti con gli altri nodi*/
        if (r == 0)
        {
            lista_nodi[k]->sud = lista_nodi[k + m];
        }
        
        /* se siamo nelle righe in mezzo, nord e sud sono
        diversi da null*/
        else if(r != (n - 1))
        {
            lista_nodi[k]->nord = lista_nodi[k - m];
            lista_nodi[k]->sud = lista_nodi[k + m];
        }
        
        /* quando arriva all'ultima riga, i puntatori sud 
        *  sono indirizzati a NULL*/
        if (r == (n - 1))
        {
            lista_nodi[k]->nord = lista_nodi[k - m];
        }

        /* se non è il primo elemento della riga ovest = prec*/
        if (((k +1) % m) != 1)
        {
            lista_nodi[k]->ovest = lista_nodi[k - 1];
        }
 
        /* se non è l'ultimo elemento della riga est = succ*/
        if (((k + 1) % m) != 0)
        {
            lista_nodi[k]->est = lista_nodi[k + 1];
        }

        /*
        fine riga, incremento contatore righe, 
        azzero conto dry
        */
        if (((k + 1) % m) == 0) 
        {   
            c_dry = 0;
            r++;
        }
        k++;
    }
    
    /*collega e aggiorna l'ultimo nodo*/

    lista_nodi[k]->nord = lista_nodi[k - m];
    lista_nodi[k]->ovest = lista_nodi[k - 1];
    if (c_dry > 0)
    {
        lista_nodi[k]->wet = 0;
    }

    k = 0;
    return lista_nodi;
}

Edge *conta_bagnate(Edge *testa)
    {
    int stop;
    Edge *tmp, *dest;
    Lista *testa_lista;
    stop = 0;
    testa_lista= new_elem(testa);
    if (testa_lista->elemento->wet == 0)
    {
        testa_lista->elemento->c_wet = 0;
    }
    else
    {
        testa_lista->elemento->c_wet = 1;
    }
    /*inserisco il primo elemento dentro la coda
    * dopo averlo inserito nella coda diventa grigio*/
    testa_lista->elemento->color = 1;
    while (stop != 1)
    {
        tmp = pop(&testa_lista);
        tmp->color = 2;
        if (tmp->val == '#')
        {
            dest = tmp;
        }
        /*per ogni nodo controlla se i suoi puntatori non sonon NULL,
        * e quindi aggiorna i valori di c_wet del nodo puntato se non
        * è mai stato raggiunto (quindi se è -1) o se c_wet è maggiore.*/
        if (tmp->sud != NULL )
        {
            if (tmp->sud->color == 0)
            {
                push(&testa_lista, tmp->sud);
                tmp->sud->color = 1;
            }
            if (tmp->sud->c_wet == -1 || tmp->sud->c_wet > tmp->c_wet)
            {
                tmp->sud->c_wet = tmp->c_wet + tmp->sud->wet;
                
                if (tmp->dist_o == tmp->sud->dist_o -1)
                {
                    tmp->sud->prev = tmp;
                }
            }
        }
        if (tmp->est != NULL )
        {
            if (tmp->est->color == 0)
            {
                push(&testa_lista, tmp->est);
                tmp->est->color = 1;
            }
            if (tmp->est->c_wet == -1 || tmp->est->c_wet > tmp->c_wet)
            {
                tmp->est->c_wet = tmp->c_wet + tmp->est->wet;
                
                if (tmp->dist_o == tmp->est->dist_o -1)
                {
                    tmp->est->prev = tmp;
                }
            }
        }
        if (tmp->ovest != NULL)
        {
            if (tmp->ovest->color == 0)
            {
                push(&testa_lista, tmp->ovest);
                tmp->ovest->color = 1;
            }
            if (tmp->ovest->c_wet == -1 || tmp->ovest->c_wet > tmp->c_wet)
            {
                tmp->ovest->c_wet = tmp->c_wet + tmp->ovest->wet;
               
                if (tmp->dist_o == tmp->ovest->dist_o -1)
                {
                    tmp->ovest->prev = tmp;
                }
            }
        }
        if (tmp->nord != NULL)
        {
            if (tmp->nord->color == 0)
            {
                push(&testa_lista, tmp->nord);
                tmp->nord->color = 1;
            }
            if (tmp->nord->c_wet == -1 || tmp->nord->c_wet > tmp->c_wet)
            {
                tmp->nord->c_wet = tmp->c_wet + tmp->nord->wet;
                
                if (tmp->dist_o == tmp->nord->dist_o -1)
                {
                    tmp->nord->prev = tmp;
                }
            }
        }
        /* se gli elementi della coda sono finiti, esce dal ciclo*/
        if (testa_lista == NULL)
        {
        stop = 1;
        }
    }
    return dest;
}
