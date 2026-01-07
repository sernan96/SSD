#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#define SZ 100

typedef enum { IDLE, 
    WRITE, 
    READ, 
    ERASE, 
    ERROR 
}Test;

typedef struct {
    Test ins;
    int add, data;
}Set;

int flag = 1;

Test SetCmd(char* cmd) {
    if (strcmp(cmd, "write") == 0)
        return WRITE;
    else if (strcmp(cmd, "read") == 0)
        return READ;
    else if (strcmp(cmd, "erase") == 0)
        return ERASE;
    else
        return ERROR;
}

int GetCmd(Set* set, char *cmd) {
    char* tok = strtok(cmd, " ");
    if (tok == 0)
        return 0;
    set->ins = SetCmd(tok);

    switch (set->ins) {
    case WRITE: {
        char* a = strtok(NULL, " ");
        char* d = strtok(NULL, " ");
        if (!a || !d)
            return 0;
        set->add = atoi(a);
        set->data = atoi(d);
        return 1;
    }


    case ERASE: case READ: {
        char* a = strtok(NULL, " ");
        set->add = atoi(a);
        if (!a)
            return 0;
        return 1;
    }
    case ERROR:
        return 0;
    default:
        return 0;
    }
}


void Command(char* cmd) {
    Set set;
    if (!GetCmd(&set, cmd)) {
        printf("error");
        return;
    }

    switch (set.ins) {
    case WRITE:
        printf("write\n");
        break;
    case READ:
        printf("read\n");
        break;
    case ERASE:
        printf("erase\n");
        break;
    default:
        printf("error");
        break;
    }
}

void Runner(void) {
    FILE* input = fopen("test.txt", "r");
    char cmd[SZ];

    if (input == NULL) {
        printf("Error");
        return;
    }

    while (fgets(cmd, sizeof(cmd), input) != NULL && flag) {
        cmd[strcspn(cmd, "\n")] = 0;
        if (strlen(cmd) == 0)
            continue;
        Command(cmd);
    }

}
