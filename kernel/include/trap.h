// trap.h

#ifndef TRAP

#define TRAP

void ktrap_install_handler(void);
void ktrap_handler(void);

void utrap_handler(void);
void utrap_return(void);

extern void kernel_handler(void);

#endif