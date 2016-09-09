#ifndef _WRAPPERS_H_
#define _WRAPPERS_H_

struct my_ptr{
    void* ptr;
    int len;
    int writeable; // 0 or 1
};

/* STRING.H */

struct my_ptr* generate_str();

void memcpy_wrapper (struct my_ptr* ptr,int num);
struct my_ptr * memmove_wrapper(struct my_ptr* ptr, int num);
void strcpy_wrapper (struct my_ptr* ptr );
void strncpy_wrapper(const char * source, size_t num );


struct my_ptr * strcat_wrapper ( struct my_ptr * dst, struct my_ptr * src );
struct my_ptr *strncat_wrapper ( struct my_ptr * dst, struct my_ptr * src, size_t num );

int memcmp_wrapper ( struct my_ptr * ptr1, struct my_ptr * ptr2, size_t num );
int strcmp_wrapper ( struct my_ptr * ptr1, struct my_ptr * ptr2 );
int strcoll_wrapper ( struct my_ptr * ptr1, struct my_ptr * ptr2 );
int strncmp_wrapper ( struct my_ptr * ptr1, struct my_ptr * ptr2, size_t num );
void strxfrm_wrapper (struct my_ptr * src);

void memchr_wrapper ( struct my_ptr * src, int val, int num);
void strchr_wrapper ( struct my_ptr * src, int character);
void strcspn_wrapper ( struct my_ptr * str1, struct my_ptr * str2 );
void strpbrk_wrapper ( struct my_ptr * str1, struct my_ptr * str2);
void strrchr_wrapper ( struct my_ptr * str, int character);
void strspn_wrapper ( struct my_ptr * str1, struct my_ptr * str2 );
void strstr_wrapper ( struct my_ptr * str1, struct my_ptr * str2);
void strtok_wrapper ( struct my_ptr * str, struct my_ptr * delimiters );

int strlen_wrapper ( struct my_ptr * str );

struct my_ptr* asctime_wrapper();
struct my_ptr* memset_wrapper (int value, size_t num );
struct my_ptr* strerror_wrapper(int errnum);

/* TIME.H */

struct my_ptr* ctime_wrapper(time_t * a);
struct my_ptr* ctime_r_wrapper(time_t * a);
struct tm *localtime_r_wrapper(const time_t * timep);
struct my_ptr* strptime_wrapper(const char * format,struct tm * tm);
time_t mktime_wrapper(struct tm * tm);
struct my_ptr* asctime_r_wrapper(const struct tm * tm);
struct tm *localtime_wrapper(const time_t * t);
struct my_ptr* strftime_wrapper(struct my_ptr* format,struct tm* timeptr );
struct tm *gmtime_r_wrapper(const time_t *timep);
struct tm *getdate_wrapper(struct my_ptr* ptr);
struct tm *gmtime_wrapper(const time_t * t);

/* STDLIB.H */
double atof_wrapper (struct my_ptr* str);
int atoi_wrapper(struct my_ptr* str);
long int atol_wrapper ( struct my_ptr* str );
double strtod_wrapper (struct my_ptr* str);
long int strtol_wrapper(struct my_ptr* str);
unsigned long int strtoul_wrapper (struct my_ptr* str);
struct my_ptr* getenv_wrapper (struct my_ptr* str);

div_t div_wrapper (int numer, int denom);
ldiv_t ldiv_wrapper (long int numer, long int denom);
int mblen_wrapper (struct my_ptr* str);
int mbtowc_wrapper (struct my_ptr* str);
int wctomb_wrapper (struct my_ptr* str);
struct my_ptr* mbstowcs_wrapper(struct my_ptr* src);

/* MATH.H */
double frexp_wrapper (double x , int* exp);
float frexpf_wrapper (float x , int* exp);
long double frexpl_wrapper (long double x, int* exp);
double modf_wrapper (double x , double* intpart);
float modff_wrapper (float x , float* intpart);
long double modfl_wrapper (long double x, long double* intpart);

/* wchar.h */
struct my_wide_ptr{
    void* ptr;
    int len;
};
struct my_wide_ptr* generate_wide_str();
double wcstod_wrapper (struct my_wide_ptr* str);
long int wcstol_wrapper (struct my_wide_ptr* str);
unsigned long int wcstoul_wrapper (struct my_wide_ptr* str);
void mbrlen_wrapper();
void mbrtowc_wrapper (struct my_ptr* str);
void mbsrtowcs_wrapper (struct my_ptr* str);
void wcrtomb_wrapper (struct my_wide_ptr* str);

struct my_wide_ptr*  wcschr_wrapper(struct my_wide_ptr* str, wchar_t wc);
int wcscmp_wrapper(struct my_wide_ptr* str1, struct my_wide_ptr* str2);
int wcscoll_wrapper(struct my_wide_ptr* str1, struct my_wide_ptr* str2);
int wcscspn_wrapper(struct my_wide_ptr* str1, struct my_wide_ptr* str2);
int wcslen_wrapper (struct my_wide_ptr* str);
int wcsncmp_wrapper(struct my_wide_ptr* str1, struct my_wide_ptr* str2);
void wcspbrk_wrapper (struct my_wide_ptr* str1, struct my_wide_ptr* str2);
struct my_wide_ptr*  wcsrchr_wrapper(struct my_wide_ptr* str, wchar_t wc);
int wcsspn_wrapper(struct my_wide_ptr* str1, struct my_wide_ptr* str2);
int wcsstr_wrapper(struct my_wide_ptr* str1, struct my_wide_ptr* str2);
int wcsxfrm_wrapper (struct my_wide_ptr* str);
struct my_wide_ptr* wmemchr_wrapper(struct my_wide_ptr* str,wchar_t wc);

void c16rtomb_wrapper ( struct my_ptr* str );

#endif
