
abbiamo deciso che a questo livello non è necessario fare i checks
per verificare che gli input sono diversi da nulli, assumiamo che siano 
sempre validi

esempio:

int emptyChild(pcb_t *p), noi assumiamo sempre che `p` sia un valido
puntatore a un pcb_t.


## process
la lista quando è allocata ha i concatenatori messi a 0.