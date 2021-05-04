#ifndef NTK_LIBRARY_H
#define NTK_LIBRARY_H

#include <stddef.h>

/**
 * @brief Check whether a given buffer is a valid UTF-8 string.
 * @param pStr Buffer to check.
 * @param len Length of the buffer.
 * @return 1 if the buffer is valid UTF-8, 0 otherwise. If pStr is NULL, 0 is always returned.
 */
int ntk_is_utf8(const char* pStr, size_t len);

/**
 * @brief Create a sanitized copy of a UTF-8 string.
 * @note When a sequence of invalid code units are detected, a single U+FFFD is inserted. The next code point in the
 *       output is the next valid UTF-8 code point in the input.
 * @param pStr Buffer to sanitize.
 * @param len Length of the buffer.
 * @param pBufferLen Output: length of the sanitized string.
 * @return Sanitized copy of pStr. If pStr is NULL, NULL is returned.
 */
char* ntk_sanitize_utf8(const char* pStr, size_t len, size_t* pBufferLen);

#endif //NTK_LIBRARY_H
