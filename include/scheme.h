#ifndef MERIDIAN_H
#define MERIDIAN_H

void Scheme_init(void);
void Scheme_free(void);
void Scheme_builtin(void);

void Scheme_run(const char* src);
void Scheme_run_file(const char* path);

#endif//MERIDIAN_H
