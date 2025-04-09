#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t length = 0;
  while(*s){
    length++;
    s++;
  }
  return length;
}

char *strcpy(char *dst, const char *src) {
  const char *tmp1 = (const char*)src;
  char *tmp2 = (char*)dst;
  while(*tmp1) {
    *(tmp2++) = *(tmp1++);
  }
  *tmp2 = '\0';
  return dst;
}


char *strncpy(char *dst, const char *src, size_t n) {
  size_t i;
  for(i=0; i<n && src[i]!='\0';i++){
    dst[i] = src[i];
  }
  for(;i<n;i++){
    dst[i] = '\0';
  }
  return dst;
}


char *strcat(char *dst, const char *src) {
  size_t dst_len = 0,i = 0,j = 0;
  while(*(dst+i)) {
    dst_len++;
    i++;
  }
  //overlap the '\0'
  while(src[j] != '\0') {
    dst[dst_len+j] = src[j];
    j++;
  }
  dst[dst_len+j] = '\0';
  return dst;
}


int strcmp(const char *s1, const char *s2) {
  char *tmp1 = (char*)s1;
  char *tmp2 = (char*)s2;
  while((*tmp1==*tmp2) && *tmp2) {
      tmp1++;
      tmp2++;
  }
  return (*tmp1-*tmp2);
}


int strncmp(const char *s1, const char *s2, size_t n) {
  //at most n bytes or meet with '\0'
  char *tmp1 = (char*)s1;
  char *tmp2 = (char*)s2;
  while((*tmp1==*tmp2) && *tmp1 && (--n)) {
      tmp1++;
      tmp2++;
  }
  return (*tmp1-*tmp2);
}

//return a pointer to the memory area
void *memset(void *s, int c, size_t n) {
  unsigned char *tmp = s;
  if(tmp==NULL || n <0) {
    return NULL;
  }
  while(*tmp!='\0' && n!=0) {
    *(tmp++) = c;
    n--;
    
  }
  return s;
}

//
void *memmove(void *dst, const void *src, size_t n){
  size_t i = 0;
  const unsigned char *s = src;
  unsigned char *d = dst;
  unsigned char temp[256];
  for(i=0;i<n;i++) {
    temp[i] = *(s+i);
  }
  for(i=0;i<n;i++){
    *(d+i) = temp[i];
  }
  return d;
  // void *pTmp = NULL;
	
	// if(NULL == dst || NULL == src || n <= 0)
	// {
	// 	return NULL;
	// }	
  //   pTmp = dst;
	// if(src >= dst)
	// {
		
  //       while(n > 0)
  //       {
	// 		*(char *)dst++ = *(char *)src++;
	// 		n--;
	// 	}
	// }
	// else
	// {
	// 	while(n > 0)
	// 	{
	// 		*((char *)dst + n - 1) = *((char *)src + n -1);
	// 		n--;
	// 	}
	// }
	// return pTmp;
}

void *memcpy(void *out, const void *in, size_t n) {
  size_t i;
  const unsigned char *src = (const void*)in;
  unsigned char *dst = (void*)out;

  for(i=0; i<n && src[i]!='\0';i++) {
    *(dst+i) = *(src+i);
  }
  for(;i<n;i++){
    *(dst+i) = '\0';
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  size_t i;
  const unsigned char *cmp1 = (const unsigned char*)s1;
  const unsigned char *cmp2 = (const unsigned char*)s2;
  for(i=0;i<n;i++,cmp1++,cmp2++) { 
    if(*cmp1 < *cmp2) return -1;
    else if(*cmp1 > *cmp2) return 1;
  }
  return 0;
}

#endif
