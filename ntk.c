#include <stdlib.h>
#include <string.h>

#include "ntk.h"

enum states_is_utf8
{
  invalid,
  start,
  continue_1,
  continue_2,
  continue_3,
  overlong_3_check,
  overlong_4_check,
  max_check,
  surrogate_pair_check,
};

enum masks
{
  hi5 = 0xF8,
  hi4 = 0xF0,
  hi3 = 0xE0,
  hi2 = 0xC0,
  hi1 = 0x80,
  none = 0x00,
  overlong_3 = 0x20,
  overlong_4 = 0x30,
  max = 0x8F,
  surrogate_pair = 0x20,
  hi2_overlong1 = 0xC0,
  hi2_overlong2 = 0xC1,
  hi3_overlong1 = 0xE0,
  hi3_surrogate_start = 0xED,
  hi4_overlong1 = 0xF0,
  hi4_overflow_risk = 0xF4,
  hi4_overflow = 0xF5,
};

static enum states_is_utf8 advance(unsigned char c, enum states_is_utf8 state);

int ntk_is_utf8(const char* pStr, size_t len)
{
  if (pStr == NULL)
  {
    return 0;
  }

  enum states_is_utf8 state = start;

  for (size_t i = 0; i < len; ++i)
  {
    state = advance(pStr[i], state);
    if (state == invalid)
    {
      break;
    }
  }

  return state == start;
}

char* ntk_sanitize_utf8(const char* pStr, size_t len, size_t* pBufferLen)
{
  if (pStr == NULL || len == 0)
  {
    *pBufferLen = 0;
    return NULL;
  }

  char* pRet = malloc(len);
  *pBufferLen = len;

  enum states_is_utf8 state = start;
  int invalidFlag = 0;
  size_t validStart = 0;
  size_t sanitizedLen = 0;

  for (size_t i = 0; i < len; ++i)
  {
    state = advance(pStr[i], state);
    if (state == invalid && !invalidFlag)
    {
      invalidFlag = 1;

      // Copy last-seen good data
      if (*pBufferLen - sanitizedLen < i - validStart + 3) // +3 for U+FFFD
      {
        *pBufferLen = sanitizedLen + i - validStart + 3;
        char* pNewBuf = realloc(pRet, *pBufferLen);
        if (pNewBuf != NULL)
        {
          pRet = pNewBuf;
        }
        else // realloc failed... give up and return a NULL buffer
        {
          free(pRet);
          pRet = NULL;
          *pBufferLen = 0;
          return NULL;
        }
      }

      memcpy(pRet + sanitizedLen, pStr + validStart, i - validStart);
      sanitizedLen += i - validStart;

      // Insert U+FFFD
      pRet[sanitizedLen++] = '\xEF';
      pRet[sanitizedLen++] = '\xBF';
      pRet[sanitizedLen++] = '\xBD';
    }
    else if (state != invalid && invalidFlag)
    {
      invalidFlag = 0;
      validStart = i;
    }
  }

  if (!invalidFlag && validStart < len)
  {
    if (*pBufferLen - sanitizedLen < len - validStart)
    {
      *pBufferLen = sanitizedLen + len - validStart;
      pRet = realloc(pRet, *pBufferLen);
    }

    memcpy(pRet + sanitizedLen, pStr + validStart, len - validStart);
    sanitizedLen += len - validStart;
  }

  *pBufferLen = sanitizedLen;
  return pRet;
}

static enum states_is_utf8 advance_start(const unsigned char c)
{
  enum states_is_utf8 ret = invalid;

  if ((c & (unsigned)hi1) == none)
  {
    // Single byte (US-ASCII) - anything can come next
    ret = start;
  }
  else if ((c & (unsigned)hi3) == hi2)
  {
    // 0xC0 and 0xC1 can only produce overlong sequences
    if (c == (unsigned)hi2_overlong1 || c == (unsigned char)hi2_overlong2)
    {
      ret = invalid;
    }
    else // Started a two-byte sequence, expect one more continuation byte
    {
      ret = continue_1;
    }
  }
  else if ((c & (unsigned)hi4) == hi3)
  {
    // 0xE0 can start an overlong sequence
    if (c == (unsigned char)hi3_overlong1)
    {
      ret = overlong_3_check;
    }
    else if (c == (unsigned char)hi3_surrogate_start) // 0xED can start encoding U+D800 - U+DFFF, which are reserved for UTF-16 surrogate pairs
    {
      ret = surrogate_pair_check;
    }
    else // Started a three-byte sequence, expect two more continuation bytes
    {
      ret = continue_2;
    }
  }
  else if ((c & (unsigned)hi5) == hi4)
  {
    // 0xF0 can start an overlong sequence
    if (c == (unsigned char)hi4_overlong1)
    {
      ret = overlong_4_check;
    }
    else if (c == (unsigned char)hi4_overflow_risk) // 0xF4 can overflow
    {
      ret = max_check;
    }
    else if (c >= (unsigned char)hi4_overflow) // 0xF5+ will overflow
    {
      ret = invalid;
    }
    else // Started a four-byte sequence, expect three more continuation bytes
    {
      ret = continue_3;
    }
  }

  return ret;
}

static enum states_is_utf8 advance(const unsigned char c, const enum states_is_utf8 state)
{
  switch (state)
  {
    case invalid:
    case start:
      return advance_start(c);
    case continue_1:
      if ((c & (unsigned)hi2) == hi1)
      {
        return start;
      }
      return invalid;
    case continue_2:
      if ((c & (unsigned)hi2) == hi1)
      {
        return continue_1;
      }
      return invalid;
    case continue_3:
      if ((c & (unsigned)hi2) == hi1)
      {
        return continue_2;
      }
      return invalid;
    case overlong_3_check:
      if ((c & (unsigned)overlong_3) != 0)
      {
        return continue_1;
      }
      return invalid;
    case overlong_4_check:
      if ((c & (unsigned)overlong_4) != 0)
      {
        return continue_2;
      }
      return invalid;
    case max_check:
      if (c > (unsigned char)max)
      {
        return invalid;
      }
      return continue_2;
    case surrogate_pair_check:
      if ((c & (unsigned)surrogate_pair) != 0)
      {
        return invalid;
      }
      return continue_1;
    default:
      return invalid;
  }
}
