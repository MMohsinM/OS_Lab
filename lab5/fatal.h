//
// Created by mohsin on 15/5/21.
//

#ifndef UNTITLED2_FATAL_H
#define UNTITLED2_FATAL_H
#include <stdio.h>
#include <stdlib.h>

#define Error( Str )        FatalError( Str )
#define FatalError( Str )   fprintf( stderr, "%s\n", Str ), exit( 1 )
#endif //UNTITLED2_FATAL_H
