#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "paging_basic.h"

void simulate_basic_paging() {
    int page_size = 0;
    int ram_size = 0;
    int total_frames = 0;
    int free_frames = 0;

    char line[256];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (line[0] == '\0' || line[0] == '#') continue;

        if (strncmp(line, "PAGE_SIZE", 9) == 0) {
            sscanf(line, "PAGE_SIZE %d", &page_size);
            // TODO: Alumno - Configurar el tamaño de página y notificar al sistema
            printf("SYSTEM: Page Size set to %d bytes\n", page_size);
        } else if (strncmp(line, "RAM_SIZE", 8) == 0) {
            sscanf(line, "RAM_SIZE %d", &ram_size);
            // Calcular el total de frames disponibles en base al ram_size y page_size
            total_frames = ram_size / page_size;
            free_frames = total_frames;
            printf("SYSTEM: RAM Size set to %d bytes (%d frames)\n", ram_size, total_frames);
        } else if (strncmp(line, "PROCESS PID", 11) == 0) {
            int pid, size;
            if (sscanf(line, "PROCESS PID %d SIZE %d", &pid, &size) == 2) {
                // Calcular cuántas páginas necesita el proceso (redondeo hacia arriba)
                int pages_needed = (size + page_size - 1) / page_size;

                // Verificar si hay suficientes frames libres para asignar las páginas
                if (pages_needed <= free_frames) {
                    // Actualizar frames libres
                    free_frames -= pages_needed;
                    printf("PROCESS: PID %d (Size: %d) -> %d pages allocated. Frames left: %d\n", 
                           pid, size, pages_needed, free_frames);
                } else {
                    // Notificar falla por falta de memoria
                    printf("PROCESS: PID %d (Size: %d) -> %d pages needed. FAILED (Not enough memory, frames left: %d)\n", 
                           pid, size, pages_needed, free_frames);
                }
            }
        }
    }
}
