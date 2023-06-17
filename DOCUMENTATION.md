# Phase 1

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

## Introduzione generale

La fase due introduce le astrazioni come scheduler, syscalls, gestione di eccezioni.

L'organizzazione proposta è divisa in 5 files presenti sotto la directory 
- `syscall.c`, in cui sono implementate tutte le syscall e il syscall handler
- `scheduler.c`, in cui è presente lo scheduler
- `nucleus.c`, in cui sono gestite le operazioni di inizializzazione.
- `exceptions.c`, in cui sono gestiti i timer e le funzioni che demultiplexano le eccezioni.
- `devices.c`, in cui sono gestiti interrupt e risoluzione di device con i propri semafori.

Nei file headers sono presenti documentazione per tutte le funzioni non locali.

## Processi

### PID

Abbiamo deciso di utilizzare i pointers ai `pcb_t` come i PID del programma.

### Global process

Se non ho nessun processo che sta runnando allora lo metto a NULL, altrimenti punta al processo corrente che sta eseguendo.

## Sincronizzazione Devices

Viene utilizzata la struttura `sysiostate` dichiarata in `pandos_types.h`

Ogni device contiene un proprio semaforo di sincronizzazione inizializzato a 0, un semaforo di mutex inizializzato a 1 e un pointer al pcb correntemente bloccato su IO (se sono bloccato su mutex non sono considerato bloccato in IO), quindi esiste al massimo un singolo processo in IO. Se non c'è nessun processo io IO questo pcb è vuoto.

Nel caso in cui si faccia un'operazione di input output su un device già impegnato, questo processo sarà bloccato su un semaforo di mutex per il device.

**INVARIANTE**: Quando un processo viene bloccato durante una operazione di input/output sui semafori di mutex oppure di sync, avrà come stato lo stato di BIOS_DATA_PAGE in modo che quando riprenderà l'esecuzione sarà già fuori dalla syscall. Si utilizza un sistema di passing the baton per riattivare un possibile secondo processo bloccato sulla mutex del device. 
*ATTENZIONE*: Non bisogna terminare un processo bloccato su un mutex esterno perché il kernel non ha (attualmente, v2) funzionalità per controllare questo blocco.
Ad esempio nel p8 se provo a printare in un figlio, ma questo viene eliminato, mantenendo comunque la mutex del print, questo sicuramente darà un PANIC perché un processo è stato ucciso mentre printava.

Quando viene issued una syscall di DOIO, questo viene sempre fermato nel semaforo di sincronizzazione, sarà riattivato solo quando ci sarà l'interrupt del device. Allora in questo momento sarà rimesso sulla coda di ready.
Per esperienza questo avviene molto presto, solitamente non appena finisce la LDST dello scheduler oppure non appena setto la mask prima di chiamare wait, ma per altri devices può essere diverso.

### Sync Semaphore

Talvolta può succedere che il semaforo sync chiamato `sem_sync` del device e sia stato settato a -1.
Questo avviene solo ed esclusivamente quandso un processo che ha richiesto una I/O è stato terminato prima che l'I/O venga finita. Allora quando arriva l'interrupt io devo solamente fare l'acknowledgement, non devo eseguire le procedure di ritorno come endIO o salvare il registro di fine.

Viene scelto -1 come valore perché in questo modo quando avrò la passeren nel device interrupt, avverrà che sarà rimesso a 0, quindi 
tornerà ad essere una specie di mutex con il solo scopo di sincronizzare l'inizio e la fine delle operazioni sul device, quando inizializzati.

### Mutex Semaphore

Questo semaforo arbitra l'accesso al device, in modo che un singolo processo alla volta possa scrivere il suo comando nel registro del device. Quando il processo che ha la mutex finisce, riattiverà i processi bloccati su questo semaforo con un pattern "pass the baton".

## Altre note generali
### Timer
`
Abbiamo deciso di far pagare il tempo delle syscalls ai processi, mentre gli interrupt non sono pagati, dato che è tempo utilizzato per tutti i processi.
Anche durante lo scheduler, mettiamo uno `STCK` per non fare contare il tempo dello scheduler o funzioni precedenti, in modo che quando il processo è caricato è come se cominciasse a contare da 0.

### GetChildByNamespace

Nel conteggio dei figli di un namespace vengono contati tutti i figli nell'intero albero dei processi sottostanti al processo corrente.

### Commento sul p5

Può apparire strano che in `p5mm` appena chiamiamo `p5b` in user mode abbiamo un errore `ADEL`, che ci riporta su `p5gen` dato che è gestito come generalexcept. Questo è sensato perché la stack di p5b ha un indirizzo minore di 0x8000.0000, leggiamo da umps3 pg13 che tali accessi generano un eccezione adel.