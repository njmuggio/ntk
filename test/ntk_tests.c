#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "ntk.h"
#include "test_articles.h"
#include "unity.h"

void setUp(void)
{
  // Empty
}

void tearDown(void)
{
  // Empty
}

void test_EmptyString(void)
{
  TEST_ASSERT_TRUE(ntk_is_utf8("", 0));
}

void test_NullString(void)
{
  TEST_ASSERT_FALSE(ntk_is_utf8(0, 0));
}

void test_SimpleAscii(void)
{
  const char* pStr = "ntk";
  TEST_ASSERT_TRUE(ntk_is_utf8(pStr, 3));
}

void test_NullTerminated(void)
{
  const char* pStr = "ntk";
  TEST_ASSERT_TRUE(ntk_is_utf8(pStr, 4));
}

void test_EmbeddedNulls(void)
{
  const char* pStr = "n\0t\0k";
  TEST_ASSERT_TRUE(ntk_is_utf8(pStr, 6));
}

void test_OneByteBoundaries(void)
{
  const char* pLow = "\x00";
  TEST_ASSERT_TRUE(ntk_is_utf8(pLow, 1));

  const char* pHi = "\x7F";
  TEST_ASSERT_TRUE(ntk_is_utf8(pHi, 1));
}

void test_TwoByteBoundaries(void)
{
  const char* pBadLow1 = "\xC0\x80";
  TEST_ASSERT_FALSE(ntk_is_utf8(pBadLow1, 2));

  const char* pBadLow2 = "\xC1\xBF";
  TEST_ASSERT_FALSE(ntk_is_utf8(pBadLow2, 2));

  // Lower starts with 0xC2. 0xC0 and 0xC1 always produce overlong sequences
  const char* pLow = "\xC2\x80";
  TEST_ASSERT_TRUE(ntk_is_utf8(pLow, 2));

  const char* pHi = "\xDF\xBF";
  TEST_ASSERT_TRUE(ntk_is_utf8(pHi, 2));
}

void test_ThreeByteBoundaries(void)
{
  const char* pBadLow1 = "\xE0\x80\x80";
  TEST_ASSERT_FALSE(ntk_is_utf8(pBadLow1, 3));

  const char* pBadLow2 = "\xE0\x9F\xBF";
  TEST_ASSERT_FALSE(ntk_is_utf8(pBadLow2, 3));

  // Lower starts with 0xE0A0. Anything below this is an overlong sequence
  const char* pLow = "\xE0\xA0\x80";
  TEST_ASSERT_TRUE(ntk_is_utf8(pLow, 3));

  const char* pHi = "\xEF\xBF\xBF";
  TEST_ASSERT_TRUE(ntk_is_utf8(pHi, 3));
}

void test_FourByteBoundaries(void)
{
  const char* pBadLow1 = "\xF0\x80\x80\x80";
  TEST_ASSERT_FALSE(ntk_is_utf8(pBadLow1, 4));

  const char* pBadLow2 = "\xF0\x8F\xBF\xBF";
  TEST_ASSERT_FALSE(ntk_is_utf8(pBadLow2, 4));

  // Lower starts with 0xF090. Anything below this is an overlong sequence
  const char* pLow = "\xF0\x90\x80\x80";
  TEST_ASSERT_TRUE(ntk_is_utf8(pLow, 4));

  const char* pHi = "\xF4\x8F\xBF\xBF";
  TEST_ASSERT_TRUE(ntk_is_utf8(pHi, 4));

  const char* pBadHi1 = "\xF4\x90\x80\x80";
  TEST_ASSERT_FALSE(ntk_is_utf8(pBadHi1, 4));

  const char* pBadHi2 = "\xF7\xBF\xBF\xBF";
  TEST_ASSERT_FALSE(ntk_is_utf8(pBadHi2, 4));
}

void test_UnexpectedContinuation(void)
{
  for (char c = (char)'\x80'; c < (char)'\xC0'; ++c)
  {
    TEST_ASSERT_FALSE(ntk_is_utf8(&c, 1));
  }
}

