#ifndef SCHEME_H
#define SCHEME_H

void Scheme_init(void);
void Scheme_free(void);
void Scheme_builtin(void);

void Scheme_run(const char* src);
void Scheme_run_file(const char* path);

#endif//SCHEME_H
