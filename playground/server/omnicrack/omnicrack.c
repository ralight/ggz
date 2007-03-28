#include "omnicrack.h"

#ifdef WITH_ICU
#include <unicode/ustring.h>
#include <unicode/uchar.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef WITH_ICU
struct blockassignment_t
{
	const char *name;
	UBlockCode code;
	int enabled;
};
typedef struct blockassignment_t blockassignment;

static blockassignment unicode_blocks[99] =
{
	{"armenian", UBLOCK_ARMENIAN},
	{"coptic", UBLOCK_COPTIC},
	{"cyrillic", UBLOCK_CYRILLIC},
	{"cyrillic_supplement", UBLOCK_CYRILLIC_SUPPLEMENT},
	{"georgian", UBLOCK_GEORGIAN},
	{"georgian_supplement", UBLOCK_GEORGIAN_SUPPLEMENT},
	{"greek", UBLOCK_GREEK},
	{"greek_extended", UBLOCK_GREEK_EXTENDED},
	{"basic_latin", UBLOCK_BASIC_LATIN},
	{"latin_1_supplement", UBLOCK_LATIN_1_SUPPLEMENT},
	{"latin_extended_a", UBLOCK_LATIN_EXTENDED_A},
	{"latin_extended_b", UBLOCK_LATIN_EXTENDED_B},
	{"latin_extended_c", UBLOCK_LATIN_EXTENDED_C},
	{"latin_extended_d", UBLOCK_LATIN_EXTENDED_D},
	{"latin_extended_additional", UBLOCK_LATIN_EXTENDED_ADDITIONAL},
	{"ethiopic", UBLOCK_ETHIOPIC},
	{"ethiopic_extended", UBLOCK_ETHIOPIC_EXTENDED},
	{"ethiopic_supplement", UBLOCK_ETHIOPIC_SUPPLEMENT},
	{"tifinagh", UBLOCK_TIFINAGH},
	{"nko", UBLOCK_NKO},
	{"cherokee", UBLOCK_CHEROKEE},
	{"unified_canadian_aboriginal_syllabics", UBLOCK_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS},
	{"deseret", UBLOCK_DESERET},
	{"arabic", UBLOCK_ARABIC},
	{"arabic_supplement", UBLOCK_ARABIC_SUPPLEMENT},
	{"arabic_presentation_forms_a", UBLOCK_ARABIC_PRESENTATION_FORMS_A},
	{"arabic_presentation_forms_b", UBLOCK_ARABIC_PRESENTATION_FORMS_B},
	{"hebrew", UBLOCK_HEBREW},
	{"alphabetic_presentation_forms", UBLOCK_ALPHABETIC_PRESENTATION_FORMS},
	{"syriac", UBLOCK_SYRIAC},
	{"thaana", UBLOCK_THAANA},
	{"kharoshthi", UBLOCK_KHAROSHTHI},
	{"mongolian", UBLOCK_MONGOLIAN},
	{"phags_pa", UBLOCK_PHAGS_PA},
	{"tibetan", UBLOCK_TIBETAN},
	{"bengali", UBLOCK_BENGALI},
	{"devanagari", UBLOCK_DEVANAGARI},
	{"gujarati", UBLOCK_GUJARATI},
	{"gurmukhi", UBLOCK_GURMUKHI},
	{"kannada", UBLOCK_KANNADA},
	{"limbu", UBLOCK_LIMBU},
	{"malayalam", UBLOCK_MALAYALAM},
	{"oriya", UBLOCK_ORIYA},
	{"sinhala", UBLOCK_SINHALA},
	{"syloti_nagri", UBLOCK_SYLOTI_NAGRI},
	{"tamil", UBLOCK_TAMIL},
	{"telugu", UBLOCK_TELUGU},
	{"buhid", UBLOCK_BUHID},
	{"hanunoo", UBLOCK_HANUNOO},
	{"tagalog", UBLOCK_TAGALOG},
	{"tagbanwa", UBLOCK_TAGBANWA},
	{"balinese", UBLOCK_BALINESE},
	{"buginese", UBLOCK_BUGINESE},
	{"khmer", UBLOCK_KHMER},
	{"khmer_symbols", UBLOCK_KHMER_SYMBOLS},
	{"lao", UBLOCK_LAO},
	{"myanmar", UBLOCK_MYANMAR},
	{"thai", UBLOCK_THAI},
	{"new_tai_lue", UBLOCK_NEW_TAI_LUE},
	{"tai_le", UBLOCK_TAI_LE},
	{"cjk_unified_ideographs", UBLOCK_CJK_UNIFIED_IDEOGRAPHS},
	{"cjk_unified_ideographs_extension_a", UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A},
	{"cjk_unified_ideographs_extension_b", UBLOCK_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B},
	{"cjk_compatibility_ideographs", UBLOCK_CJK_COMPATIBILITY_IDEOGRAPHS},
	{"cjk_compatibility_ideographs_supplement", UBLOCK_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT},
	{"cjk_radicals_supplement", UBLOCK_CJK_RADICALS_SUPPLEMENT},
	{"kangxi_radicals", UBLOCK_KANGXI_RADICALS},
	{"cjk_strokes", UBLOCK_CJK_STROKES},
	{"ideographic_description_characters", UBLOCK_IDEOGRAPHIC_DESCRIPTION_CHARACTERS},
	{"bopomofo", UBLOCK_BOPOMOFO},
	{"bopomofo_extended", UBLOCK_BOPOMOFO_EXTENDED},
	{"hiragana", UBLOCK_HIRAGANA},
	{"katakana", UBLOCK_KATAKANA},
	{"katakana_phonetic_extensions", UBLOCK_KATAKANA_PHONETIC_EXTENSIONS},
	{"hangul_syllables", UBLOCK_HANGUL_SYLLABLES},
	{"hangul_jamo", UBLOCK_HANGUL_JAMO},
	{"hangul_compatibility_jamo", UBLOCK_HANGUL_COMPATIBILITY_JAMO},
	{"yi_syllables", UBLOCK_YI_SYLLABLES},
	{"yi_radicals", UBLOCK_YI_RADICALS},
	{"ancient_greek_numbers", UBLOCK_ANCIENT_GREEK_NUMBERS},
	{"ancient_greek_musical_notation", UBLOCK_ANCIENT_GREEK_MUSICAL_NOTATION},
	{"cuneiform", UBLOCK_CUNEIFORM},
	{"cuneiform_numbers_and_punctuation", UBLOCK_CUNEIFORM_NUMBERS_AND_PUNCTUATION},
	{"old_persian", UBLOCK_OLD_PERSIAN},
	{"ugaritic", UBLOCK_UGARITIC},
	{"linear_b_syllabary", UBLOCK_LINEAR_B_SYLLABARY},
	{"linear_b_ideograms", UBLOCK_LINEAR_B_IDEOGRAMS},
	{"aegean_numbers", UBLOCK_AEGEAN_NUMBERS},
	{"counting_rod_numerals", UBLOCK_COUNTING_ROD_NUMERALS},
	{"cypriot_syllabary", UBLOCK_CYPRIOT_SYLLABARY},
	{"gothic", UBLOCK_GOTHIC},
	{"old_italic", UBLOCK_OLD_ITALIC},
	{"ogham", UBLOCK_OGHAM},
	{"runic", UBLOCK_RUNIC},
	{"phoenician", UBLOCK_PHOENICIAN},
	{"shavian", UBLOCK_SHAVIAN},
	{"glagolitic", UBLOCK_GLAGOLITIC},
	{"osmanya", UBLOCK_OSMANYA},
	{NULL, UBLOCK_NO_BLOCK}
};

