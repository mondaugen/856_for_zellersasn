#ifndef VERSION_H
#define VERSION_H 

#ifdef BOARD_V1 
#define ZELLERSASN_VERSION "1.1b1"
#elif BOARD_V2
#define ZELLERSASN_VERSION "1.1b2"
#else
#error("Define board version.")
#endif  

#endif /* VERSION_H */
