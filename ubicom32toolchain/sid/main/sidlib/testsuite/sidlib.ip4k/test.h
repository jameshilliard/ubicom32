/* Testing utilities for ip4k sidlib testing */

#define PASS { printf ("pass\n"); return 0; }

#define FAIL(exp, file, line) { \
  printf ("failed test: \"%s\" at line %d in file %s\n", exp, line, file); \
}

#define CHECK(exp) { \
  if (!(exp)) \
    FAIL(#exp, __FILE__, __LINE__) \
}