void test_BrokenContinuation(void)
{
  const char* pTrunc2 = "\xC2";
  TEST_ASSERT_FALSE(ntk_is_utf8(pTrunc2, 1));

  const char* pInterrupt2 = "\xC2 ";
  TEST_ASSERT_FALSE(ntk_is_utf8(pInterrupt2, 2));

  const char* pTrunc3 = "\xE2\x82";
  TEST_ASSERT_FALSE(ntk_is_utf8(pTrunc3, 1));
  TEST_ASSERT_FALSE(ntk_is_utf8(pTrunc3, 2));

  const char* pInterrupt3A = "\xE2 ";
  TEST_ASSERT_FALSE(ntk_is_utf8(pInterrupt3A, 2));

  const char* pInterrupt3B = "\xE2\x82 ";
  TEST_ASSERT_FALSE(ntk_is_utf8(pInterrupt3B, 3));

  const char* pTrunc4 = "\xF2\x82\x82";
  TEST_ASSERT_FALSE(ntk_is_utf8(pTrunc4, 1));
  TEST_ASSERT_FALSE(ntk_is_utf8(pTrunc4, 2));
  TEST_ASSERT_FALSE(ntk_is_utf8(pTrunc4, 3));

  const char* pInterrupt4A = "\xF2 ";
  TEST_ASSERT_FALSE(ntk_is_utf8(pInterrupt4A, 2));

  const char* pInterrupt4B = "\xF2\x82 ";
  TEST_ASSERT_FALSE(ntk_is_utf8(pInterrupt4B, 3));

  const char* pInterrupt4C = "\xF2\x82\x82 ";
  TEST_ASSERT_FALSE(ntk_is_utf8(pInterrupt4C, 4));
}

void test_OverlongSequences(void)
{
  const char* pSeq1 = "\xC0\x80";
  TEST_ASSERT_FALSE(ntk_is_utf8(pSeq1, 2));

  const char* pSeq2 = "\xC0\xBF";
  TEST_ASSERT_FALSE(ntk_is_utf8(pSeq2, 2));

  const char* pSeq3 = "\xC1\x80";
  TEST_ASSERT_FALSE(ntk_is_utf8(pSeq3, 2));

  const char* pSeq4 = "\xC1\xBF";
  TEST_ASSERT_FALSE(ntk_is_utf8(pSeq4, 2));

  const char* pSeq5 = "\xE0\x80\x80";
  TEST_ASSERT_FALSE(ntk_is_utf8(pSeq5, 3));

  const char* pSeq6 = "\xE0\x9F\xBF";
  TEST_ASSERT_FALSE(ntk_is_utf8(pSeq6, 3));

  const char* pSeq7 = "\xF0\x80\x80\x80";
  TEST_ASSERT_FALSE(ntk_is_utf8(pSeq7, 4));

  const char* pSeq8 = "\xF0\x8F\xBF\xBF";
  TEST_ASSERT_FALSE(ntk_is_utf8(pSeq8, 4));
}

void test_SurrogatePairs(void)
{
  const char* pSeq1 = "\xED\xA0\x80";
  TEST_ASSERT_FALSE(ntk_is_utf8(pSeq1, 3));

  const char* pSeq2 = "\xED\xBF\xBF";
  TEST_ASSERT_FALSE(ntk_is_utf8(pSeq2, 3));
}

void test_InvalidStartBytes(void)
{
  char fe = (char)0xFE;
  TEST_ASSERT_FALSE(ntk_is_utf8(&fe, 1));

  char ff = (char)0xFF;
  TEST_ASSERT_FALSE(ntk_is_utf8(&ff, 1));
}

void test_HannoverHtml(void)
{
  TEST_ASSERT_TRUE(ntk_is_utf8((const char*)uni_hannover_html, uni_hannover_html_len));
}

