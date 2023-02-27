#ifndef COMMON_H
#define COMMON_H
#include "Arduino.h"

// To handle base case of below recursive
// Variadic function Template
inline void serialPrintLine()
{
  Serial.println("");
}
 
// Variadic function Template that takes
// variable number of arguments and prints
// all of them in one line (spaced by " ")
template <typename T, typename... Types> 
inline void serialPrintLine(T var1, Types... var2)
{
  Serial.print(var1);
  serialPrintLine(var2...);
}

// To handle base case of below recursive
// Variadic function Template
inline void serialPrint()
{
  Serial.print("");
}
 
// Variadic function Template that takes
// variable number of arguments and prints
// all of them in one line (spaced by " ")
template <typename T, typename... Types> 
inline void serialPrint(T var1, Types... var2)
{
  Serial.print(var1);
  serialPrint(var2...);
}

#endif