// ----------------------------------------------------------- 
// NAME : Kyler Witting                      User ID: kmwittin 
// DUE DATE : 04/15/2016
// PROGRAM ASSIGNMENT 5
// FILE NAME : thread.cpp
// PROGRAM PURPOSE :
//      This simulates the threads for each baboon that is trying to 
//      cross the rope. 
//      This also contains the monitor used to ensure that baboons are 
//      crossing the rope properly.       
// -----------------------------------------------------------


#include <stdio.h> //Used for sprintf
#include <string.h> //Used for strlen
#include "thread.h"

//Used to insure proper printing order
static Mutex Print;

//Monitor for the Rope
static RopeMonitor Rope( "Rope" );

//---------------------------------------------------------------//
//  Monkey Class: 
//     constructor
//---------------------------------------------------------------//

    MonkeyThread::MonkeyThread( int id, int dir, int max )
:ID( id ), Max( max ), Direction( dir )
{
    int i;
    char buffer[1024];

    Print.Lock();
    sprintf( buffer, "  " );
    for( i = 1; i < id; i++ ) {
        sprintf( buffer+strlen(buffer), "  " );
    }
    if( dir == 0 )
        sprintf( buffer+strlen(buffer), "Eastward-moving baboon %d started\n", id );
    else
        sprintf( buffer+strlen(buffer), "Westward-moving baboon %d started\n", id );
    write( 1, buffer, strlen( buffer ) );
    Print.Unlock();
};

//---------------------------------------------------------------//
//  Monkey Thread Class body:
//      The baboon goes through the loop the max number of times 
//      (specified by the arguments given) and then exits 
//      In each time trying to cross, it prints where it is at in 
//      it's journey and uses the monitors consctructed to 
//      ask permission. 
//      Print.Lock and Unlock are used to ensure proper printing 
//      when mutual exclusion is not gurenteed outside of the 
//      monitor. 
//---------------------------------------------------------------//

void MonkeyThread::ThreadFunc( ) {
    Thread::ThreadFunc();
    //loop iterators & buffer for printing 
    int i, j;
    char buffer[1024];

    for( i = 0; i < Max; i++ ) {
        //Walking towards the rope
        //The baboon will wait a random amount of time before going 
        //back to the rope. 
        srand ( time(NULL) * ID  );
        for( j = 0; j < rand() % 10000000; j++ ) {
            Delay();
        }

        //---------------------------------------------------------------------
        //Arrives at the Rope 
        Print.Lock();
        sprintf( buffer, "  " );
        for( j = 1; j < ID; j++ ) {
            sprintf( buffer+strlen(buffer), "  " );
        }
        if( Direction == 0 )
            sprintf( buffer+strlen(buffer), "Eastward-moving baboon %d arrives at the west end\n", ID );
        else
            sprintf( buffer+strlen(buffer), "Westward-moving baboon %d arrives at the east end\n", ID );
        write( 1, buffer, strlen( buffer ) );
        Print.Unlock();

        Rope.GetOnRope( Direction );

        //--------------------------------------------------------------------
        //Is now on the Rope and climbing across

        Print.Lock();
        sprintf( buffer, "  " );
        for( j = 1; j < ID; j++ ) {
            sprintf( buffer+strlen(buffer), "  " );
        }
        if( Direction == 0 )
            sprintf( buffer+strlen(buffer), "Eastward-moving baboon %d is on the rope\n", ID );
        else
            sprintf( buffer+strlen(buffer), "Westward-moving baboon %d is on the rope\n", ID );
        write( 1, buffer, strlen( buffer ) );
        Print.Unlock();

        //This is the delay time to get accross the rope 
        Delay();
        for( j = 0; j < (rand() % 10000000) * 2; j++ ) {
            Delay();
        }

        //---------------------------------------------------------------------
        //Got to the other side 
        Print.Lock();
        Rope.GetOffRope( Direction );

        sprintf( buffer, "  " );
        for( j = 1; j < ID; j++ ) {
            sprintf( buffer+strlen(buffer), "  " );
        }
        if( Direction == 0 )
            sprintf( buffer+strlen(buffer), "Eastward-moving baboon %d completes crossing the canyon (%d)\n", ID, i+1 );
        else
            sprintf( buffer+strlen(buffer), "Westward-moving baboon %d completes crossing the canyon (%d)\n", ID, i+1 );
        write( 1, buffer, strlen( buffer ) );
        Print.Unlock();
    }

    //Prints out the Baboon's final message
    Print.Lock();
    sprintf( buffer, "  " );
    for( j = 1; j < ID; j++ ) {
        sprintf( buffer+strlen(buffer), "  " );
    }
    if( Direction == 0 )
        sprintf( buffer+strlen(buffer), "Eastward-moving baboon %d completes all (%d) crossings and retires. Bye-Bye!\n", ID, Max );
    else
        sprintf( buffer+strlen(buffer), "Westward-moving baboon %d completes all (%d) crossings and retires. Bye-Bye!\n", ID, Max );
    write( 1, buffer, strlen( buffer ) );
    Print.Unlock();
    Exit();
}