const char *block_name(UBlockCode code)
{
	int i;

	for(i = 0; unicode_blocks[i].name; i++)
	{
		if(unicode_blocks[i].code == code)
			return unicode_blocks[i].name;
	}

	return NULL;
}

char *block_type(UBlockCode code)
{
	UBool alphabetic = u_isUAlphabetic(code);
	UBool alpha = u_isalpha(code);
	UBool numeric = u_isdigit(code);
	UBool lowercase = u_isULowercase(code);
	UBool lower = u_islower(code);

	char *ret = (char*)malloc(10);
	snprintf(ret, 10, "%c%c%c%c%c",
		(alphabetic ? 'A' : '_'),
		(alpha ? 'a' : '_'),
		(numeric ? 'n' : '_'),
		(lowercase ? 'L' : '_'),
		(lower ? 'l' : '_'));

	return ret;
}

int variety_score(UChar *ustr, int length)
{
	int score = 0;
	int has_alpha = 0;
	int has_num = 0;
	int has_lower = 0;
	int has_upper = 0;
	int has_nonumalpha = 0;
	int has_noupperlower = 0;
	int i;
	UChar uc;

	for(i = 0; i < length; i++)
	{
		uc = ustr[i];

		UBool alphabetic = u_isUAlphabetic(uc);
		UBool numeric = u_isdigit(uc);
		UBool lowercase = u_isULowercase(uc);
		UBool uppercase = u_isUUppercase(uc);

		if(alphabetic) has_alpha = 1;
		if(numeric) has_num = 1;
		if(lowercase) has_lower = 1;
		if(uppercase) has_upper = 1;

		if((!alphabetic) && (!numeric)) has_nonumalpha = 1;
		if((!lowercase) && (!uppercase)) has_noupperlower = 1;
	}

	score += has_alpha;
	score += has_num;
	score += has_lower;
	score += has_upper;
	score += has_nonumalpha;
	score += has_noupperlower;

	return score;
}

