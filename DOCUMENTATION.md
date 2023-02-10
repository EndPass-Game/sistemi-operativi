# Scelte progettuali

## La struttura del progetto

### Divisione delle cartelle
Raccontiamo del contenuto di ogni cartella:
- `src`: sono presenti i sorgenti dei file con le funzioni da implementare.
- `include`: sono presenti gli headers con all'interno le dichiarazioni delle funzioni da implementare.
- `lib`: sono presenti tutti gli headers delle librerie utilizzate nel progetto, presenti già tra i file iniziali del progetto, le quali sono state modificate durante lo svolgimento del progetto per risolvere un problema di definizione di `NULL`.
- `build`: sarà una cartella creata in automatico in cui saranno presenti i file di output dopo l'operazione dis build.

### Divisione dei file
Il progetto è suddiviso in 3 parti principali: lo sviluppo delle funzioni relative ai `PCB` implementate nel file `process.c`, lo sviluppo delle funzioni di gestione dei semafori implementate nel file `semaphore.c` e lo sviluppo delle funzioni di gestione del `namespace` implementate nel file `namespace.c`.

Inoltre nella cartella `src` abbiamo anche il file `utils.c`, dove definiamo delle funzioni comuni tra tutti i files implementati. 


### Dichiarazione delle variabili globali
Inizialmente avevamo provato a dichiarare le variabili globali negli header. Questa sarebbe stata la scelta più chiara dato che gli headers dovrebbero contenere tutte le interfacce comuni all'eseguibile, e consideravamo le variabili globali globali all'intero eseguibile.
Però abbiamo in seguito scoperto che questo comportava ad errori di linkaggio, dato che sembrava che provava ad allocare le variabili globali ogni volta che il file era incluso. 
Abbiamo notato che le variabili globali erano utilizzate solo nel singolo modulo, quindi le abbiamo messe nei file `.c`. Nel caso però una variabile globale dovesse risultare necessaria in più file, si può dichiararla nell'header come `extern`.


## Note implementative
In questa sezione parliamo delle assunzioni fatte in alcune parti del progetto.

Tutta la documentazione riguardante le funzioni e le macro è inserita nei file header, mentre quella delle variabili utilizzate nell'implementazione è nei file `.c`.

### Assunzioni generali
Abbiamo deciso che a questo livello non è necessario fare checks
per verificare che gli input siano diversi da `null`, infatti assumiamo che siano sempre validi.

Esempio:

`int emptyChild(pcb_t *p)`, noi assumiamo sempre che `p` sia un puntatore valido a un `pcb_t`. Eventuali chiamate errate sono da 

### Process
la lista quando è allocata ha i concatenatori messi a 0.

### Semaphore

assumiamo che un semaforo se allocato ha sempre almeno un processo che aspetta su di essa.

### Namespace
`addnamespace` assumiamo che i figli siano inizializzati bene, altrimenti potrebbe succedere che andiamo in ricorsione infinita.
Quindi non dovrebbe mai fare errore.

### Utilizzi di p_list
il campo p_list è utilizzato come concatenatore in 3 liste principali:
- lista dei pcb liberi
- lista dei pcb bloccati su semafori
- code dei processi, implementati tramite le liste.

È importante assumere che queste liste siano tutti e tre mutualmente esclusive, in quanto altrimenti avremmo alcuni conflitti, che porterebbero a uno stato invalido.
In particolare potremmo dire che un processo che sia fermo sulla lista dei semafori, sia in stato di *waiting*, mentre se è nella coda dei processi si potrebbe dire nello stato di *running*, mentre se è allocato e non è nella coda dei processi è in stato di *ready*.