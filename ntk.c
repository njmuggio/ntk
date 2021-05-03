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
  surrogate_pair = 0x20
};

static enum states_is_utf8 advance(char, enum states_is_utf8);

int ntk_is_utf8(const char* pStr, size_t len)
{
  if (pStr == NULL)
  {
    return 0;
  }

  int ret = 1;
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

static enum states_is_utf8 advance(const char c, const enum states_is_utf8 state)
{
  switch (state)
  {
    case invalid:
      return invalid;
    case start:
      if ((c & hi1) == none)
      {
        // Single byte (US-ASCII) - anything can come next
        return start;
      }
      else if ((c & hi3) == hi2)
      {
        // 0xC0 and 0xC1 can only produce overlong sequences
        if (c == (char)0xC0 || c == (char)0xC1)
        {
          return invalid;
        }

        // Started a two-byte sequence, expect one more continuation byte
        return continue_1;
      }
      else if ((c & hi4) == hi3)
      {
        // 0xE0 can start an overlong sequence
        if (c == (char)0xE0)
        {
          return overlong_3_check;
        }

        // 0xED can start encoding U+D800 - U+DFFF, which are reserved for UTF-16 surrogate pairs
        if (c == (char)0xED)
        {
          return surrogate_pair_check;
        }

        // Started a three-byte sequence, expect two more continuation bytes
        return continue_2;
      }
      else if ((c & hi5) == hi4)
      {
        // 0xF0 can start an overlong sequence
        if (c == (char)0xF0)
        {
          return overlong_4_check;
        }

        // 0xF4 can overflow
        if (c == (char)0xF4)
        {
          return max_check;
        }

        // 0xF5+ will overflow
        if (c >= (char)0xF5)
        {
          return invalid;
        }

        // Started a four-byte sequence, expect three more continuation bytes
        return continue_3;
      }
      return invalid;
    case continue_1:
      if ((c & hi2) == hi1)
      {
        return start;
      }
      return invalid;
    case continue_2:
      if ((c & hi2) == hi1)
      {
        return continue_1;
      }
      return invalid;
    case continue_3:
      if ((c & hi2) == hi1)
      {
        return continue_2;
      }
      return invalid;
    case overlong_3_check:
      if ((c & overlong_3) != 0)
      {
        return continue_1;
      }
      return invalid;
    case overlong_4_check:
      if ((c & overlong_4) != 0)
      {
        return continue_2;
      }
      return invalid;
    case max_check:
      if (c > (char)max)
      {
        return invalid;
      }
      return continue_2;
    case surrogate_pair_check:
      if ((c & surrogate_pair) != 0)
      {
        return invalid;
      }
      return continue_1;
    default:
      return invalid;
  }
}
