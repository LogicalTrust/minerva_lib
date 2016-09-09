#include <sys/types.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <wchar.h>
#include <uchar.h>
#include <math.h>
#include <setjmp.h>
#include "wrappers.h"


struct my_ptr* generate_str(){
	char format[] = "%qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM_-0987654321^#";

    char* r;
    struct my_ptr* ptr = malloc(sizeof(struct my_ptr));
    size_t i;
    size_t max = rand() % 300;
    r = calloc(max+1,sizeof(*r));
    if(r==NULL)
        goto exit;
    for(i=0;i<max;i++)
        r[i] = format[rand() % sizeof(format)];
    r[max] = '\0';
    
    exit:
        ptr->ptr = r;
        ptr->len = max;
        ptr->writeable =1;
        return ptr; 
}
struct my_wide_ptr* generate_wide_str()
{
    wchar_t  format[] = L"%qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM_-0987654321^#";

    wchar_t * r = NULL;
    struct my_wide_ptr* ptr = malloc(sizeof(struct my_wide_ptr));
    size_t i;
    size_t max = rand() % 300;
    r = calloc(max+1,sizeof(*r));
    if(r==NULL)
        goto exit;
    for(i=0;i<max;i++)
        r[i] = format[rand() % sizeof(format)];
    r[max] = L'\0';
    
    exit:
        ptr->ptr = r;
        ptr->len = max;
        return ptr; 
}
struct my_ptr* asctime_wrapper()
{
    struct my_ptr* ptr = malloc(sizeof(struct my_ptr));
    char* temp;
    time_t czas;
    time( & czas );
    temp = asctime( localtime( & czas ) ) ;
    ptr->ptr = calloc(strlen(temp)+1,1);
    memcpy(ptr->ptr,temp,strlen(temp));
    ptr->len = strlen(temp);
    return ptr;
}
void memcpy_wrapper (struct my_ptr* ptr,int num)
{
    
    if(ptr->ptr!=NULL && num>0 && ptr->len>num)
    {
        void* destination = malloc(ptr->len+1);
        memcpy(destination,ptr->ptr,num);
        free(destination);
    }
}
struct my_ptr * memmove_wrapper(struct my_ptr* ptr, int num)
{
    struct my_ptr* ret = malloc(sizeof(struct my_ptr));
    if(ptr && ptr->ptr!=NULL && num>0 && ptr->len>num)
    {
        void* destination = calloc(ptr->len+1,1);
        memcpy(destination,ptr->ptr,num);
        ret->ptr = destination;
        ret->len = num;
        ret->writeable = 1;
        memmove(ret->ptr,ptr->ptr,num);
        return ret;
    }
    return NULL;
}
void strcpy_wrapper (struct my_ptr* ptr )
{
    if(ptr->ptr!=NULL && ptr->len > 0){
        char* destination = malloc(ptr->len+1);
        ((char*)ptr->ptr)[ptr->len] = '\0';
        strcpy(destination,ptr->ptr);
        free(destination);
    }
}
void strncpy_wrapper(const char * source, size_t num )
{
    if(source!=NULL && num>0){
        char* destination = malloc(num);
        if(!destination)
            return;
        strncpy(destination,source,num);
        free(destination);
    }
}

