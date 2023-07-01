#define USING_STRINGVEC

#include "str.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

String string_alloc(Arena *arena, usize size) {
    String str;

    str.len = 0;

    str.data = arena_alloc(arena, size);
    str.data[size] = '\0';

    return str;
}

String string_from(Arena *arena, const char *content) {
    String str = string_alloc(arena, strlen(content));

    memcpy(str.data, content, strlen(content));

    str.len = strlen(content);

    return str;
}

String string_from_stdin(Arena *arena) {
    Arena scratch;

    arena_init(&scratch);

    String input = string_alloc(&scratch, STRING_DEFAULT_SIZE);
    
    char stdin_chr = 0;
    usize cap = STRING_DEFAULT_SIZE;

    while (stdin_chr != '\n') {
        stdin_chr = getchar();
        
        input.data[input.len++] = stdin_chr;

        if (input.len >= cap) {
            cap *= 2;
            arena_resize(&scratch, input.data, cap * sizeof(char)); 
        }
    }

    /* getting rid of the new line character and putting instead of him null terminator */
    input.data[input.len -= 1] = '\0';

    /* to avoid internal fragmantation, we put the string into global arena with the necessary size */
    String result = string_alloc(arena, strlen(input.data));

    result.len = input.len;

    memmove(result.data, input.data, result.len);

    arena_deinit(&scratch);

    return result;
}

void string_print(const String str) {
    fwrite(str.data, sizeof(char), str.len, stdout);
}

void string_log(const String str) {
     for (usize i = 0; i < str.len; ++i) {
        fprintf(stdout, " '%c' ", str.data[i]);
    }
}

void stringarr_print(const StringArr list) {
    for (usize i = 0; i < list.len; ++i) {
        string_print(list.arr[i]);
    }
}

void stringarr_log(StringArr list) {
    for (usize i = 0; i < list.len; ++i) {
        printf("[");
        string_print(list.arr[i]);
        printf("]");
    }
}

StringArr string_parse(Arena *arena, String *str, const char *delimiters) {
    Arena scratch;

    arena_init(&scratch);

    usize max_num_of_strings = 16;

    StringArr list = {
        .arr = (String *)arena_alloc(&scratch, sizeof(String) * max_num_of_strings),
        .len = 0,
        .cap = max_num_of_strings
    };

    usize offset = 0;
    usize current_str_len = 0;

    while (offset < str->len) {
        if (!strchr(delimiters, str->data[offset])) {
            current_str_len++;
            offset++;
            continue;
        }

        /* in case there were two delimiters next to each other */
        if (current_str_len <= 0) {
            offset++;
            continue;
        }

        /* we assign the strings data with the data of given strign and increment it by the offset - current strings length */
        list.arr[list.len].data = str->data + offset - current_str_len;
        list.arr[list.len].len = current_str_len;

        list.len++;

        current_str_len = 0;

        /* in case we need more memory */
        if (list.len >= list.cap) {
            list.cap *= 2;
            arena_resize(&scratch, list.arr, sizeof(String) * list.cap);
        }

        offset++;
    }

    /* if the last string isnt delimiter, we also assign it */
    if (current_str_len > 0) {
        list.arr[list.len].data = str->data + offset - current_str_len;
        list.arr[list.len].len = current_str_len;

        list.len++;   
    }

    /* creating new list in the given area and copying data into it
       we do this because we dont know, in advance, how many strings there will be 
       and resizing could leave us with huge amount of internal fragmantation */
    StringArr alist = {
        .arr = (String *)arena_alloc(arena, list.len * sizeof(String)),
        .len = list.len,
        .cap = list.len
    };

    memcpy(alist.arr, list.arr, sizeof(String) * list.len);

    arena_deinit(&scratch);

    return alist;
}

