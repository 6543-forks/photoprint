#include <iostream>

#include "config.h"

#include "thread.h"
#include "rwmutex.h"
#include "progressthread.h"

using namespace std;

int MyThread(Thread *t,void *ud)
{
	RWMutex *mutex=(RWMutex *)ud;

	cerr << "Subthread attempting write lock" << endl;
	if(mutex->AttemptMutex())
	{
		cerr << "Obtained write-lock" << endl;
		mutex->ReleaseMutex();
	}
	else
		cerr << "Subthread Can't get write lock - read lock held elsewhere?" << endl;
	mutex->ObtainMutexShared();
	cerr << "Got shared mutex" << endl;

	cerr << "Sub-thread about to sleep..." << endl;
	ProgressThread p(*t);
	t->SendSync();
	for(int i=0;i<100;++i)
	{
#ifdef WIN32
		Sleep(20);
#else
		usleep(20000);
#endif
		if(!p.DoProgress())
		{
			cerr << "Thread cancelled - exiting" << endl;
			mutex->ReleaseMutex();
			return(0);
		}
	}

	cerr << "Woken up - attempting exclusive lock (with shared lock still held)" << endl;
	mutex->ObtainMutex();
	cerr << "Subthread got exclusive lock" << endl;
	mutex->ObtainMutexShared();
	cerr << "Subthread got shared lock (with exclusive lock still held)" << endl;

	mutex->ReleaseMutex();
	mutex->ReleaseMutex();

	t->SendSync();
#ifdef WIN32
		Sleep(5000);
#else
		sleep(5);
#endif
	mutex->ReleaseMutex();
	cerr << "Thread finished sleeping - exiting" << endl;
//	cerr << "Sub-thread ID: " << (long)pthread_self() << endl;
	return(0);
}


int main(int argc, char **argv)
{
	RWMutex mutex;

	mutex.ObtainMutexShared();	// Get a non-exclusive lock...

	cerr << "Got shared lock" << endl;

	Thread t(MyThread,&mutex);
	t.Start();
	t.WaitSync();

	mutex.ReleaseMutex();

	t.WaitSync();

	mutex.ObtainMutexShared();

#if 0
	while(!t.TestFinished())
	{
		cerr << "Thread still running - sleeping for 1 second" << endl;
		sleep(1);
		cerr << "Sending break..." << endl;
		t.Stop();
	}
#endif
	t.WaitFinished();

	cerr << "Done" << endl;

	return(0);
}