void test_SanitizeInvalid(void)
{
  const char* pIn1 = "Scrunch-faced \xF8 fear baboon";
  size_t inLen1 = strlen(pIn1);
  const char* pExp1 = "Scrunch-faced \xEF\xBF\xBD fear baboon";
  size_t expLen1 = strlen(pExp1);
  size_t actualLen1;
  char* pActual1 = ntk_sanitize_utf8(pIn1, inLen1, &actualLen1);
  TEST_ASSERT_EQUAL_CHAR_ARRAY(pExp1, pActual1, expLen1);
  TEST_ASSERT_EQUAL_size_t(expLen1, actualLen1);
  free(pActual1);

  const char* pIn2 = "Scrunch-faced \xF8\x80\x80\x80\xF9\x80\x80\x8F fear baboon";
  size_t inLen2 = strlen(pIn2);
  const char* pExp2 = "Scrunch-faced \xEF\xBF\xBD fear baboon";
  size_t expLen2 = strlen(pExp2);
  size_t actualLen2;
  char* pActual2 = ntk_sanitize_utf8(pIn2, inLen2, &actualLen2);
  TEST_ASSERT_EQUAL_CHAR_ARRAY(pExp2, pActual2, expLen2);
  TEST_ASSERT_EQUAL_size_t(expLen2, actualLen2);
  free(pActual2);

  const char* pIn3 = "\xF8\x80\x80";
  size_t inLen3 = strlen(pIn3);
  const char* pExp3 = "\xEF\xBF\xBD";
  size_t expLen3 = strlen(pExp3);
  size_t actualLen3;
  char* pActual3 = ntk_sanitize_utf8(pIn3, inLen3, &actualLen3);
  TEST_ASSERT_EQUAL_CHAR_ARRAY(pExp3, pActual3, expLen3);
  TEST_ASSERT_EQUAL_size_t(expLen3, actualLen3);
  free(pActual3);

  const char* pIn4 = "\xF8\x80 fear baboon";
  size_t inLen4 = strlen(pIn4);
  const char* pExp4 = "\xEF\xBF\xBD fear baboon";
  size_t expLen4 = strlen(pExp4);
  size_t actualLen4;
  char* pActual4 = ntk_sanitize_utf8(pIn4, inLen4, &actualLen4);
  TEST_ASSERT_EQUAL_CHAR_ARRAY(pExp4, pActual4, expLen4);
  TEST_ASSERT_EQUAL_size_t(expLen4, actualLen4);
  free(pActual4);

  const char* pIn5 = "Scrunch-faced \x80";
  size_t inLen5 = strlen(pIn5);
  const char* pExp5 = "Scrunch-faced \xEF\xBF\xBD";
  size_t expLen5 = strlen(pExp5);
  size_t actualLen5;
  char* pActual5 = ntk_sanitize_utf8(pIn5, inLen5, &actualLen5);
  TEST_ASSERT_EQUAL_CHAR_ARRAY(pExp5, pActual5, expLen5);
  TEST_ASSERT_EQUAL_size_t(expLen5, actualLen5);
  free(pActual5);
}

void test_SanitizeValid(void)
{
  size_t len;
  char* pActual = ntk_sanitize_utf8((const char*)uni_hannover_html, uni_hannover_html_len, &len);
  TEST_ASSERT_EQUAL_CHAR_ARRAY(uni_hannover_html, pActual, uni_hannover_html_len);
  TEST_ASSERT_EQUAL_size_t(uni_hannover_html_len, len);
}

void test_SanitizeFuzz(void)
{
  srand(time(NULL));
  size_t origLen = 536870912;
  char* pBuf = malloc(origLen);
  if (pBuf != NULL)
  {
    for (size_t i = 0; i < origLen / sizeof(int); i++)
    {
      int val = rand();
      memcpy(pBuf + i * sizeof(int), &val, sizeof(int));
    }

    size_t len;
    char* pActual = ntk_sanitize_utf8(pBuf, origLen, &len);
    TEST_ASSERT_NOT_NULL(pActual);

    free(pBuf);
  }
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_EmptyString);
  RUN_TEST(test_NullString);
  RUN_TEST(test_SimpleAscii);
  RUN_TEST(test_NullTerminated);
  RUN_TEST(test_EmbeddedNulls);
  RUN_TEST(test_OneByteBoundaries);
  RUN_TEST(test_TwoByteBoundaries);
  RUN_TEST(test_ThreeByteBoundaries);
  RUN_TEST(test_FourByteBoundaries);
  RUN_TEST(test_UnexpectedContinuation);
  RUN_TEST(test_BrokenContinuation);
  RUN_TEST(test_OverlongSequences);
  RUN_TEST(test_SurrogatePairs);
  RUN_TEST(test_InvalidStartBytes);
  RUN_TEST(test_HannoverHtml);
  RUN_TEST(test_SanitizeInvalid);
  RUN_TEST(test_SanitizeValid);

  RUN_TEST(test_SanitizeFuzz);
  return UNITY_END();
}
