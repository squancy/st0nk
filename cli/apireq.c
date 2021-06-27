#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "headers/colors.h"
#include "headers/lib.h"

// Ptr points to data allocated in the heap that stores the JSON response
typedef struct string_buffer_s {
  char* ptr;
  size_t len;
} string_buffer_t;

static void string_buffer_initialize(string_buffer_t* sb) {
  sb->len = 0;
  sb->ptr = malloc(sb->len + 1);
  if (sb->ptr == NULL) {
    exit(EXIT_FAILURE);
  }
  sb->ptr[0] = '\0';
}

static void string_buffer_finish(string_buffer_t* sb) {
  free(sb->ptr);
  sb->len = 0;
  sb->ptr = NULL;
}

/*
  This function is called whenever new data is available
  Appends the newly downloaded data to the previously stored response in the heap
*/
static size_t string_buffer_callback(void* buf, size_t size, size_t nmemb, void* data) {
  string_buffer_t* sb = data;
  size_t new_len = sb->len + size * nmemb;
  sb->ptr = realloc(sb->ptr, new_len + 1);
  if (sb->ptr == NULL) {
    exit(EXIT_FAILURE);
  }

  memcpy(sb->ptr + sb->len, buf, size * nmemb);
  sb->ptr[new_len] = '\0';
  sb->len = new_len;

  return size * nmemb;
}

static size_t header_callback(void* buf, size_t size, size_t nmemb, void* data) {
  return string_buffer_callback(buf, size, nmemb, data);
}

static size_t write_callback(void* buf, size_t size, size_t nmemb, void* data) {
  return string_buffer_callback(buf, size, nmemb, data);
}

char* get_json(char* url, int include_headers) {
  CURL* curl;
  CURLcode res;
  string_buffer_t strbuf;
  string_buffer_initialize(&strbuf);

  curl = curl_easy_init();

  if (!curl) {
    string_buffer_finish(&strbuf);
    return "Curl init error";
  }
  
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strbuf);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
  if (include_headers) {
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &strbuf);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
  }

  res = curl_easy_perform(curl);
	
  if (res != CURLE_OK) {
    curl_easy_cleanup(curl);
    string_buffer_finish(&strbuf);

    return "Curl perform error";
  }

  return strbuf.ptr;
  
  /*
  curl_easy_cleanup(curl);
  string_buffer_finish(&strbuf);
  */
}
