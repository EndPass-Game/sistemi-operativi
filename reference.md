# Rereference doc

In questo documento saranno presenti informazioni utili allo sviluppo del team.

## Exception and status

Molte cose sono eseguibili solo se il processore parte in modalità kernel, altrimenti fa raise di eccezzioni come BP
che stoppano l'esecuzione (esempio HALT, LDIT e molti altri) bloccano l'esecuzione se la cpu non è in kernel mode.
Per vedere se è in kernel mode pops pg9, c'è la struttura del registro di controllo (attenzione che in umps è little endian
quindi è inverso).