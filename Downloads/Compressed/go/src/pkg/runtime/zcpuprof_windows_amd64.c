// auto generated by go tool dist
// goos=windows goarch=amd64


#include "runtime.h"
#include "arch_GOARCH.h"
#include "malloc.h"

#line 56 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
enum 
{ 
HashSize = 1<<10 , 
LogSize = 1<<17 , 
Assoc = 4 , 
MaxStack = 64 , 
} ; 
#line 64 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
typedef struct Profile Profile; 
typedef struct Bucket Bucket; 
typedef struct Entry Entry; 
#line 68 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
struct Entry { 
uintptr count; 
uintptr depth; 
uintptr stack[MaxStack]; 
} ; 
#line 74 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
struct Bucket { 
Entry entry[Assoc]; 
} ; 
#line 78 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
struct Profile { 
bool on; 
Note wait; 
uintptr count; 
uintptr evicts; 
uintptr lost; 
#line 86 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
Bucket hash[HashSize]; 
#line 91 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
uintptr log[2][LogSize/2]; 
uintptr nlog; 
int32 toggle; 
uint32 handoff; 
#line 99 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
uint32 wtoggle; 
bool wholding; 
bool flushing; 
bool eod_sent; 
} ; 
#line 105 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
static Lock lk; 
static Profile *prof; 
#line 108 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
static void tick ( uintptr* , int32 ) ; 
static void add ( Profile* , uintptr* , int32 ) ; 
static bool evict ( Profile* , Entry* ) ; 
static bool flushlog ( Profile* ) ; 
#line 113 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
static uintptr eod[3] = { 0 , 1 , 0 } ; 
#line 118 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
static void 
LostProfileData ( void ) 
{ 
} 
#line 125 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
void 
runtime·SetCPUProfileRate ( intgo hz ) 
{ 
uintptr *p; 
uintptr n; 
#line 132 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
if ( hz < 0 ) 
hz = 0; 
if ( hz > 1000000 ) 
hz = 1000000; 
#line 137 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
runtime·lock ( &lk ) ; 
if ( hz > 0 ) { 
if ( prof == nil ) { 
prof = runtime·SysAlloc ( sizeof *prof , &mstats.other_sys ) ; 
if ( prof == nil ) { 
runtime·printf ( "runtime: cpu profiling cannot allocate memory\n" ) ; 
runtime·unlock ( &lk ) ; 
return; 
} 
} 
if ( prof->on || prof->handoff != 0 ) { 
runtime·printf ( "runtime: cannot set cpu profile rate until previous profile has finished.\n" ) ; 
runtime·unlock ( &lk ) ; 
return; 
} 
#line 153 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
prof->on = true; 
p = prof->log[0]; 
#line 157 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
*p++ = 0; 
*p++ = 3; 
*p++ = 0; 
*p++ = 1000000 / hz; 
*p++ = 0; 
prof->nlog = p - prof->log[0]; 
prof->toggle = 0; 
prof->wholding = false; 
prof->wtoggle = 0; 
prof->flushing = false; 
prof->eod_sent = false; 
runtime·noteclear ( &prof->wait ) ; 
#line 170 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
runtime·setcpuprofilerate ( tick , hz ) ; 
} else if ( prof != nil && prof->on ) { 
runtime·setcpuprofilerate ( nil , 0 ) ; 
prof->on = false; 
#line 177 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
for ( ;; ) { 
n = prof->handoff; 
if ( n&0x80000000 ) 
runtime·printf ( "runtime: setcpuprofile(off) twice" ) ; 
if ( runtime·cas ( &prof->handoff , n , n|0x80000000 ) ) 
break; 
} 
if ( n == 0 ) { 
#line 186 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
runtime·notewakeup ( &prof->wait ) ; 
} 
} 
runtime·unlock ( &lk ) ; 
} 
#line 192 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
static void 
tick ( uintptr *pc , int32 n ) 
{ 
add ( prof , pc , n ) ; 
} 
#line 203 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
static void 
add ( Profile *p , uintptr *pc , int32 n ) 
{ 
int32 i , j; 
uintptr h , x; 
Bucket *b; 
Entry *e; 
#line 211 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
if ( n > MaxStack ) 
n = MaxStack; 
#line 215 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
h = 0; 
for ( i=0; i<n; i++ ) { 
h = h<<8 | ( h>> ( 8* ( sizeof ( h ) -1 ) ) ) ; 
x = pc[i]; 
h += x*31 + x*7 + x*3; 
} 
p->count++; 
#line 224 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
b = &p->hash[h%HashSize]; 
for ( i=0; i<Assoc; i++ ) { 
e = &b->entry[i]; 
if ( e->depth != n ) 
continue; 
for ( j=0; j<n; j++ ) 
if ( e->stack[j] != pc[j] ) 
goto ContinueAssoc; 
e->count++; 
return; 
ContinueAssoc:; 
} 
#line 238 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
e = &b->entry[0]; 
for ( i=1; i<Assoc; i++ ) 
if ( b->entry[i].count < e->count ) 
e = &b->entry[i]; 
if ( e->count > 0 ) { 
if ( !evict ( p , e ) ) { 
#line 245 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
p->lost++; 
return; 
} 
p->evicts++; 
} 
#line 252 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
e->depth = n; 
e->count = 1; 
for ( i=0; i<n; i++ ) 
e->stack[i] = pc[i]; 
} 
#line 264 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
static bool 
evict ( Profile *p , Entry *e ) 
{ 
int32 i , d , nslot; 
uintptr *log , *q; 
#line 270 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
d = e->depth; 
nslot = d+2; 
log = p->log[p->toggle]; 
if ( p->nlog+nslot > nelem ( p->log[0] ) ) { 
if ( !flushlog ( p ) ) 
return false; 
log = p->log[p->toggle]; 
} 
#line 279 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
q = log+p->nlog; 
*q++ = e->count; 
*q++ = d; 
for ( i=0; i<d; i++ ) 
*q++ = e->stack[i]; 
p->nlog = q - log; 
e->count = 0; 
return true; 
} 
#line 293 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
static bool 
flushlog ( Profile *p ) 
{ 
uintptr *log , *q; 
#line 298 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
if ( !runtime·cas ( &p->handoff , 0 , p->nlog ) ) 
return false; 
runtime·notewakeup ( &p->wait ) ; 
#line 302 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
p->toggle = 1 - p->toggle; 
log = p->log[p->toggle]; 
q = log; 
if ( p->lost > 0 ) { 
*q++ = p->lost; 
*q++ = 1; 
*q++ = ( uintptr ) LostProfileData; 
p->lost = 0; 
} 
p->nlog = q - log; 
return true; 
} 
#line 317 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
Slice 
getprofile ( Profile *p ) 
{ 
uint32 i , j , n; 
Slice ret; 
Bucket *b; 
Entry *e; 
#line 325 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
ret.array = nil; 
ret.len = 0; 
ret.cap = 0; 
#line 329 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
if ( p == nil ) 
return ret; 
#line 332 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
if ( p->wholding ) { 
#line 335 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
for ( ;; ) { 
n = p->handoff; 
if ( n == 0 ) { 
runtime·printf ( "runtime: phase error during cpu profile handoff\n" ) ; 
return ret; 
} 
if ( n & 0x80000000 ) { 
p->wtoggle = 1 - p->wtoggle; 
p->wholding = false; 
p->flushing = true; 
goto flush; 
} 
if ( runtime·cas ( &p->handoff , n , 0 ) ) 
break; 
} 
p->wtoggle = 1 - p->wtoggle; 
p->wholding = false; 
} 
#line 354 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
if ( p->flushing ) 
goto flush; 
#line 357 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
if ( !p->on && p->handoff == 0 ) 
return ret; 
#line 361 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
runtime·notetsleepg ( &p->wait , -1 ) ; 
runtime·noteclear ( &p->wait ) ; 
#line 364 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
n = p->handoff; 
if ( n == 0 ) { 
runtime·printf ( "runtime: phase error during cpu profile wait\n" ) ; 
return ret; 
} 
if ( n == 0x80000000 ) { 
p->flushing = true; 
goto flush; 
} 
n &= ~0x80000000; 
#line 376 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
p->wholding = true; 
#line 378 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
ret.array = ( byte* ) p->log[p->wtoggle]; 
ret.len = n*sizeof ( uintptr ) ; 
ret.cap = ret.len; 
return ret; 
#line 383 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
flush: 
#line 388 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
for ( i=0; i<HashSize; i++ ) { 
b = &p->hash[i]; 
for ( j=0; j<Assoc; j++ ) { 
e = &b->entry[j]; 
if ( e->count > 0 && !evict ( p , e ) ) { 
#line 394 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
goto breakflush; 
} 
} 
} 
breakflush: 
#line 401 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
if ( p->nlog > 0 ) { 
#line 404 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
ret.array = ( byte* ) p->log[p->toggle]; 
ret.len = p->nlog*sizeof ( uintptr ) ; 
ret.cap = ret.len; 
p->nlog = 0; 
return ret; 
} 
#line 412 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
if ( !p->eod_sent ) { 
#line 415 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
p->eod_sent = true; 
ret.array = ( byte* ) eod; 
ret.len = sizeof eod; 
ret.cap = ret.len; 
return ret; 
} 
#line 423 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"
p->flushing = false; 
if ( !runtime·cas ( &p->handoff , p->handoff , 0 ) ) 
runtime·printf ( "runtime: profile flush racing with something\n" ) ; 
return ret; 
} 
void
runtime·CPUProfile(Slice ret)
{
	ret.array = 0;
	ret.len = 0;
	ret.cap = 0;
	FLUSH(&ret);
#line 431 "C:\Users\gopher\AppData\Local\Temp\1\makerelease745458658\go\src\pkg\runtime\cpuprof.goc"

	ret = getprofile(prof);
	FLUSH(&ret);
}
