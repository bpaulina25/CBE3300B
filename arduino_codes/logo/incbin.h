#pragma once


#define INCBIN(name, file) \
 __asm__(".section .rodata\n" \
         ".global " #name "Data\n" \
         ".balign 4\n" \
         #name "Data:\n" \
         ".incbin \"" file "\"\n" \
         ".global " #name "End\n" \
         ".balign 4\n" \
         #name "End:\n" \
         ".byte 0\n" \
         ".text\n"); \
 extern const unsigned char name##Data[]; \
 extern const unsigned char name##End[]
