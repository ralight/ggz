#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <ggz.h>
#ifdef WITH_ICU
#include <unicode/ustring.h>
#include <unicode/uchar.h>
#include <unicode/usprep.h>
#endif
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "unicode.h"

#include "err_func.h"

#ifdef WITH_ICU
struct blockassignment_t
{
	char *name;
	UBlockCode code;
	bool enabled;
};
typedef struct blockassignment_t blockassignment;

static bool exclude_forbidden_chars = 1;

static UChar forbiddenlist[] = {0x20, 0x25, 0x26, 0x5c, 0};

static bool policy_initialized = false;
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

static bool block_enabled(UBlockCode code)
{
	int i;

	for(i = 0; unicode_blocks[i].name; i++)
	{
		if(unicode_blocks[i].code == code)
			return unicode_blocks[i].enabled;
	}

	return false;
}
#endif

#ifdef WITH_ICU
bool contains(UChar c, UChar* list)
{
	int i;
	for(i = 0; list[i]; i++)
		if(list[i] == c)
			return true;
	return false;
}
#endif

bool username_allowed(const char *str)
{
#ifdef WITH_ICU
	UChar *ustr = NULL;
	UChar uc;
	int32_t length = 0;
	UErrorCode error = U_ZERO_ERROR;
	int i;
	bool allowed;

	u_strFromUTF8(NULL, 0, &length, str, -1, &error);
	if(U_FAILURE(error)) {
		if(error != U_BUFFER_OVERFLOW_ERROR) {
			ggz_error_msg("Error: utf->unicode conversion failure");
			return false;
		}
	}

	ustr = (UChar*)malloc(sizeof(UChar) * length);
	if(!ustr) {
		ggz_error_msg("Error: utf->unicode malloc failure");
		return false;
	}

	error = U_ZERO_ERROR;
	u_strFromUTF8(ustr, length, NULL, str, -1, &error);
	if(U_FAILURE(error)) {
		ggz_error_msg("Error: utf->unicode conversion failure");
		free(ustr);
		return NULL;
	}

	/* iterate over the characters */
	allowed = true;

	for(i = 0; i < length; i++)
	{
		uc = ustr[i];

		UBlockCode code = ublock_getCode(uc);

		if((policy_initialized) && (!block_enabled(code)))
		{
			/* prohibited, return after free() */
			/* no policy in use allows all blocks */
			allowed = false;
		}

		if(exclude_forbidden_chars)
		{
			/* FIXME: use unicode classes for more general checks */
			if(contains(uc, forbiddenlist))
				allowed = false;
		}

		if(u_isISOControl(uc))
		{
			/* those are always forbidden */
			allowed = false;
		}
	}

	free(ustr);

	if(!allowed)
	{
		return false;
	}

	/* name is alright */
	return true;
#else
	const char *p;

	/* For compatibility, we'll have to assume ASCII */
	/* Nothing less than a space and no extended ASCII */
	/* & - can mess with M$ Windows labels, etc */
	/* % - can screw up log and debug's printf()s */
	/* \ - can screw up log and debug's printf()s */
	for(p = str; *p != '\0'; p++)
	{
		if((*p < 33) || (*p > 126)
		|| (*p == '%') || (*p == '&') || (*p == '\\'))
		{
			return false;
		}
	}

	return true;
#endif
}

bool init_unicode(const char *policyfile)
{
#ifdef WITH_ICU
	int rc, enabled;
	int i;

	rc = ggz_conf_parse(policyfile, GGZ_CONF_RDONLY);
	if(rc == -1)
	{
		ggz_error_msg("Error: unable to open policy file.");
		return false;
	}

	for(i = 0; unicode_blocks[i].name; i++)
	{
		enabled = ggz_conf_read_int(rc, "UsernameCharacterSets",
			unicode_blocks[i].name, 0);
		unicode_blocks[i].enabled = enabled;
	}

	exclude_forbidden_chars = ggz_conf_read_int(rc, "UsernameExclusions",
			"forbidden_chars", 1);

	ggz_conf_close(rc);
	policy_initialized = true;

	return true;
#else
	return false;
#endif
}