void stringarr_push_multiple(StringArr *arr, u64 number_of_strings, ...) {
    va_list args;

    va_start(args, number_of_strings);

    usize old_len = arr->len;

    for (; arr->len < arr->cap && arr->len < number_of_strings + old_len; ++arr->len) {
        char *arg = va_arg(args, char*); 
        arr->arr[arr->len].data = strdup(arg);
        arr->arr[arr->len].len = strlen(arg);
    }

    va_end(args);
}

bool stringarr_push(StringArr *arr, const String str) {
    if (arr->len >= arr->cap) {
        return false;
    }

    arr->arr[arr->len++] = str;

    return true;
}

StringArr stringarr_alloc(Arena *arena, usize size) {
    return (StringArr) {
        .arr = arena_alloc(arena, size * sizeof(String)),
        .len = 0,
        .cap = size
    };
}

bool string_equal(const String str1, const String str2) {
    if (str1.len != str2.len) {
        return false;
    }

    return 0 == memcmp(str1.data, str2.data, str1.len);
}

bool string_equal_n(const String str1, const String str2, usize n) {
    return 0 == memcmp(str1.data, str2.data, n);
}

i8 string_compare(const String str1, const String str2) {
    i8 flag = 0;

    if (string_equal(str1, str2)) {
        return flag;
    }

    usize i = 0;
    usize max = str1.len <= str2.len ? str1.len : str2.len;

    while (0 == flag && i < max) {
        if (str1.data[i] > str2.data[i]) {
            flag = 1;
        } else if (str1.data[i] < str2.data[i]) {
            flag = -1;
        }
        
        ++i;
    }

    return flag;
}

String string_cat(Arena *arena,  String str1, const String str2) {
    String str = { 0 };

    str.len = str1.len + str2.len + 1;  

    str.data = arena_alloc(arena, str.len);

    memcpy(str.data, str1.data, str1.len);
    memcpy(str.data + str1.len, str2.data, str2.len);

    str.data[str.len] = '\0';

    return str;
}

String string_dup(Arena *arena, const String source) {
    String destination = { 0 };

    /* we have to accommodate for the possibly missing null terminator */
    destination.len = source.len + 1;

    destination = string_alloc(arena, destination.len);

    memcpy(destination.data,source.data, source.len);

    destination.data[destination.len] = '\0';

    return destination;
}

String string_copy(Arena *arena, const String source, usize destination_size) {
    String result = string_alloc(arena, destination_size);

    result.len = source.len;

    memcpy(result.data, source.data, source.len); 

    return result;
}

String string_str_lit(const char *lit) {
    return (String) { 
        .data = (char *)lit, 
        .len = strlen((const char *)lit) 
    };
}

usize string_find_first(const String haystack, const String needle, u32 offset) {
    if (0 == needle.len) {
        return haystack.len + 1;
    }

    if (needle.len > haystack.len || offset > haystack.len) {
        return haystack.len + 1;
    }

    const char *search_start = haystack.data + offset;
    usize remaining_length = haystack.len - offset;

    for (usize i = 0; i <= remaining_length - needle.len; ++i) {
        if (memcmp(search_start + i, needle.data, needle.len) == 0) {
            return offset + i;
        }
    }

    return haystack.len + 1;
}

usize string_find_last(const String haystack, const String needle, u32 offset) {
    if (0 == needle.len) {
        return haystack.len + 1;
    }

    if (needle.len > haystack.len || offset > haystack.len) {
        return haystack.len + 1;  
    }

    usize search_length = haystack.len - offset;
    usize search_start = haystack.len - needle.len;
    
    if (search_length < needle.len) {
        return haystack.len + 1;  
    }

    for (usize i = search_start; i >= offset; --i) {
        if (memcmp(haystack.data + i, needle.data, needle.len) == 0) {
            return i;
        }
    }

    return haystack.len + 1;
}

u64 string_substr_count(const String haystack, const String needle) {
    u64 substr_count = 0;

    usize current_index = 0;

    while (current_index != haystack.len) {
        current_index = string_find_first(haystack, needle, current_index);

        if (current_index == haystack.len + 1) {
            break;
        }

        current_index += needle.len;

        substr_count++;
    }

    return substr_count;
}

