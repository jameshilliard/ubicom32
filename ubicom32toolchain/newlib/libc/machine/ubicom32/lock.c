/* global lock areas */

long __mlock[3];  /* malloc lock for malloc family */
long __elock[3];  /* env lock for getenv/setenv */