char *username_canonical(const char *username)
{
#ifdef WITH_ICU
	char *canonical;
	UStringPrepProfile *profile;
	UErrorCode status;
	uint32_t numchars;
	UChar *ustr = NULL;
	UChar *ustr2;
	int32_t length = 0;
	int32_t length2;
	UErrorCode error = U_ZERO_ERROR;

	status = U_ZERO_ERROR;
	/* FIXME: we need GGZ-specific or SASL datafile here */
	/* FIXME: IBM sucks again, "uidna" isn't documented anywhere */
	profile = usprep_open(NULL, "uidna", &status);
	if(U_FAILURE(status))
	{
		ggz_error_msg("Error: unable to open stringprep profile.");
		return NULL;
	}

	u_strFromUTF8(NULL, 0, &length, username, -1, &error);
	if(U_FAILURE(error)) {
		if(error != U_BUFFER_OVERFLOW_ERROR) {
			ggz_error_msg("Error: utf->unicode pre-flight failure");
			usprep_close(profile);
			return NULL;
		}
	}

	ustr = (UChar*)malloc(sizeof(UChar) * length);
	if(!ustr) {
		ggz_error_msg("Error: utf->unicode malloc failure");
		usprep_close(profile);
		return NULL;
	}

	error = U_ZERO_ERROR;
	ustr = u_strFromUTF8(ustr, length, NULL, username, -1, &error);
	if(U_FAILURE(error)) {
		ggz_error_msg("Error: utf->unicode conversion failure");
		free(ustr);
		usprep_close(profile);
		return NULL;
	}

	/* FIXME: of course there's no documentation of how to estimate... */
	length2 = length * 4;
	ustr2 = (UChar*)malloc(sizeof(UChar) * length2);

	numchars = usprep_prepare(profile, ustr, length, ustr2, length2, USPREP_DEFAULT, NULL, &error);
	free(ustr);
	if(U_FAILURE(error)) {
		ggz_error_msg("Error: stringprep failure");
		free(ustr2);
		usprep_close(profile);
		return NULL;
	}

	error = U_ZERO_ERROR;
	u_strToUTF8(NULL, 0, &length, ustr2, numchars, &error);
	if(U_FAILURE(error)) {
		if(error != U_BUFFER_OVERFLOW_ERROR) {
			ggz_error_msg("Error: unicode->utf pre-flight failure");
			free(ustr2);
			usprep_close(profile);
			return NULL;
		}
	}

	canonical = malloc(length);
	if(!canonical) {
		ggz_error_msg("Error: unicode->utf malloc failure");
		free(ustr2);
		usprep_close(profile);
		return NULL;
	}

	error = U_ZERO_ERROR;
	u_strToUTF8(canonical, length, NULL, ustr2, numchars, &error);
	if(U_FAILURE(error)) {
		ggz_error_msg("Error: unicode->utf conversion failure");
		free(canonical);
		free(ustr2);
		usprep_close(profile);
		return NULL;
	}

	free(ustr2);
	usprep_close(profile);

	return canonical;
#else
	return ggz_strdup(username);
#endif
}

#if 0
char *stringcut(const char *string, int len)
{
#ifdef WITH_ICU
	char *cutstring;
	UChar *ustr = NULL;
	int32_t length = 0;
	int32_t blength = 0;
	UErrorCode error = U_ZERO_ERROR;

	if(len < 0)
		return NULL;

	ustr = u_strFromUTF8(NULL, 0, &length, string, -1, &error);
	if(U_FAILURE(error)) {
		if(error != U_BUFFER_OVERFLOW_ERROR) {
			ggz_error_msg("Error: utf->unicode pre-flight failure");
			return NULL;
		}
	}

	ustr = (UChar*)malloc(sizeof(UChar) * length);
	if(!ustr) {
		ggz_error_msg("Error: malloc failure in utf->unicode transformation");
		return NULL;
	}

	error = U_ZERO_ERROR;
	u_strFromUTF8(ustr, length, NULL, string, -1, &error);
	if(U_FAILURE(error)) {
		ggz_error_msg("Error: utf->unicode conversion failure");
		free(ustr);
		return NULL;
	}

	while(1) {
		error = U_ZERO_ERROR;
		u_strToUTF8(NULL, 0, &blength, ustr, length, &error);
		if(U_FAILURE(error)) {
			if(error != U_BUFFER_OVERFLOW_ERROR) {
				ggz_error_msg("Error: unicode->utf back-pre-flight failure");
				return NULL;
			}
		}
		if(blength < len)
			break;
		length--;
	}

	cutstring = ggz_malloc(blength + 1);
	if(!cutstring) {
		ggz_error_msg("Error: malloc failure in unicode->utf transformation");
		return NULL;
	}

	error = U_ZERO_ERROR;
	u_strToUTF8(cutstring, blength, NULL, ustr, length, &error);
	if(U_FAILURE(error)) {
		ggz_error_msg("Error: unicode->utf conversion failure");
		ggz_free(cutstring);
		return NULL;
	}

	free(ustr);

	return cutstring;
#else
	return NULL;
#endif
}
#endif
