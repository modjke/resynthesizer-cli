/*
Proxy for /usr/include/glib-2.0/glib.h

Glib is for portability, on top of ANSI C.
This is an alternative: does a subset of the same thing but without GPL license.

This is a limited subset: only what is used in synth.
*/

#define guint unsigned int
#define gint int
#define gint32 int
#define gushort short unsigned int
#define gulong long unsigned int

#define gfloat float
#define gdouble double

#define guint8 unsigned char
#define guchar unsigned char
#define gchar char

typedef const void *TConstPointer;
typedef gint (*TCompareFunc) (TConstPointer  a, TConstPointer  b);

// Apparently true, false are in ANSI C but not TRUE, FALSE
// Use enumerated type?
#define gboolean int
#define FALSE 0
#define TRUE 1

#define G_PI    3.1415926535897932384626433832795028841971693993751

// Glib defines based on limits.h
#include <limits.h>
#define G_MAXINT INT_MAX
#define G_MAXUINT UINT_MAX
#define G_MAXUSHORT USHRT_MAX

// NULL defined in stddef.h

#include <assert.h>
#define g_assert assert

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

/*
PRNG
Using ANSI c rand().
Note GRand type is passed, but not used,
since rand() keeps its own internal data.
*/

// When using this proxy with GIMP (for testing the proxy)
// We can't redefine certain structs (although we can redefine most other things.)
#ifndef SYNTH_USE_GLIB
typedef void GRand;
#endif

GRand *
s_rand_new_with_seed(guint seed);

guint
s_rand_int_range(
  GRand * prng,   // not used
  guint lowerBound,
  guint upperBound
  );


/*
Dynamic 1D array (sequence, vector.)

Note: GArrays are dynamically allocated on the heap
AND they grow automatically when appended to.
In synth, they are dynamically allocated
but the automatic growing is NOT needed,
since the size is known ahead of allocation.
IOW, synth only needs dynamically allocated arrays,
which can be accessed (read and write) by appropriate casting
with ordinary array indexing syntax.
The indirection through the GArray struct is NOT needed.
*/

#ifndef SYNTH_USE_GLIB
typedef struct _GArray
{
  gchar *data;
  guint len;
} GArray;
#endif

// We can always redefine this struct because it is private to glib.
typedef struct _GRealArray
{
  guint8 *data;
  guint   len;
  guint   alloc;
  guint   elt_size;
  guint   zero_terminated : 1;
  guint   clear : 1;
  gint    ref_count;
} GRealArray;


#define g_array_index(a,t,i)      (((t*) (void *) (a)->data) [(i)])

// Use macro to pass address of 2nd parameter
#define g_array_append_val(a,v)	  s_array_append_vals (a, &(v), 1)

// Simple redirecting to our implementation
#define g_array_sized_new(z,c,s,r)  s_array_sized_new (z,c,s,r)
#define g_array_sort(a,f) s_array_sort (a,f)
#define g_array_free(p,b) s_array_free(p,b)

GArray* 
s_array_sized_new (
  gboolean zero_terminated,
  gboolean clear,
  guint    elt_size,
  guint    reserved_size);
			   
GArray* 
s_array_append_vals(
  GArray           *array,
  TConstPointer     data,
  int               len   // unused
  );

void
s_array_sort (
  GArray       *farray,
  TCompareFunc  compare_func
  );

void
s_array_free(
  GArray * array,
  int   cascade
  );		   

#ifdef SYNTH_THREADED
// Proxies for thread mutex
// Redefine glib mutex to use POSIX pthread mutex
// Depends on <pthread.h>
// Other differences between glib and POSIX threading are #ifdefed in the code
#define g_static_mutex_init(A)      pthread_mutex_init(A, NULL);        // POSIX additional parameter
#define g_static_mutex_lock(A)      pthread_mutex_lock(A)
#define g_static_mutex_unlock(A)    pthread_mutex_unlock(A)

#define g_mutex_init(A)      pthread_mutex_init(A, NULL);        // POSIX additional parameter
#define g_mutex_lock(A)      pthread_mutex_lock(A)
#define g_mutex_unlock(A)    pthread_mutex_unlock(A)
#endif