struct my_ptr * strcat_wrapper ( struct my_ptr * dst, struct my_ptr * src )
{
    if(dst->ptr != src->ptr && src->ptr!=NULL && dst->ptr !=NULL ){
        struct my_ptr* ret = malloc(sizeof(struct my_ptr));
        ret->ptr =  malloc(dst->len+src->len+2);
        ret->len = dst->len+src->len;
        memcpy(ret->ptr,dst->ptr,dst->len);
        strcat(ret->ptr,src->ptr);
        return ret;
    }
    return NULL;
}
struct my_ptr *strncat_wrapper ( struct my_ptr * dst, struct my_ptr * src, size_t num )
{
    //num is maximium, so is not a requirement
   if(dst->ptr != src->ptr && src->ptr!=NULL && dst->ptr !=NULL){
        struct my_ptr* ret = malloc(sizeof(struct my_ptr));
        ret->ptr =  malloc(dst->len+num+2);
        ret->len = dst->len+num;
        memcpy(ret->ptr,dst->ptr,dst->len);
        strncat(ret->ptr,src->ptr,num);
        return ret;
      }
    return NULL;
}
int memcmp_wrapper ( struct my_ptr * ptr1, struct my_ptr * ptr2, size_t num )
{
    if(ptr1->ptr!=NULL && ptr2->ptr!=NULL)
        return memcmp(ptr1->ptr,ptr2->ptr,num);
    return 0;
}
int strcmp_wrapper ( struct my_ptr * ptr1, struct my_ptr * ptr2 )
{
    if(ptr1->ptr!=NULL && ptr2->ptr!=NULL)
        return strcmp(ptr1->ptr,ptr2->ptr);
    return 0;
}
int strcoll_wrapper ( struct my_ptr * ptr1, struct my_ptr * ptr2 )
{
     if(ptr1->ptr!=NULL && ptr2->ptr!=NULL)
        return strcoll(ptr1->ptr,ptr2->ptr);
    return 0;
}
int strncmp_wrapper ( struct my_ptr * ptr1, struct my_ptr * ptr2, size_t num )
{
    if(ptr1->ptr!=NULL && ptr2->ptr!=NULL)
        return strncmp(ptr1->ptr,ptr2->ptr,num);
    return 0;
}
void strxfrm_wrapper (struct my_ptr * src)
{
    if(src->ptr !=NULL)
    {
       strxfrm(NULL,src->ptr,0);
    }
}
void memchr_wrapper ( struct my_ptr * src, int val, int num)
{
    if(src->ptr != NULL)
    {
        int len = num;
        if(num>src->len)
            len = src->len;
        (void)memchr(src->ptr,val,len);
    }
}
void strchr_wrapper ( struct my_ptr * src, int character)
{
    if(src->ptr!=NULL)
        (void)strchr(src->ptr,character);
}
void strcspn_wrapper ( struct my_ptr * str1, struct my_ptr * str2 )
{
    if(str1->ptr!=NULL && str2->ptr!=NULL)
        (void)strcspn(str1->ptr,str2->ptr);
}
void strpbrk_wrapper ( struct my_ptr * str1, struct my_ptr * str2)
{
    if(str1->ptr!=NULL && str2->ptr!=NULL)
        (void)strpbrk(str1->ptr,str2->ptr);
}
void strrchr_wrapper ( struct my_ptr * str, int character)
{
    if(str->ptr!=NULL)
        (void)strrchr(str->ptr,character);
}
void strspn_wrapper ( struct my_ptr * str1, struct my_ptr * str2 )
{
    if(str1->ptr!=NULL && str2->ptr!=NULL)
        (void)strspn(str1->ptr,str2->ptr);    
}
void strstr_wrapper ( struct my_ptr * str1, struct my_ptr * str2)
{
    if(str1->ptr!=NULL && str2->ptr!=NULL)
        (void)strstr(str1->ptr,str2->ptr);
}
void strtok_wrapper ( struct my_ptr * str, struct my_ptr * delimiters )
{
    if(str->ptr!=NULL && delimiters->ptr!=NULL)
    {
        char* new_str = malloc(str->len);
        memcpy(new_str,str->ptr,str->len);
        strtok(new_str,delimiters->ptr);
        free(new_str);
    }
}
int strlen_wrapper ( struct my_ptr * str )
{
    if(str->ptr!=NULL)
        return strlen((const char *)str);
    return 0;
}
struct my_ptr* memset_wrapper (int value, size_t num )
{
    if(num >1){
        struct my_ptr* ret = malloc(sizeof(struct my_ptr));
        ret->ptr = malloc(num+1);
        memset(ret->ptr,value,num);
        ((char*)ret->ptr)[num] = '\0';
        ret->len = num;
        return ret;
    }
    return NULL;
}
struct my_ptr* strerror_wrapper(int errnum)
{
    char* src = strerror(errnum);
    struct my_ptr* ret = malloc(sizeof(struct my_ptr));
    ret->len = strlen(src);
    ret->ptr = calloc(ret->len+1,1);
    memcpy(ret->ptr,src,ret->len);
    return ret;
}

