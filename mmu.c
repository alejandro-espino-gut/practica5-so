#include <stdio.h>
#include "mmu.h"
#include "tlb.h"

#define PAGE_TABLE_SIZE 256

int page_table[PAGE_TABLE_SIZE];
int use_tlb = 0; // Flag to enable TLB behavior

void init_mmu() {
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        page_table[i] = -1;
    }
    init_tlb();
}

void enable_tlb() {
    use_tlb = 1;
}

void set_page_table_entry(int page, int frame) {
    if (page >= 0 && page < PAGE_TABLE_SIZE) {
        // TODO: Alumno - Mapear la pagina al frame en la tabla de paginas
        page_table[page] = frame;
        printf("MMU: Page %d mapped to Frame %d\n", page, frame);
    } else {
        fprintf(stderr, "Error: Page index out of bounds\n");
    }
}

int mmu_translate(int virtual_addr, int page_size) {
    if (page_size <= 0) return -1;

    // calcular el numero de pagina y el offset
    int page = virtual_addr / page_size;
    int offset = virtual_addr % page_size;
    int frame = -1;

    // si use_tlb es 1, buscar primero en el TLB
    if (use_tlb) {
        frame = tlb_lookup(page);
    }

    if (frame == -1) {
        // si no esta en TLB, buscar en la tabla de paginas
        if (page >= 0 && page < PAGE_TABLE_SIZE) {
            frame = page_table[page];
            // si se encuentra, y use_tlb es 1, insertarlo en el TLB
            if (frame != -1 && use_tlb) {
                tlb_insert(page, frame);
            }
        }
    }

    if (frame != -1) {
        // calcular la direccion fisica
        int physical_addr = frame * page_size + offset;
        printf("TRANSLATE: Virtual 0x%04X -> Physical 0x%04X (Frame %d)\n", 
               virtual_addr, physical_addr, frame);
        return physical_addr;
    } else {
        printf("TRANSLATE ERROR: Page %d is not mapped\n", page);
        return -1;
    }
}
