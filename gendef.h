// Define Flags for Compilation
#define SHORTCODE
#define JOBS
#define LIST
#define STAB

// Define boundary values for parameters
#define MIN_N  3  // Minimum Number of Nodes
#define MAX_N 99  // Maximum Number of Nodes
#define MIN_K  2  // Minimum Value for k, in the context of k-connected graph
#define MAX_K  9  // Maximum Value for k, in the context of k-connected graph
#define MIN_T  3  // Minimum Value for t; t is used to control the lower bound of girth (graphs with girth ≥ t)
#define MAX_T  9  // Maximum Value for t; t is used to control the lower bound of girth (graphs with girth ≥ t)

#define NOTSET 127
#define COUNTSTEP 100

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Defining time settings based on platform configuration 
#ifdef __UNIX__
#include <sys/times.h>
#include <time.h>
#include <limits.h>
#else
#include <time.h>
#define DOSTIME
#endif

#ifdef __osf__ /* OSF on the Alphas */
#define time_factor CLK_TCK
#endif

#ifdef __sgi /* Silicon Graphics */
#define time_factor CLK_TCK 
#define SIGNED 
#endif

#ifdef sun /* Sun */
#define time_factor 60
#endif

#ifdef __hpux /* Hewlett Packard */
#define time_factor CLK_TCK
#endif

#ifdef __dec /* DEC */
#define time_factor 60
#endif

#ifdef __ultrix /* DEC */
#define time_factor 60
#endif        /* Thanks to Gunnar */

#ifdef time_factor
#define UNIXTIME
#endif

#ifdef DOSTIME
#define time_factor CLK_TCK
#endif

#ifdef SHORTCODE
#define APP "scd"
#else
#define APP "lst"
#endif
    