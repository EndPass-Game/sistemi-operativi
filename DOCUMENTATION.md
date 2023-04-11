# Scelte progettuali

## La struttura del progetto

### Divisione delle cartelle
Raccontiamo del contenuto di ogni cartella:
- `src`: sono presenti i sorgenti dei file con le funzioni da implementare.
- `include`: sono presenti gli headers con all'interno le dichiarazioni delle funzioni da implementare.
- `lib`: sono presenti tutti gli headers delle librerie utilizzate nel progetto, presenti già tra i file iniziali del progetto, le quali sono state modificate durante lo svolgimento del progetto per risolvere un problema di definizione di `NULL`.
- `build`: sarà una cartella creata in automatico in cui saranno presenti i file di output dopo l'operazione dis build.

### Divisione dei file
Il progetto è suddiviso in 3 parti principali all'interno della cartella `src`:
1. Sviluppo delle funzioni relative ai `process control block` implementate nel file `process.c`.
2. Sviluppo delle funzioni di gestione dei `semafori` implementate nel file `semaphore.c`.
3. Sviluppo delle funzioni di gestione del `namespace` implementate nel file `namespace.c`.

Inoltre è presente anche il file `utils.c`, dove definiamo delle funzioni comuni tra tutti i files implementati. 

### Dichiarazione delle variabili globali
Inizialmente avevamo provato a dichiarare le variabili globali negli header. Questa sarebbe stata la scelta più chiara dato che gli headers dovrebbero contenere tutte le interfacce comuni all'eseguibile, e consideravamo le variabili globali all'intero eseguibile.
Però abbiamo in seguito scoperto che questo comportava ad errori di linkaggio, dato che sembrava esserci un allocamento delle variabili globali ogni volta che il file era incluso.
Abbiamo notato che le variabili globali erano utilizzate solo nel singolo modulo, quindi le abbiamo messe nei file `.c`. Nel caso però una variabile globale dovesse risultare necessaria in più file, si può dichiararla nell'header come `extern`.

## Note implementative
In questa sezione parliamo delle assunzioni fatte in alcune parti del progetto.

Tutta la documentazione riguardante le funzioni e le macro è inserita nei file header, mentre quella delle variabili utilizzate nell'implementazione è presente nei file `.c` tramite commenti.

## Assunzioni generali
Abbiamo deciso che a questo livello non è necessario fare checks per verificare che gli input siano diversi da `null`, infatti assumiamo che questi siano sempre validi.

Esempio:

`int emptyChild(pcb_t *p)`, qui noi assumiamo sempre che `p` sia un puntatore valido a un `pcb_t`. Eventuali chiamate errate sono da controllare prima della chiamata di funzione quando utilizzate.

### Process
La lista quando è allocata ha i concatenatori settati a 0.

### Semaphore

Assumiamo che un semaforo, se allocato, ha sempre almeno un processo che aspetta su di esso.

### Namespace
`addnamespace`, qui assumiamo che i figli siano inizializzati correttamente, altrimenti potrebbe succedere una ricorsione infinita.
In questo caso non dovrebbe mai dare errore.

### Utilizzi di `p_list`
Il campo `p_list` è utilizzato come concatenatore in 3 liste principali:
- Lista dei `pcb` liberi.
- Lista dei `pcb` bloccati su semafori.
- Code dei processi, implementati tramite le liste.

È importante assumere che queste liste siano tutti e tre mutualmente esclusive in quanto altrimenti avremmo alcuni conflitti, i quali porterebbero a uno stato invalido.
In particolare potremmo dire che un processo che sia fermo sulla lista dei semafori sia in stato di *`waiting`* mentre, se è nella coda dei processi, si potrebbe dire nello stato di *`running`*, altrimenti se è stato allocato e non è presente nella coda dei processi, è in stato di *`ready`*.


# Phase 2

## PID

Abbiamo deciso di utilizzare i pointers ai `pcb_t` come i PID del programma

## Global process

Se non ho nessun processo che sta runnando allora lo metto a 0