int block_score(UChar *ustr, int length)
{
	int score = 0;
	int i, j;
	UChar uc;

	/* FIXME: uses global table instead of copy */

	for(i = 0; unicode_blocks[i].name; i++)
	{
		unicode_blocks[i].enabled = 0;
	}

	for(i = 0; i < length; i++)
	{
		uc = ustr[i];

		UBlockCode code = ublock_getCode(uc);

		for(j = 0; unicode_blocks[j].name; j++)
		{
			if(unicode_blocks[j].code == code)
			{
				unicode_blocks[j].enabled = 1;
				break;
			}
		}
	}

	for(i = 0; unicode_blocks[i].name; i++)
	{
		score += unicode_blocks[i].enabled;
	}

	return score;
}
#endif

int length_score(int length)
{
	if(length < 3) return 0;
	if(length < 5) return 1;
	if(length < 8) return 2;
	return 3;
}

int dict_score(const char *password)
{
	/* FIXME: we don't have a dictionary yet */
	return 0;
}

int passwd_score(const char *password)
{
	/* FIXME: we don't have a passwd lookup yet */
	return 0;
}

int omnicrack_checkstrength(const char *password, int strategy, const char *dictpath)
{
#ifdef WITH_ICU
	UChar *ustr = NULL;
#if DEBUG
	UChar uc;
	UChar ucx[2];
	char str[8];
	int i;
#endif
	int32_t length = 0;
	UErrorCode error = U_ZERO_ERROR;
	int vs, bs, ls, ds, ps;
	int score;
	int ret;

	/* FIXME: the following two lines document a sucky IBM API bug */
	ustr = (UChar*)malloc(strlen(password) * 4);
	length = strlen(password) * 4;

	ustr = u_strFromUTF8(ustr, length, &length, password, -1, &error);
	if(U_FAILURE(error))
	{
#if DEBUG
		fprintf(stderr, "Error: conversion failure\n");
#endif
		return OMNICRACK_RESULT_BADINPUT;
	}
	ustr = (UChar*)malloc(sizeof(UChar) * length);
	if(!ustr)
	{
#if DEBUG
		fprintf(stderr, "Error: malloc failure\n");
#endif
		return OMNICRACK_RESULT_BADINPUT;
	}
	ustr = u_strFromUTF8(ustr, length, NULL, password, -1, &error);

#if DEBUG
	for(i = 0; i < length; i++)
	{
		uc = ustr[i];

		ucx[0] = uc;
		ucx[1] = '\0';
		u_strToUTF8(str, sizeof(str), NULL, ucx, -1, &error);

		char *bt = block_type(uc);

		UBlockCode code = ublock_getCode(uc);
		printf("  [%2i: %s] %24s | %10s\n",
			i, str, block_name(code), bt);
		/* FIXME: using %2s uses 2 bytes, not 2 characters! */

		free(bt);
	}
#endif

	if(strategy & OMNICRACK_STRATEGY_VARIETY)
		vs = variety_score(ustr, length);
	else
		vs = 0;
	if(strategy & OMNICRACK_STRATEGY_BLOCKS)
		bs = block_score(ustr, length);
	else
		bs = 0;
	if(strategy & OMNICRACK_STRATEGY_LENGTH)
		ls = length_score(length);
	else
		ls = 0;
	if(strategy & OMNICRACK_STRATEGY_DICT)
		ds = dict_score(password);
	else
		ds = 0;
	if(strategy & OMNICRACK_STRATEGY_PASSWD)
		ps = passwd_score(password);
	else
		ps = 0;

	score = vs + bs + ls + ds + ps;

	ret = OMNICRACK_RESULT_OK;
	if(score < 5)
		ret = OMNICRACK_RESULT_BADSCORE;
	if(ls < 2)
		if(strategy & OMNICRACK_STRATEGY_LENGTH)
			ret = OMNICRACK_RESULT_BADLENGTH;
	/*if(ds == 0) ret = OMNICRACK_RESULT_BADDICT;*/

#if DEBUG
	printf("  => Variety score: %i\n", vs);
	printf("  => Character block score: %i\n", bs);
	printf("  => Length score: %i\n", ls);
	printf("  => Dict score: %i\n", ds);
	printf("  => Passwd score: %i\n", ps);
	printf("  => Overall score: %i\n", score);
#endif

	free(ustr);

	return ret;
#else /* WITH_ICU */
	return 0;
#endif
}

