#include <stdio.h>
#include "tlb.h"

tlb_entry_t tlb[TLB_SIZE];
int global_timer = 0; // Simulated clock for LRU/FIFO

void init_tlb() {
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].valid = 0;
        tlb[i].page = -1;
        tlb[i].frame = -1;
        tlb[i].last_access = 0;
        tlb[i].insertion_time = 0;
    }
}

int tlb_lookup(int page) {
    global_timer++;
    // Buscar la pagina en el TLB
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].valid && tlb[i].page == page) {
            // actualizar last_access y retornar el frame
            tlb[i].last_access = global_timer;
            printf("TLB HIT: Page %d found in entry %d (Frame %d)\n", page, i, tlb[i].frame);
            return tlb[i].frame;
        }
    }
    // MISS: no se encuentra
    printf("TLB MISS: Page %d not in TLB\n", page);
    return -1;
}

void tlb_insert(int page, int frame) {
    global_timer++;
    
    // Buscar una entrada vacia
    for (int i = 0; i < TLB_SIZE; i++) {
        if (!tlb[i].valid) {
            // insertar en entrada vacia
            tlb[i].page = page;
            tlb[i].frame = frame;
            tlb[i].valid = 1;
            tlb[i].last_access = global_timer;
            tlb[i].insertion_time = global_timer;
            printf("TLB INSERT: Page %d -> Frame %d (Entry %d)\n", page, frame, i);
            return;
        }
    }
    
    // TLB lleno: usar LRU con FIFO como tie-breaker
    int lru_index = 0;
    int min_last_access = tlb[0].last_access;
    int min_insertion_time = tlb[0].insertion_time;
    
    for (int i = 1; i < TLB_SIZE; i++) {
        if (tlb[i].last_access < min_last_access) {
            lru_index = i;
            min_last_access = tlb[i].last_access;
            min_insertion_time = tlb[i].insertion_time;
        } else if (tlb[i].last_access == min_last_access && tlb[i].insertion_time < min_insertion_time) {
            // Tie-breaker: FIFO
            lru_index = i;
            min_insertion_time = tlb[i].insertion_time;
        }
    }
    
    // Desalojar la entrada LRU
    printf("TLB EVICT: Entry %d (Page %d) discarded for Page %d\n", lru_index, tlb[lru_index].page, page);
    
    // Insertar la nueva entrada
    tlb[lru_index].page = page;
    tlb[lru_index].frame = frame;
    tlb[lru_index].valid = 1;
    tlb[lru_index].last_access = global_timer;
    tlb[lru_index].insertion_time = global_timer;
    printf("TLB INSERT: Page %d -> Frame %d (Entry %d)\n", page, frame, lru_index);
}
