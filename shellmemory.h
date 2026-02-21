#define MEM_SIZE 1000
extern int next_free_line;
void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
void mem_free_program_lines(int start, int end);
char* mem_get_program_line(int index);
int mem_load_script(char* filename);
