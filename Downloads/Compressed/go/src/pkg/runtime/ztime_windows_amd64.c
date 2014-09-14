// auto generated by go tool dist
// goos=windows goarch=amd64


#include "runtime.h"
#include "defs_GOOS_GOARCH.h"
#include "os_GOOS.h"
#include "arch_GOARCH.h"
#include "malloc.h"
#include "race.h"

#line 16 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
enum { 
debug = 0 , 
} ; 
#line 20 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
static Timers timers; 
static void addtimer ( Timer* ) ; 
static void dumptimers ( int8* ) ; 
#line 25 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
int64 runtime·timens; 
void
time·runtimeNano(int64 ns)
{
	ns = 0;
	FLUSH(&ns);
#line 33 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"

	ns = runtime·nanotime();
	FLUSH(&ns);
}
void
time·Sleep(int64 ns)
{
#line 38 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"

	runtime·tsleep(ns, "sleep");
}
void
time·startTimer(Timer* t)
{
#line 43 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"

	if(raceenabled)
		runtime·racerelease(t);
	runtime·addtimer(t);
}
void
time·stopTimer(Timer* t, bool stopped)
{
	stopped = 0;
	FLUSH(&stopped);
#line 51 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"

	stopped = runtime·deltimer(t);
	FLUSH(&stopped);
}