//---------------------------------------------------------------//
//  Rope Monitor Class: 
//     constructor
//---------------------------------------------------------------//
RopeMonitor::RopeMonitor(char* Name): Monitor(Name, HOARE) 
{
    //Count of Monkeys on the rope. Initially 0
    MonkeyCount = 0;                                    
    
    //Turn[1] is the current direction (Doesn't make a difference of what it 
    //starts at)
    //Turn[0] shows if a baboon from the other side showed up 
    //0 = Eastward && 1 = Westward
    Turn[0] = Turn[1] = 1;
    
    //The count of baboons waiting to cross the rope 
    Waiting[0] = Waiting[1] = 0;

    //The line for Wait and signal of the 2 lines of baboons 
    WaitingLine[0] = new Condition( "Eastward" );
    WaitingLine[1] = new Condition( "Westward" );
};

//---------------------------------------------------------------//
//  Rope Monitor GetOnRope body:
//      Checks to see if the rope is safe to cross
//      Gives the baboon permission to cross. If there are more 
//      baboons in line behind them, they also go. 
//      The direction is set to the way these baboons are going 
//
//      If the rope isn't safe, then it checks to see if they need 
//      to not allow any more of the other direction to continue
//      and sets the flag to their turn 
//      Then they wait for a signal that the rope is empty 
//---------------------------------------------------------------//
void RopeMonitor::GetOnRope( int Direction )
{
    MonitorBegin();
    
    if( !RopeSafety(Direction) ) {
        Waiting[Direction]++;
        //If a baboon on the other side hasn't shown up
        //Flips the turn to this side, and then sets turn 0 to 0 
        //so everyone knows to wait because it is this side's turn
        if( Turn[0] ) {
            Turn[1] = Direction;
            Turn[0] = 0;
        }
        WaitingLine[Direction]->Wait();
        Waiting[Direction]--;     
    }
    //If there are baboons waiting in line, the lead baboon has to signal 
    //them so they can cross before other baboons show up. 
    if( Waiting[Direction] != 0 && Turn[0] ) 
        WaitingLine[Direction]->Signal();

    //The baboon is on the rope now 
    MonkeyCount++;
    if( Turn[0] )
        Turn[1] = Direction;
    
    MonitorEnd();
}

//---------------------------------------------------------------//
//  Rope Monitor GetOffRope body:
//      This method ends the baboon on the rope. Checks if the 
//      rope is completely empty, 
//---------------------------------------------------------------//
void RopeMonitor::GetOffRope( int Direction )
{
    MonitorBegin();

    //Removes this baboon from the rope 
    MonkeyCount--;
    //If this baboon was the last one, resets the Turn var
    //Then it gives the other side priority to get on the rope
    //If there is no line on the other side, then he allows his side to go
    if( MonkeyCount == 0 ) {
        Turn[0] = 1;
        if (Waiting[1-Direction] != 0) 
            WaitingLine[1-Direction]->Signal();
        else
            WaitingLine[Direction]->Signal();
    }

    MonitorEnd();
}

//---------------------------------------------------------------//
//  Rope Monitor GetOnRope body:
//      Simple Checking method for the GetOnRope
//      Checks if the MonkeyCount is 0, if it is then it 
//      says the rope is safe. 
//      If Turn[0] is untouched and Turn1 is the current direction
//      then it will say the rope is safe. 
//      Anything else and the rope is not good to cross 
//---------------------------------------------------------------//
int RopeMonitor::RopeSafety( int Direction ) 
{ 
    if( MonkeyCount == 0 ) 
        return 1; 
    else if( Turn[1] == Direction && Turn[0] == 1 )
        return 1;
    else 
        return 0;
}
