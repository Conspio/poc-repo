#include <stdio.h>
#include <iostream>

#define PCRE2_CODE_UNIT_WIDTH 8
#include "pcre2.h"

using namespace std;

int main() {
    const char *pattern = "\x2d\x32\x35\x7c\x72\xf3\xb8\xb2\xa3\x2f"
                          "\x35\x77\x87\x31\xff\x2d\x2e\x31\x78\x26"
                          "\x2a\x2e\x7c\x31\x2a\x28\x28\x28\x47\x29"
                          "\x29\x4a\x0a\x76\x00\x00\x7c\x2e\x3f\x2e"
                          "\x7c\xa9\xa9\xa9\xa9\x29\x7b\x38\x32\x35"
                          "\x7d\x00";
    size_t size = 51;

    //pattern opt
    pcre2_compile_context *compile_context = pcre2_compile_context_create(NULL);
    if (!compile_context) {
        cout << "PCRE2 cannot allocate compile context" << endl;
        return -1;
    }

    pcre2_code *re;
    int err_code;
    PCRE2_SIZE err_offset;

    //the compiled pattern
    re = pcre2_compile(
        (PCRE2_SPTR8)pattern,           /* the pattern */
        size,                           /* length */
        0,                                  /* options */
        &err_code,                          /* for error code */
        &err_offset,                        /* for error offset */
        compile_context);                   /* character tables */
    if (!re) {
        cout << "PCRE2 compilation failed at offset " << (int)err_offset << ": [" << err_code << "]" << endl;
        pcre2_compile_context_free(compile_context);
        return -1;
    }
    pcre2_compile_context_free(compile_context);

    //matching opt
    pcre2_match_context *match_context = pcre2_match_context_create(NULL);
    if (!match_context) {
        cout << "PCRE cannot allocate match context" << endl;
        pcre2_code_free(re);
        return -1;
    }

    //match info
    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);
    if (!match_data) {
        printf("PCRE2 cannot allocate match data\n");
        pcre2_match_context_free(match_context);
        pcre2_code_free(re);
        return -1;
    }

    int count = 0;  
    int rc;
    PCRE2_SIZE start_offset = 0;
    while ((rc = pcre2_match(
                 re,                      /* the compiled pattern */
                 (PCRE2_SPTR8)pattern,    /* the subject string */
                 size,                    /* the length of the string */
                 start_offset,            /* start at offset 0 in the subject */
                 PCRE2_NO_JIT,            /* default options */
                 match_data,              /* match data */
                 match_context))          /* match context */
                 > 0)
    {
        count++;
        start_offset = pcre2_get_ovector_pointer(match_data)[1];
        cout << "start_offset: " << start_offset << endl;
        if (start_offset >= (PCRE2_SIZE)size || rc == 0) break;
        if (count > size) {
            cout << "pattern error! endless matching" << endl;
            break;
        }
    }

    cout << "count: " << count << endl;

    pcre2_match_context_free(match_context);
    pcre2_match_data_free(match_data);
    pcre2_code_free(re);

    return 0;
}
