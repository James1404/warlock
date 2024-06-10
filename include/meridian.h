#ifndef MERIDIAN_H
#define MERIDIAN_H

void Meridian_init();
void Meridian_free();
void Meridian_builtin();

void Meridian_run(const char* src);
void Meridian_run_file(const char* path);

#endif//MERIDIAN_H
