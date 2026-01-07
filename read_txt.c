#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "read_text.h"

#define SZ 15

static const char* const cmdtype[COUNT] = {
    [WRITE] = "write",[READ] = "read",
    [ERASE] = "erase",[ERROR] = "error",
    [FULLWR] = "fullwrite",[FULLR] = "fullread",
    [EXIT] = "exit",[HELP] = "help"
};
int flag = 1;

Test SetCmd(char* cmd) {
    for (int i = 0; i < COUNT; i++) {
        if (!cmdtype[i])
            continue;
        if (strcmp(cmd, cmdtype[i]) == 0)
            return (Test)i;
    }
    return ERROR;
}


int GetCmd(Set* set, char* cmd) {
    char* tok = strtok(cmd, " ");

    if (tok == 0)
        return 0;
    set->ins = SetCmd(tok);

    switch (set->ins) {
    case WRITE: {
        char* add = strtok(NULL, " ");
        char* data = strtok(NULL, " ");
        if (!add || !data)
            return 0;
        set->add = atoi(add);
        set->data = strtoul(data, NULL, 16);
        return 1;
    }
    case ERASE: case READ: {
        char* add = strtok(NULL, " ");
        if (!add)
            return 0;
        set->add = atoi(add);
        return 1;
    }
    case FULLR: case FULLWR: case HELP: case EXIT:
        return 1;
    case ERROR: default:
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
        printf("write %d %x\n", set.add, set.data);
        break;
    case READ:
        printf("read %d\n", set.add);
        break;
    case ERASE:
        printf("erase %d\n", set.add);
        break;
    case FULLR:
        printf("fullr\n");
        break;
    case FULLWR:
        printf("fullwr\n");
        break;
    case HELP:
        printf("write, read, erase, fullread, fullwrite\n");
        break;
    case EXIT:
        printf("exit\n");
        flag = 0;
        break;
    default:
        printf("error");
        flag = 0;
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
        Command(cmd);
    }

}