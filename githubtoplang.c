#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

// Structure to store HTTP response
struct MemoryStruct {
    char *memory;
    size_t size;
};

// Callback function for writing HTTP response
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) {
        fprintf(stderr, "Memory allocation error\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// Function to get JSON from URL
json_t *get_json_from_url(const char *url) {
    CURL *curl_handle;
    CURLcode res;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "C GitHub Language Analyzer");

    res = curl_easy_perform(curl_handle);
    
    if(res != CURLE_OK) {
        fprintf(stderr, "Request error: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl_handle);
        free(chunk.memory);
        return NULL;
    }

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    // Parse JSON
    json_error_t error;
    json_t *root = json_loads(chunk.memory, 0, &error);
    free(chunk.memory);
    
    if(!root) {
        fprintf(stderr, "JSON error: %s (line %d, column %d)\n",
                error.text, error.line, error.column);
        return NULL;
    }

    return root;
}

// Structure to store language information
typedef struct {
    char *name;
    int count;
} LanguageInfo;

// Comparison function for sorting
int compare_languages(const void *a, const void *b) {
    const LanguageInfo *la = (const LanguageInfo *)a;
    const LanguageInfo *lb = (const LanguageInfo *)b;
    return lb->count - la->count;
}

int main(void) {
    char username[256];
    printf("Enter GitHub username: ");
    scanf("%255s", username);
    
    // Form URL for repositories request
    char url[512];
    snprintf(url, sizeof(url), "https://api.github.com/users/%s/repos", username);
    
    json_t *repos = get_json_from_url(url);
    if(!repos || !json_is_array(repos)) {
        fprintf(stderr, "Error fetching data or user not found\n");
        if(repos) json_decref(repos);
        return 1;
    }

    // Collect language statistics
    LanguageInfo *languages = NULL;
    size_t num_languages = 0;
    
    size_t repo_count = json_array_size(repos);
    for(size_t i = 0; i < repo_count; i++) {
        json_t *repo = json_array_get(repos, i);
        json_t *lang_obj = json_object_get(repo, "language");
        
        if(!json_is_string(lang_obj)) continue;
        
        const char *lang_name = json_string_value(lang_obj);
        if(!lang_name) continue;
        
        // Look for language in array
        int found = 0;
        for(size_t j = 0; j < num_languages; j++) {
            if(strcmp(languages[j].name, lang_name) == 0) {
                languages[j].count++;
                found = 1;
                break;
            }
        }
        
        // Add new language
        if(!found) {
            LanguageInfo *temp = realloc(languages, (num_languages + 1) * sizeof(LanguageInfo));
            if(!temp) {
                fprintf(stderr, "Memory allocation error\n");
                continue;
            }
            languages = temp;
            
            languages[num_languages].name = strdup(lang_name);
            languages[num_languages].count = 1;
            num_languages++;
        }
    }

    json_decref(repos);

    // Sort languages by popularity
    qsort(languages, num_languages, sizeof(LanguageInfo), compare_languages);
    
    // Print results
    printf("\nTop programming languages for %s:\n", username);
    printf("---------------------------------\n");
    for(size_t i = 0; i < num_languages && i < 10; i++) {
        printf("%zu. %s: %d repositories\n", i+1, languages[i].name, languages[i].count);
    }
    
    // Free memory
    for(size_t i = 0; i < num_languages; i++) {
        free(languages[i].name);
    }
    free(languages);

    return 0;
}