String string_replace_all(Arena *arena, const String to_fix, const String needle, const String replacement) {
    u64 strings_to_replace = string_substr_count(to_fix, needle);

    if (strings_to_replace <= 0) {
        return to_fix;
    }

    usize result_size = to_fix.len + (replacement.len - needle.len) * strings_to_replace;
    String result = string_alloc(arena, result_size);

    usize to_fix_index = 0;
    usize result_index = 0;

    while (to_fix_index < to_fix.len) {
        usize next_index = string_find_first(to_fix, needle, to_fix_index);

        if (next_index == to_fix.len + 1) {
            break;
        }

        /* Copy the portion before the next occurrence of the needle */
        usize portion_size = next_index - to_fix_index;
        memcpy(result.data + result_index, to_fix.data + to_fix_index, portion_size);
        result_index += portion_size;

        /* Copy the replacement */
        memcpy(result.data + result_index, replacement.data, replacement.len);
        result_index += replacement.len;

        /* Update the index in the original string */
        to_fix_index = next_index + needle.len;
    }

    /* the possible remaining */
    usize remaining_size = to_fix.len - to_fix_index;

    memcpy(result.data + result_index, to_fix.data + to_fix_index, remaining_size);
    result_index += remaining_size;

    result.len = result_index;

    return result;
}

bool string_is_null(const String str) {
     return (0 == str.len) || ('\0' == str.data[0]) || (NULL == str.data);
}

bool stringarr_remove(StringArr *str_arr, usize index) {
    if (index > str_arr->len) {
        return false;
    }

    memmove(&str_arr->arr[index], &str_arr->arr[index + 1], sizeof(String) * str_arr->len--);

    return true;
}

bool stringarr_insert(StringArr *str_arr, usize index, String src) {
    if (str_arr->len >= str_arr->cap) {
        return false;
    }

    memmove(str_arr->arr + index + 1, str_arr->arr + index, sizeof(String) * (str_arr->len - index));
    str_arr->arr[index] = src;

    str_arr->len++;

    return true;
}

StringVec stringvec_new(void) {
    return (StringVec) {
        .str = calloc(STRINGVEC_DEFAULT_SIZE, sizeof(String)),
        .cap = STRINGVEC_DEFAULT_SIZE,
        .len = 0
    };
}

void stringvec_resize(StringVec *vec, usize new_cap) {
    new_cap *= 2;

    if (new_cap <= vec->cap) {
        return;
    }

    vec->cap = new_cap;

    String *temp = (String *)realloc(vec->str, vec->cap);

    if (!temp) {
        return;
    }

    vec->str = temp;
}

void stringvec_push(StringVec *vec, const String str) {
    if (vec->len >= vec->cap) {
        stringvec_resize(vec, vec->cap);
    }

    vec->str[vec->len++] = str;
}

void stringvec_delete(StringVec *str_vec) {
    free(str_vec->str);
    str_vec->cap = 0;
    str_vec->len = 0;
}

void stringvec_print(const StringVec str_vec) {
    for (usize i = 0; i < str_vec.len; ++i) {
        string_print(str_vec.str[i]);
    }
}

StringArr stringarr_from_args(Arena *arena, u32 argc, char **argv) {
    StringArr arr = stringarr_alloc(arena, sizeof(String) * argc);

    for (u32 i = 0; i < argc; ++i) {
        stringarr_push(&arr, string_str_lit(argv[i]));
    }

    return arr;
}

bool string_contains(const String haystack, const String needle) {
    return string_find_first(haystack, needle, 0) != 0 && string_find_first(haystack, needle, 0) != haystack.len + 1;
}

const char *string_get_lit(const String str) {
    char *lit_result = strdup(str.data);

    lit_result[str.len] = '\0';

    return lit_result;
}

