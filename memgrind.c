#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "umalloc.h"

int maximal_allocation(){
    int last_allocation = 1;
    char* f = (char*)malloc(last_allocation*1);
    int g = 1;
    int count = 0;
    
    if(f!=NULL){
        free(f);
        while(g == 1){
            last_allocation = last_allocation*2;
            f = (char*)malloc(last_allocation);
            if(f!= NULL){
                free(f);
            }else{
                break;
            }
        }
        last_allocation = last_allocation/2;
        f = (char*)malloc(last_allocation);
        if(f!=NULL){
            free(f);
            return last_allocation;
        }else{
            printf("Error on allocation!\n");
        }

    }else{
        printf("This should never happen in this test case\n");
    }
    return last_allocation;
}
// int maximal_allocation(){
//     int last_allocation = 1;
//     char* f = (char*)malloc(last_allocation*1);
//     int g = 1;
//     int count = 0;
    
//     if(f!=NULL){
//         free(f);
//         while(g == 1){
//             last_allocation = last_allocation*2;
//             f = (char*)malloc(last_allocation);
//             if(f!= NULL){
//                 free(f);
//             }else{
//                 break;
//             }
//         }
//         last_allocation = last_allocation/2;
//         count = last_allocation;
//         while(count>0){
//             f = (char*)malloc(last_allocation+count);
//             if(f!=NULL){
//                 free(f);
//                 last_allocation = last_allocation+count;
//             }else{
//                 count = count/2;
//             }
//         }
//     }else{
//         printf("This should never happen in this test case\n");
//     }
//     return last_allocation;
// }

void basicCoalescence(int last_allocation){
    //Basic Coalescence
   int last_allocation_ref = last_allocation/2;
   char* test_2 = (char*)malloc(last_allocation_ref);
   printf("Allocated half memory\n");
   if(test_2!=NULL){
   	char* test_2_q = (char*)malloc(last_allocation/4);
    printf("Allocated additional quarter memory\n");
   	if(test_2_q!=NULL){   		
   		free(test_2);
   		free(test_2_q);
        printf("Freed allocated memory\n");

   		char* test_2_f = (char*)malloc(last_allocation);
        printf("Max allocation done\n");

   		if(test_2_f!=NULL){
   			free(test_2_f);
   			printf("Basic Coalescence test success\n");
   		}else{
   			printf("Basic Coalescence test Failure\n");   		
   		}
   	}else{
   		printf("Failure allocating quarter of maximal allocation\n");   	
   	}
   }else{
   	printf("Failure allocating half of the maximal allocation!\n");
   }
}

int saturation(int* arr[]){
    struct timespec beginTimer, endTimer;
    int k = 0;
    for(k = 0 ; k < 9216 ; k++){
        arr[k]= (int*)malloc(1024*sizeof(char));
    }
    while((arr[k] = (int*)malloc(sizeof(char))) != NULL) {
        k++;
    }
    printf("Total number of allocations = %d\n", k);
    return k;
}

void timeOverhead(int* arr[],int k){
    //Time Overhead
    struct timespec beginTimer, endTimer;
    free(arr[k-1]);
    clock_gettime(CLOCK_MONOTONIC, &beginTimer);
    arr[k-1] = (int*)malloc(sizeof(char));
    clock_gettime(CLOCK_MONOTONIC, &endTimer);
    printf("\nTime for 1B allocation in saturated mem : %ld nano-seconds\n", endTimer.tv_nsec - beginTimer.tv_nsec);
    // printf("Time for 1B allocation in saturated mem: %lu micro-seconds\n", (endTimer.tv_sec - beginTimer.tv_sec) * 1000 + (endTimer.tv_nsec - beginTimer.tv_nsec) / 1000000);
}

void intermediateCoalescence(int* arr[],int k,int maximalAllocation){
    //Intermediate Coalescence
    for(int e = 0; e < k ; e++){
        free(arr[e]);
    }
    printf("Freed allocated Memory\n");
    char* final_allocation = (char*)malloc(maximalAllocation);
    printf("Maximum Memory allocated\n");
    if(final_allocation!=NULL){
        printf("Intermediate Coalescence Test Success!\n");   
        free(final_allocation);
    }else{
        printf("Intermediate Coalescence Test Failure!\n");
    }
    return ;
}

void consistency(){
    char* first_allocation = (char*)malloc(1);
    first_allocation[0] = 'a';
    free(first_allocation);
    char* second_allocation = (char*)malloc(1);
    free(second_allocation);
    if(first_allocation!=second_allocation){
        printf("Consistency test FAIL!\n");
    }else{
        printf("Address of first allocation %p\n",first_allocation);
        printf("Address of second allocation %p\n",second_allocation);
        printf("Consistency test PASS!\n");
    }
    return;
}
int main(){
    struct timespec beginTimer, endTimer;
    clock_gettime(CLOCK_MONOTONIC, &beginTimer);

    printf("\n********* Consistency Test Start *********\n\n");
    consistency();
    printf("\n********* Consistency Test End *********\n");
    
    printf("\n********* Maximal Allocation Test Start *********\n\n");
    int maxAllocation = maximal_allocation();
    printf("Maximal allocation is %d\n",maxAllocation);
    printf("\n********* Maximal Allocation Test End *********\n");

    printf("\n********* Basic Coalescence Test Start *********\n\n");
    basicCoalescence(maxAllocation);
    printf("\n********* Basic Coalescence Test End *********\n");

    printf("\n*********Saturation Test Start *********\n\n");
    int* arr[118000];
    int k = saturation(arr);
    printf("\n********* Saturation Test End *********\n");


    printf("\n********* Timeoverhead Test Start *********\n\n");
    timeOverhead(arr,k);
    printf("\n********* Timeoverhead Test End *********\n");

    printf("\n********* Intermediate Test Start *********\n\n");
    intermediateCoalescence(arr,k,maxAllocation);
    printf("\n********* Intermediate Test End *********\n");

    clock_gettime(CLOCK_MONOTONIC, &endTimer);


    printf("\nOverall Test Time: %ld milliseconds\n", (endTimer.tv_sec - beginTimer.tv_sec) * 1000 + (endTimer.tv_nsec - beginTimer.tv_nsec) / 1000000);

}
