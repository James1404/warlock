#ifndef WARLOCK_GC_H
#define WARLOCK_GC_H

typedef enum Marking {
    MARKING_WHITE,
    MARKING_GRAY,
    MARKING_BLACK,
} Marking;

typedef struct GC {
    void* ptr;
} GC;

void GC_mark_sweep(GC* gc);

#endif // WARLOCK_GC_H