struct my_ptr* ctime_wrapper(time_t * a)
{
    if(a)
    {
        char* src = ctime(a);
        struct my_ptr* ret = malloc(sizeof(struct my_ptr));
        ret->len = strlen(src);
        ret->ptr = calloc(ret->len+1,1);
        memcpy(ret->ptr,src,ret->len);
        return ret;
    }
    return NULL;
}
 struct my_ptr* ctime_r_wrapper(time_t * a)
 {
     if(a)
     {
        struct my_ptr* ret = malloc(sizeof(struct my_ptr));
        ret->len = 64;
        ret->ptr = calloc(64,1);
        ctime_r(a,ret->ptr);
     }
     return NULL;
 }
struct tm *localtime_r_wrapper(const time_t * timep)
{
    if(timep!=NULL )
    {
        struct tm* ret = malloc(sizeof(struct tm));
        localtime_r(timep,ret);
    }
    return NULL;
}
struct my_ptr* strptime_wrapper(const char * format,struct tm * tm)
{
    if(format && tm)
    {
        struct my_ptr* ret = malloc(sizeof(struct my_ptr));
        ret->len = 64;
        ret->ptr = calloc(64,1);
        strptime(ret->ptr,format,tm);
        return ret;
    }
    return NULL;
}
time_t mktime_wrapper(struct tm * tm)
{
    time_t ret_zero; // TODO: How to declare an empty time_t ?
    if(tm)
        return mktime(tm);
    else
        return ret_zero;
}
struct my_ptr* asctime_r_wrapper(const struct tm * tm)
{
    if(tm)
    {
        struct my_ptr* ret = malloc(sizeof(struct my_ptr));
        ret->len = 64;
        ret->ptr = calloc(64,1);
        asctime_r(tm,ret->ptr);
        return ret;
    }
    return NULL;
}

struct tm *localtime_wrapper(const time_t * t)
{
    if(t)
        return localtime(t);
    return NULL;
}

struct my_ptr* strftime_wrapper(struct my_ptr* format,struct tm* timeptr )
{
    if(timeptr && format->ptr)
    {
        struct my_ptr* ret = malloc(sizeof(struct my_ptr));
        ret->len = 64;
        ret->ptr = calloc(64,1);
        strftime(ret->ptr,ret->len,format->ptr, timeptr);
        return ret;
    }
    return NULL;
}
struct tm *gmtime_r_wrapper(const time_t *timep)
{
    if(timep)
    {
        struct tm* ret = malloc(sizeof(struct tm));
        gmtime_r(timep,ret);
        return ret;
    }
    return NULL;
}
struct tm *getdate_wrapper(struct my_ptr* ptr)
{
    if(ptr->ptr)
        return getdate(ptr->ptr);
    return NULL;
}
struct tm *gmtime_wrapper(const time_t * t)
{
    if(t)
        return gmtime(t);
    return NULL;
}
double atof_wrapper (struct my_ptr* str)
{
    if(str->ptr)
        return atof(str->ptr);
    return 0;
}
int atoi_wrapper(struct my_ptr* str)
{
    if(str->ptr)
        return atoi(str->ptr);
    return 0;
}
long int atol_wrapper ( struct my_ptr* str )
{
   if(str->ptr)
        return atol(str->ptr);
    return 0; 
}
double strtod_wrapper (struct my_ptr* str)
{
     if(str->ptr)
        return strtod(str->ptr,NULL);
    return 0; 
}
long int strtol_wrapper (struct my_ptr* str)
{
    if(str->ptr)
        return strtol(str->ptr,NULL,0);
    return 0;
}
unsigned long int strtoul_wrapper (struct my_ptr* str)
{
    if(str->ptr)
        return strtoul(str->ptr,NULL,0);
    return 0;
}
struct my_ptr* getenv_wrapper (struct my_ptr* str)
{
    if(str->ptr)
    {
        char* temp = getenv(str->ptr);
        struct my_ptr* ret = calloc(sizeof(struct my_ptr),1);
        ret->len = 64;
        ret->ptr = calloc(64,1);
        if(!temp)
            return NULL;
        if(strlen(temp) > 64)
            memcpy(ret->ptr, temp, 64);
        else
        {
            memcpy(ret->ptr, temp, strlen(temp));
            ret->len = strlen(temp);
        }
        return ret;
    }
    return NULL;
}
div_t div_wrapper (int numer, int denom)
{
    if(denom==0)
        return div(numer,1);
    return div(numer,denom);
}
ldiv_t ldiv_wrapper (long int numer, long int denom)
{
    if(denom==0)
        return ldiv(numer,1);
    return ldiv(numer,denom);
}
int mblen_wrapper (struct my_ptr* str)
{
    if(str->ptr)
        return mblen(str->ptr,str->len);
    return 0;
}
int mbtowc_wrapper (struct my_ptr* str)
{
    if(str->ptr)
    {
        return mbtowc(NULL,str->ptr,str->len);
    }
    return 0;
}