i64 string_stoi(const String str) {
    i8 sign = 1;
    i64 result = 0;

    usize index = 0;

    while (index < str.len && (str.data[index] == ' ' || str.data[index] == '\t')) {
        ++index;
    }

    if (index < str.len && (str.data[index] == '-' || str.data[index] == '+')) {
        sign = (str.data[index] == '-') ? -1 : 1;
        ++index;
    }

    while (index < str.len && str.data[index] >= '0' && str.data[index] <= '9') {
        result = (result * 10) + (str.data[index] - '0');
        ++index;
    }

    return sign * result;
}

#define BUF_SIZE 65536

usize count_lines(const char *filename) {
    FILE *file = 0;

    if (fopen_s(&file, filename, "rb")) {
        return 0;
    }

    char buffer[BUF_SIZE];

    usize counter = 0;

    while (true) {
        usize result = fread(buffer, sizeof(char), BUF_SIZE, file);

        if (ferror(file)) {
            return 0;
        }

        for (usize i = 0; i < result; ++i) {
            if ('\n' == buffer[i]) {
                ++counter;
            }
        }

        if (feof(file)) {
            break;
        }
    }

    fclose(file);

    return counter;
}

void string_trim(String *str) {
    if (isspace(str->data[0])) {
        str->data = &str->data[1];
        str->len--;
    }

    if (isspace(str->data[str->len])) {
        str->data[str->len--] = '\0';
    }
}

StringArr stringarr_from_file(Arena *arena, const char *filename) {
    FILE *file = 0; /* just initialization, there is no other reason for the 0 */

    if (fopen_s(&file, filename, "r")) {
        /* if the opening of the file fails, we return an empty array */
        return (StringArr) { .arr = NULL, .len = 0, .cap = 0 };
    }

    char chr;   /* a single character from the given file */

    Arena scratch;

    arena_init(&scratch);

    StringArr result = stringarr_alloc(arena, count_lines(filename));

    if (0 == result.cap) {
        return (StringArr) { .arr = NULL, .len = 0, .cap = 0 };
    }

    String row = string_alloc(&scratch, STRING_DEFAULT_SIZE);
    usize cap = STRING_DEFAULT_SIZE;

    while (fscanf(file, "%c", &chr) != EOF) {
        if ('\n' == chr) {
            String line = string_copy(arena, row, row.len);
            
            string_trim(&line);  

            stringarr_push(&result, line);
            
            row.len = 0;
        }

        row.data[row.len++] = chr;

        if (row.len >= cap) {
            cap *= 2;
            arena_resize(&scratch, row.data, cap * sizeof(char)); /* technically speaking, the sizeof(char) is unnecessary but it is there for clarity */
        }
    }

    /* the last trailing string */
    stringarr_push(&result, string_copy(arena, row, row.len));

    arena_deinit(&scratch);

    fclose(file);
    
    return result;
}

void string_print_format(const char *__format__, ...) {
    u64 num_string = string_substr_count(string_str_lit(__format__), string_str_lit("{}")) - 1;

    /* grabing the strings and puting them in string array */
    va_list args;

    va_start(args, __format__);

    Arena scratch;

    arena_init(&scratch);

    StringArr strings = stringarr_alloc(&scratch, num_string);

    for (u64 i = 0; i < num_string; ++i) {
        stringarr_push(&strings, (String)va_arg(args, String));
    }

    va_end(args);

    for (usize i = 0, j = 0; i < strlen(__format__); ++i) {
        
        if ('{' == __format__[i]) {
            string_print(strings.arr[j++]);
            ++i;
            continue;
        }

        fprintf(stdout, "%c", __format__[i]);
    }

    arena_deinit(&scratch);
}

String stirng_from_format(Arena *arena, const char *__format__, ...) {
    va_list args;

    va_start(args, __format__);

    char buf[8092];
    vsnprintf(buf, 8092, __format__, args);

    va_end(args);

    String result = string_from(arena, buf);
    
    return result;
}
