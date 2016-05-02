// ----------------------------------------------------------- 
// NAME : Kyler Witting                      User ID: kmwittin 
// DUE DATE : 04/01/2016
// PROGRAM ASSIGNMENT 4
// FILE NAME : thread.h
// PROGRAM PURPOSE :
//      Head file for class Thread classes definitions
// -----------------------------------------------------------

#ifndef _THREAD_H
#define _THREAD_H

// header file
#include "ThreadClass.h"

class RopeMonitor: public Monitor 
{
    public:
        RopeMonitor(char* Name);    // constructor  
        void GetOnRope ( int Direction );
        void GetOffRope( int Direction );
        int  RopeSafety( int Direction );

    private:
        Condition *WaitingLine[2];
        int MonkeyCount;
        int  Waiting[2];  
        int Turn[2]; 

};

//---------------------------------------------------------------//
//  Monkey Thread Class: 
//       Contstuctor for the Monkey Thread
//---------------------------------------------------------------//
class MonkeyThread : public Thread
{
    public:
        // constructor
        MonkeyThread( int id, int dir, int max );

    private:
        void ThreadFunc();  // thread body
        int ID;
        int Direction;
        int Max;
};
#endif