#line 57 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
void runtime·gc_unixnanotime ( int64 *now ) ; 
#line 59 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
int64 runtime·unixnanotime ( void ) 
{ 
int64 now; 
#line 63 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
runtime·gc_unixnanotime ( &now ) ; 
return now; 
} 
#line 67 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
static void timerproc ( void ) ; 
static void siftup ( int32 ) ; 
static void siftdown ( int32 ) ; 
#line 72 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
static void 
ready ( int64 now , Eface e ) 
{ 
USED ( now ) ; 
#line 77 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
runtime·ready ( e.data ) ; 
} 
#line 80 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
static FuncVal readyv = { ( void ( * ) ( void ) ) ready } ; 
#line 83 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
void 
runtime·tsleep ( int64 ns , int8 *reason ) 
{ 
Timer t; 
#line 88 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
if ( ns <= 0 ) 
return; 
#line 91 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
t.when = runtime·nanotime ( ) + ns; 
t.period = 0; 
t.fv = &readyv; 
t.arg.data = g; 
runtime·lock ( &timers ) ; 
addtimer ( &t ) ; 
runtime·parkunlock ( &timers , reason ) ; 
} 
#line 100 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
static FuncVal timerprocv = { timerproc } ; 
#line 102 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
void 
runtime·addtimer ( Timer *t ) 
{ 
runtime·lock ( &timers ) ; 
addtimer ( t ) ; 
runtime·unlock ( &timers ) ; 
} 
#line 112 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
static void 
addtimer ( Timer *t ) 
{ 
int32 n; 
Timer **nt; 
#line 120 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
if ( t->when < 0 ) 
t->when = ( 1LL<<63 ) -1; 
#line 123 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
if ( timers.len >= timers.cap ) { 
#line 125 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
n = 16; 
if ( n <= timers.cap ) 
n = timers.cap*3 / 2; 
nt = runtime·malloc ( n*sizeof nt[0] ) ; 
runtime·memmove ( nt , timers.t , timers.len*sizeof nt[0] ) ; 
runtime·free ( timers.t ) ; 
timers.t = nt; 
timers.cap = n; 
} 
t->i = timers.len++; 
timers.t[t->i] = t; 
siftup ( t->i ) ; 
if ( t->i == 0 ) { 
#line 139 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
if ( timers.sleeping ) { 
timers.sleeping = false; 
runtime·notewakeup ( &timers.waitnote ) ; 
} 
if ( timers.rescheduling ) { 
timers.rescheduling = false; 
runtime·ready ( timers.timerproc ) ; 
} 
} 
if ( timers.timerproc == nil ) { 
timers.timerproc = runtime·newproc1 ( &timerprocv , nil , 0 , 0 , addtimer ) ; 
timers.timerproc->issystem = true; 
} 
if ( debug ) 
dumptimers ( "addtimer" ) ; 
} 
#line 159 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
bool 
runtime·deltimer ( Timer *t ) 
{ 
int32 i; 
#line 166 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
i = t->i; 
USED ( i ) ; 
#line 169 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
runtime·lock ( &timers ) ; 
#line 174 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
i = t->i; 
if ( i < 0 || i >= timers.len || timers.t[i] != t ) { 
runtime·unlock ( &timers ) ; 
return false; 
} 
#line 180 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
timers.len--; 
if ( i == timers.len ) { 
timers.t[i] = nil; 
} else { 
timers.t[i] = timers.t[timers.len]; 
timers.t[timers.len] = nil; 
timers.t[i]->i = i; 
siftup ( i ) ; 
siftdown ( i ) ; 
} 
if ( debug ) 
dumptimers ( "deltimer" ) ; 
runtime·unlock ( &timers ) ; 
return true; 
} 
#line 200 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
static void 
timerproc ( void ) 
{ 
int64 delta , now; 
Timer *t; 
void ( *f ) ( int64 , Eface ) ; 
Eface arg; 
#line 208 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
for ( ;; ) { 
runtime·lock ( &timers ) ; 
timers.sleeping = false; 
now = runtime·nanotime ( ) ; 
for ( ;; ) { 
if ( timers.len == 0 ) { 
delta = -1; 
break; 
} 
t = timers.t[0]; 
delta = t->when - now; 
if ( delta > 0 ) 
break; 
if ( t->period > 0 ) { 
#line 223 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
t->when += t->period * ( 1 + -delta/t->period ) ; 
siftdown ( 0 ) ; 
} else { 
#line 227 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
timers.t[0] = timers.t[--timers.len]; 
timers.t[0]->i = 0; 
siftdown ( 0 ) ; 
t->i = -1; 
} 
f = ( void* ) t->fv->fn; 
arg = t->arg; 
runtime·unlock ( &timers ) ; 
if ( raceenabled ) 
runtime·raceacquire ( t ) ; 
f ( now , arg ) ; 
#line 240 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
f = nil; 
USED ( f ) ; 
arg.type = nil; 
arg.data = nil; 
USED ( &arg ) ; 
#line 246 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
runtime·lock ( &timers ) ; 
} 
if ( delta < 0 ) { 
#line 250 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
timers.rescheduling = true; 
g->isbackground = true; 
runtime·parkunlock ( &timers , "timer goroutine (idle)" ) ; 
g->isbackground = false; 
continue; 
} 
#line 257 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
timers.sleeping = true; 
runtime·noteclear ( &timers.waitnote ) ; 
runtime·unlock ( &timers ) ; 
runtime·notetsleepg ( &timers.waitnote , delta ) ; 
} 
} 
#line 266 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
static void 
siftup ( int32 i ) 
{ 
int32 p; 
int64 when; 
Timer **t , *tmp; 
#line 273 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
t = timers.t; 
when = t[i]->when; 
tmp = t[i]; 
while ( i > 0 ) { 
p = ( i-1 ) /4; 
if ( when >= t[p]->when ) 
break; 
t[i] = t[p]; 
t[i]->i = i; 
t[p] = tmp; 
tmp->i = p; 
i = p; 
} 
} 
#line 288 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
static void 
siftdown ( int32 i ) 
{ 
int32 c , c3 , len; 
int64 when , w , w3; 
Timer **t , *tmp; 
#line 295 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
t = timers.t; 
len = timers.len; 
when = t[i]->when; 
tmp = t[i]; 
for ( ;; ) { 
c = i*4 + 1; 
c3 = c + 2; 
if ( c >= len ) { 
break; 
} 
w = t[c]->when; 
if ( c+1 < len && t[c+1]->when < w ) { 
w = t[c+1]->when; 
c++; 
} 
if ( c3 < len ) { 
w3 = t[c3]->when; 
if ( c3+1 < len && t[c3+1]->when < w3 ) { 
w3 = t[c3+1]->when; 
c3++; 
} 
if ( w3 < w ) { 
w = w3; 
c = c3; 
} 
} 
if ( w >= when ) 
break; 
t[i] = t[c]; 
t[i]->i = i; 
t[c] = tmp; 
tmp->i = c; 
i = c; 
} 
} 
#line 331 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
static void 
dumptimers ( int8 *msg ) 
{ 
Timer *t; 
int32 i; 
#line 337 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\time.goc"
runtime·printf ( "timers: %s\n" , msg ) ; 
for ( i = 0; i < timers.len; i++ ) { 
t = timers.t[i]; 
runtime·printf ( "\t%d\t%p:\ti %d when %D period %D fn %p\n" , 
i , t , t->i , t->when , t->period , t->fv->fn ) ; 
} 
runtime·printf ( "\n" ) ; 
} 