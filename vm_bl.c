#include <stdio.h>
#include "vm_bl.h"

#define MAX_PROCESSES 10

process_t processes[MAX_PROCESSES];
int process_count = 0;

static int get_indice_proc(int pid) {
    for (int i = 0; i < process_count; i++) {
        if (processes[i].pid == pid) {
            return i;
        }
    }
    return -1;
}

void assign_process(int pid, int base, int limit) {
    int index = get_indice_proc(pid);

    if (index != -1) {
        processes[index].base = base;
        processes[index].limit = limit;
    } else if (process_count < MAX_PROCESSES) {
        processes[process_count].pid = pid;
        processes[process_count].base = base;
        processes[process_count].limit = limit;
        process_count++;
    } else {
        fprintf(stderr, "Error: Maximum number of processes reached\n");
    }
}

int base_limit(int pid, int address) {
    int index = get_indice_proc(pid);
    if (index == -1) {
        return -1;
    }

    long long base = processes[index].base;
    long long limit = processes[index].limit;
    long long physical_address = address;

    if (physical_address >= base && physical_address < base + limit &&
        physical_address >= 0 && physical_address < RAM_SIZE) {
        return 1;
    }

    return 0;
}

void ram_write(int pid, int address, unsigned char value) {
    int result = base_limit(pid, address);
    if (result == 1) {
        ram[address] = value;
        printf("WRITE: Process %d wrote %02X to 0x%03X\n", pid, value, address);
    } else if (result == 0) {
        printf("ERROR: Process %d access violation at 0x%03X (Base-Limit)\n", pid, address);
    } else {
        printf("ERROR: Process %d not found\n", pid);
    }
}

unsigned char ram_read(int pid, int address) {
    int result = base_limit(pid, address);
    if (result == 1) {
        unsigned char value = ram[address];
        printf("READ: Process %d read %02X from 0x%03X\n", pid, value, address);
        return value;
    } else if (result == 0) {
        printf("ERROR: Process %d access violation at 0x%03X (Base-Limit)\n", pid, address);
    } else {
        printf("ERROR: Process %d not found\n", pid);
    }
    return 0;
}