int wctomb_wrapper (struct my_ptr* str)
{
    if(str->ptr)
    {
        return wctomb(str->ptr,NULL);
    }
    return 0;
}

struct my_ptr* mbstowcs_wrapper(struct my_ptr* src)
{
    if(src->ptr)
    {
        struct my_ptr* ret = calloc(sizeof(struct my_ptr),1);
        ret->len = 64;
        ret->ptr = calloc(64,1);
        int max_len = 64;
        if(src->len < 64)
            max_len = src->len;
        mbstowcs(ret->ptr,src->ptr,max_len);
    }
}

double frexp_wrapper(double x , int* exp)
{
    if(exp)
        return frexp(x,exp);
    return 0;
}
float frexpf_wrapper(float x, int* exp)
{
    if(exp)
        return frexpf(x,exp);
    return 0;
}
long double frexpl_wrapper (long double x, int* exp)
{
    if(exp)
        return frexpl(x,exp);
    return 0;
}
double modf_wrapper(double x, double* intpart)
{
    if(intpart)
        return modf(x,intpart);
    return 0;
}
float modff_wrapper(float x, float* intpart)
{
    if(intpart)
        return modff(x,intpart);
    return 0;
}
long double modfl_wrapper(long double x, long double* intpart)
{
    if(intpart)
        return modfl(x,intpart);
    return 0;
}
void c16rtomb_wrapper ( struct my_ptr* str )
{
    char buffer [MB_CUR_MAX];
    mbstate_t mbs;
    char* pt = str->ptr;
    size_t length;
    
    mbrlen (NULL,0,&mbs);   /* initialize mbs */
    while (*pt) {
        length = c16rtomb(buffer,*pt,&mbs);
        if ((length==0)||(length>MB_CUR_MAX)) break;
        ++pt;
    }
}

double wcstod_wrapper (struct my_wide_ptr* str)
{
    if(str->ptr)
        return wcstod(str->ptr,NULL);
    return 0;
}

long int wcstol_wrapper (struct my_wide_ptr* str)
{
   if(str->ptr)
   {
       return wcstol(str->ptr,NULL,10);
   }
   return 0;   
}

unsigned long int wcstoul_wrapper (struct my_wide_ptr* str)
{
    if(str->ptr)
   {
       return wcstoul(str->ptr,NULL,10);
   }
   return 0;
}

void mbrlen_wrapper(struct my_ptr* str)
{
    mbstate_t mbs;
    if ( !mbsinit(&mbs) )
        memset (&mbs,0,sizeof(mbs));
    mbrlen (NULL,0,&mbs);
    if(str->ptr!=NULL && str->len>0)
    {
        mbrlen(str->ptr,str->len,&mbs);
    }
}

void mbrtowc_wrapper (struct my_ptr* str)
{
    if(str->ptr && str->len > 0){
        mbstate_t mbs;
        if ( !mbsinit(&mbs) )
            memset (&mbs,0,sizeof(mbs));
        mbrlen (NULL,0,&mbs);
        mbrtowc(NULL,str->ptr,str->len,&mbs);
    }
}

void mbsrtowcs_wrapper (struct my_ptr* str)
{
    if(str->ptr && str->len>0){
        mbstate_t mbs;
        if ( !mbsinit(&mbs) )
            memset (&mbs,0,sizeof(mbs));
        mbrlen (NULL,0,&mbs);
        mbsrtowcs(NULL,&str->ptr,0,&mbs);
    }
}

void wcrtomb_wrapper (struct my_wide_ptr* str)
{
    if(str->ptr && str->len>0){
        mbstate_t mbs;
        char buffer [MB_CUR_MAX];
        if ( !mbsinit(&mbs) )
            memset (&mbs,0,sizeof(mbs));
        mbrlen (NULL,0,&mbs);
        wcrtomb(buffer,*(wchar_t*)str->ptr,&mbs);
    }
}

