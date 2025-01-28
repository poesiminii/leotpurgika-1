#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#define MEMORY_SIZE 512  // Total memory
#define TIME_QUANTUM 3   // Time quantum

void initialize_memory();
bool allocate_memory(int pid, int memory_needed);
void deallocate_memory(int pid);
void simulate();


typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int memory_needed;
    bool in_memory;
} Process;

typedef struct {
    int start;
    int size;
    bool free;
    int pid;
} MemoryBlock;

MemoryBlock *memory;
Process *processes;
int process_count = 0;
int current_time = 0;

int main() {
    initialize_memory();

   process_count = 5;

    processes = (Process *)calloc(process_count, sizeof(Process));

    for (int i = 0; i < process_count; i++) {
        processes[i].pid = i + 1;
        printf("\nEnter details for Process %d:\n", processes[i].pid);
        printf("Arrival Time: ");
        scanf("%d", &processes[i].arrival_time);
        printf("Burst Time: ");
        scanf("%d", &processes[i].burst_time);
        processes[i].remaining_time = processes[i].burst_time;
        printf("Memory Needed (KB): ");
        scanf("%d", &processes[i].memory_needed);
        processes[i].in_memory = false;
    }


    simulate();

    free(memory);
    free(processes);
    return 0;
}

void initialize_memory() {
    memory = (MemoryBlock *)calloc(MEMORY_SIZE, sizeof(MemoryBlock));
    memory[0].start = 0;
    memory[0].size = MEMORY_SIZE;
    memory[0].free = true;
    memory[0].pid = -1;
}

bool allocate_memory(int pid, int memory_needed) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i].free && memory[i].size >= memory_needed) {
            if (memory[i].size > memory_needed) {
                // Split the block
                for (int j = MEMORY_SIZE - 1; j > i; j--) {
                    memory[j] = memory[j - 1];
                }
                memory[i + 1].start = memory[i].start + memory_needed;
                memory[i + 1].size = memory[i].size - memory_needed;
                memory[i + 1].free = true;
                memory[i + 1].pid = -1;
                memory[i].size = memory_needed;
            }
            memory[i].free = false;
            memory[i].pid = pid;
            return true;
        }
    }
    return false;
}

void deallocate_memory(int pid) {
    for (int i = 0; i < MEMORY_SIZE; i++) {
        if (memory[i].pid == pid) {
            memory[i].free = true;
            memory[i].pid = -1;
            // Merge free blocks
            if (i > 0 && memory[i - 1].free) {
                memory[i - 1].size += memory[i].size;
                for (int j = i; j < MEMORY_SIZE - 1; j++) {
                    memory[j] = memory[j + 1];
                }
                i--;
            }
            if (i < MEMORY_SIZE - 1 && memory[i + 1].free) {
                memory[i].size += memory[i + 1].size;
                for (int j = i + 1; j < MEMORY_SIZE - 1; j++) {
                    memory[j] = memory[j + 1];
                }
            }
        }
    }
}



void simulate() {
    int time_slice = 0;
    int running_process = -1;

    while (1) {
        bool all_done = true;

        for (int i = 0; i < process_count; i++) {
            if (processes[i].remaining_time > 0) {
                all_done = false;

                // Load process in memory
                if (processes[i].arrival_time <= current_time && !processes[i].in_memory) {
                    if (allocate_memory(processes[i].pid, processes[i].memory_needed)) {
                        processes[i].in_memory = true;
                        printf("Time %d: Process %d loaded into memory.\n", current_time, processes[i].pid);
                    }
                }
            }
        }

        if (all_done) break;

        // Find the next process to run
        if (running_process == -1 || time_slice == 0) {
            int start = (running_process + 1) % process_count;
            for (int i = 0; i < process_count; i++) {
                int index = (start + i) % process_count;
                if (processes[index].remaining_time > 0 && processes[index].in_memory) {
                    running_process = index;
                    time_slice = TIME_QUANTUM;
                    break;
                }
            }
        }

        if (running_process != -1) {
            printf("Time %d: Process %d is running.\n", current_time, processes[running_process].pid);
            processes[running_process].remaining_time--;
            time_slice--;

            if (processes[running_process].remaining_time == 0) {
                printf("Time %d: Process %d finished execution.\n", current_time, processes[running_process].pid);
                deallocate_memory(processes[running_process].pid);
                processes[running_process].in_memory = false;
                running_process = -1;
                time_slice = 0;
            }
        }

        current_time++;
    }
}
