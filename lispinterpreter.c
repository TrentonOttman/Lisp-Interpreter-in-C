#include "lisp.h"

int main() {
    char string[1024];
    GLOBAL_ENV = createEnv();

    while (1) { 
        printf(">");
        if (fgets(string, sizeof(string), stdin) != NULL) {
            string[strcspn(string, "\n")] = 0;

            if (strlen(string) == 0) {
                continue;
            }

            Parser parser;
            parser.input = string;
            parser.pos = 0;

            while (next(&parser)) {
                SExp* parsed = nextSexp(&parser);
            
                if (parsed) {
                    if (isError(parsed)) {
                        printf(":");
                        printSexp(parsed);
                        printf("\n");
                    } else {
                        printf(":");
                        printSexp(eval(&GLOBAL_ENV, parsed));
                        printf("\n"); 
                    }
                }
            }
        } else {
            break;
        }
    }
    return 0;
}