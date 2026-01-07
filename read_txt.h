#ifndef READ_TXT_H
#define READ_TXT_H
typedef enum {
    WRITE, READ, ERASE,
    ERROR, HELP, FULLWR,
    FULLR, EXIT, COUNT
}Test;

typedef struct {
    Test ins;
    int add, data;
}Set;

Test SetCmd(char* cmd);
int GetCmd(Set* set, char* cmd);
void Command(char* cmd);
void Runner(void);

#endif READ_TXT_H