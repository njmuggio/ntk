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

void test_WikipediaEmoji(void)
{
  TEST_ASSERT_TRUE(ntk_is_utf8((const char*)wikipedia_emoji_html, wikipedia_emoji_html_len));
  TEST_ASSERT_TRUE(ntk_is_utf8((const char*)wikipedia_till_lindemann_html, wikipedia_till_lindemann_html_len));
  TEST_ASSERT_TRUE(ntk_is_utf8((const char*)wikipedia_tudor_dynasty_html, wikipedia_tudor_dynasty_html_len));
  TEST_ASSERT_TRUE(ntk_is_utf8((const char*)uni_hannover_html, uni_hannover_html_len));
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
  RUN_TEST(test_WikipediaEmoji);
  return UNITY_END();
}