void wcsrtombs_wrapper(struct my_wide_ptr* str)
{
     if(str->ptr && str->len>0){
        mbstate_t mbs;
        char buffer [MB_CUR_MAX];
        if ( !mbsinit(&mbs) )
            memset (&mbs,0,sizeof(mbs));
        mbrlen (NULL,0,&mbs);
        wcsrtombs (buffer, &str->ptr, str->len, &mbs);
     }
}

struct my_wide_ptr*  wcschr_wrapper(struct my_wide_ptr* str, wchar_t wc)
{
    if(str->ptr && str->len>0)
    {
        wchar_t* temp = wcschr ( str->ptr, wc);
        struct my_wide_ptr* ret = malloc(sizeof(struct my_wide_ptr));
        ret->len = wcslen(temp);
        ret->ptr = malloc(ret->len);
        wmemcpy(ret->ptr,temp,ret->len);
        return ret;
    }
    return NULL;
}
int wcscmp_wrapper(struct my_wide_ptr* str1, struct my_wide_ptr* str2)
{
    if(str1->ptr && str2->ptr)
        return wcscmp (str1->ptr,str2->ptr);
    return 0;
}
int wcscoll_wrapper(struct my_wide_ptr* str1, struct my_wide_ptr* str2)
{
    if(str1->ptr && str2->ptr)
        return wcscoll (str1->ptr,str2->ptr);
    return 0;
}
int wcscspn_wrapper(struct my_wide_ptr* str1, struct my_wide_ptr* str2)
{
    if(str1->ptr && str2->ptr)
        return wcscspn (str1->ptr,str2->ptr);
    return 0;
}
int wcslen_wrapper (struct my_wide_ptr* str)
{
    if(str->ptr)
        wcslen (str->ptr);
    return 0;
}
int wcsncmp_wrapper(struct my_wide_ptr* str1, struct my_wide_ptr* str2)
{
    if(str1->ptr && str2->ptr)
    {
        int len = str1->len;
        if(str2->len < len)
            len = str2->len;
        return wcsncmp(str1->ptr,str2->ptr,len);
    }
    return 0;
}
void wcspbrk_wrapper (struct my_wide_ptr* str1, struct my_wide_ptr* str2)
{
    if(str1->ptr && str2->ptr)
        wcspbrk (str1->ptr,str2->ptr);
}
struct my_wide_ptr*  wcsrchr_wrapper(struct my_wide_ptr* str, wchar_t wc)
{
    if(str->ptr && str->len>0)
    {
        wchar_t* temp = wcsrchr( str->ptr, wc);
        struct my_wide_ptr* ret = malloc(sizeof(struct my_wide_ptr));
        ret->len = wcslen(temp);
        ret->ptr = malloc(ret->len);
        wmemcpy(ret->ptr,temp,ret->len);
        return ret;
    }
    return NULL;
}
int wcsspn_wrapper(struct my_wide_ptr* str1, struct my_wide_ptr* str2)
{
    if(str1->ptr && str2->ptr)
    {
        return wcsspn(str1->ptr,str2->ptr);
    }
    return 0;
}
int wcsstr_wrapper(struct my_wide_ptr* str1, struct my_wide_ptr* str2)
{
    if(str1->ptr && str2->ptr)
    {
        return (int) wcsstr(str1->ptr,str2->ptr);
    }
    return 0;
}
int wcsxfrm_wrapper (struct my_wide_ptr* str)
{
    if(str->ptr)
    {
        return (int) wcsxfrm (NULL, str->ptr, 0);
    }
    return 0;
}
struct my_wide_ptr* wmemchr_wrapper(struct my_wide_ptr* str,wchar_t wc)
{
    if(str->ptr)
    {
       wchar_t* temp = wmemchr(str->ptr,wc,str->len);
       struct my_wide_ptr* ret = malloc(sizeof(struct my_wide_ptr));
       ret->len = wcslen(temp);
       ret->ptr = malloc(ret->len);
       wmemcpy(ret->ptr,temp,ret->len);
       return ret;
    }
    return NULL;
}