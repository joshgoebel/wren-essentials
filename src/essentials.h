#ifndef essentials_h
#define essentials_h

#include "wren.h"

#define MAX_METHODS_PER_CLASS 14
#define MAX_CLASSES_PER_MODULE 6
#define MAX_MODULES_PER_LIBRARY 20
#define MAX_LIBRARIES 20

typedef struct
{
  bool isStatic;
  const char* signature;
  WrenForeignMethodFn method;
} MethodRegistry;

// Describes one class in a built-in module.
typedef struct
{
  const char* name;

  MethodRegistry methods[MAX_METHODS_PER_CLASS];
} ClassRegistry;

// Describes one built-in module.
typedef struct
{
  // The name of the module.
  const char* name;

  // Pointer to the string containing the source code of the module. We use a
  // pointer here because the string variable itself is not a constant
  // expression so can't be used in the initializer below.
  const char **source;

  ClassRegistry classes[MAX_CLASSES_PER_MODULE];
} ModuleRegistry;

typedef struct
{
  const char* name;

  ModuleRegistry (*modules)[MAX_MODULES_PER_LIBRARY];
} LibraryRegistry;

#endif