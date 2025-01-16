#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myLib.h"

int compare_ints(const void *a, const void *b) {
    return *(int *)b - *(int *)a;
}

// Функция 1
void sort_and_replace(char *str, char *res, size_t res_size) {
    char *token = strtok(str, " ");
    int arr[100];
    int count = 0;
    while (token != NULL && count < 100) {
        arr[count++] = atoi(token);
        token = strtok(NULL, " ");
    }

    qsort(arr, count, sizeof(int), compare_ints);

    res[0] = '\0';

    for (int i = 0; i < count; i++) {
        if (arr[i] % 2 == 0) {
          if (strlen(res) + 3 >= res_size){
            fprintf(stderr, "Переполнение буфера в sort_and_replace!\n");
            return;
          }
            strcat(res, "KB ");
        } else {
          int len = snprintf(NULL, 0, "%d ", arr[i]);
          if (strlen(res) + len >= res_size){
            fprintf(stderr, "Переполнение буфера в sort_and_replace!\n");
            return;
          }
            sprintf(res + strlen(res), "%d ", arr[i]);
        }
    }
    if (strlen(res)>0)
        res[strlen(res)-1] = '\0';
    printf("RES %s\n", res);
}

// Функция 2
int sum_numeric(char *str) {
    int sum = 0;
    char *token = strtok(str, " ");
    while (token != NULL) {
        sum += atoi(token);
        token = strtok(NULL, " ");
    }
    return sum;
}

// Функция 3
char *analyze_string(int len) {
    if (len > 2 && len % 32 == 0) {
        return "истина";
    } else {
        return "ложь";
    }
}

