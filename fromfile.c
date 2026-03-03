#include "lisp.h"

void runFileTests(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open %s\n", filename);
        return;
    }
    
    char line[1024];
    
    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        line[strcspn(line, "\n")] = 0;

        char* trimmed = line;
        while (*trimmed && isspace((unsigned char)*trimmed)) {
            trimmed++;
        }
        
        if (strlen(trimmed) == 0 || trimmed[0] == '#') {
            continue;
        }
        
        printf(">%s\n", line);
        printf(":");
        
        Parser parser;
        parser.input = line;
        parser.pos = 0;
        
        while (next(&parser)) {
            SExp* result = nextSexp(&parser);
            if (result) {
                printSexp(result);
                printf(" ");
                freeSexp(result);
            }
        }
        printf("\n");
    }
    fclose(file);
}

int main() {
    runFileTests("test_cases.txt");
    return 0;
}