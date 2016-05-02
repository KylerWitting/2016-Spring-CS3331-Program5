// ----------------------------------------------------------- 
// NAME : Kyler Witting                      User ID: kmwittin 
// DUE DATE : 04/15/2016
// PROGRAM ASSIGNMENT 5
// FILE NAME : thread-main.cpp
// PROGRAM PURPOSE :
//      This is the main thread program. It takes the 
//      arguments from the command line. Then it creates the 
//      threads for each monkey. Then waits to join them. 
//      Then it Exits
// -----------------------------------------------------------

#include <stdio.h>
#include "thread.h"

//---------------------------------------------------------------//
//  main program 
//---------------------------------------------------------------//

int main( int argc, char *argv[] ) {
    //Program Variables
    int eastCount, westCount, tripCount; 	
    //loop iterator
    int i;

    //checks arguments, if 0 sets to defaults
    //guarantees no negative numbers 
    if ( argc ==  4 ){ 
        eastCount = atoi( argv[1] );
        westCount = atoi( argv[2] );
        tripCount = atoi( argv[3] );

        if( eastCount == 0 )
            eastCount = 10;
        if( westCount == 0 )
            westCount = 10;
        if( tripCount == 0 )
            tripCount = 10;
    }
    else { 
        fprintf( stdout, "Incorrect Number of arguments. Now exiting.\n" );
        return -1; 
    }

    //Starts the Monkey threads
    MonkeyThread *eThread[ eastCount ];
    for( i = 0; i < eastCount; i++ ) { 
        eThread[i] = new MonkeyThread( i+1, 0, tripCount );
        eThread[i]->Begin();
    }

    MonkeyThread *wThread[ westCount ];
    for( i = 0; i < westCount; i++ ) { 
        wThread[i] = new MonkeyThread( i+1, 1, tripCount );
        wThread[i]->Begin();
    }

    //Joins all the Monkey Threads and then exits
    for( i = 0; i < eastCount; i++ )
        eThread[i]->Join();
    for( i = 0; i < westCount; i++ )
        wThread[i]->Join();
    Exit();
    return 0; 
}
