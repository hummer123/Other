#include "kfifo.h"
//#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define BUFFER_SIZE (1024)

typedef struct student_info
{
    uint32_t u32_id;
    uint32_t u32_age;
    uint32_t u32_score;
}ST_STUDENT_INFO;


uint32_t roundup_pow_of_two(uint32_t n)
{
    uint32_t len = 0;
    uint32_t i = 0;

    for (i = n; i != 0; i >>= 1)
    {
        len++;
    }

	return 1UL << len;
}


void print_student_info(const ST_STUDENT_INFO *std_info)
{
    assert(std_info);

    printf("id   : %u\n", std_info->u32_id);
    printf("age  : %u\n", std_info->u32_age);
    printf("score: %u\n", std_info->u32_score);

    return;
}


ST_STUDENT_INFO *make_student_info(time_t timer)
{
    ST_STUDENT_INFO *std_info = (ST_STUDENT_INFO *)malloc(sizeof(ST_STUDENT_INFO));
    if(!std_info)
    {
        fprintf(stderr, "Failed to malloc!\n");
        return NULL;
    }
    
    srand(timer);
    std_info->u32_id    = 10000 + rand() % 9999;
    std_info->u32_age   = rand() % 30;
    std_info->u32_score = rand() % 101;
    print_student_info(std_info);

    return std_info;
}


void *consumer_proc(void *arg)
{
    ST_RING_BUFFER *ring_buf = (ST_RING_BUFFER *)arg;
    ST_STUDENT_INFO std_info;
    
    while(1)
    {
        sleep(2);

        printf("--------------------------------------------------------\n");
        printf("Get a student info from ring_buffer.\n");
        ring_buffer_get(ring_buf, (void *)&std_info, sizeof(ST_STUDENT_INFO));
        printf("ring_buffer length: %u\n", ring_buffer_len(ring_buf));
        print_student_info(&std_info);
        printf("--------------------------------------------------------\n");
    }

    return NULL;
}


void *producer_proc(void *arg)
{
    ST_RING_BUFFER  *ring_buf = (ST_RING_BUFFER *)arg;
    ST_STUDENT_INFO *std_info = NULL;
    time_t   cur_time;
    uint32_t u32Seed = 0;

    while(1)
    {
        time(&cur_time);
        srand(cur_time);

        u32Seed = rand() % 1111;
        
        printf("--------------------------------------------------------\n");
        std_info = make_student_info(cur_time + u32Seed);
        printf("Put a student info to ring_buffer.\n");
        ring_buffer_put(ring_buf, (void *)std_info, sizeof(ST_STUDENT_INFO));
        printf("ring_buffer length: %u\n", ring_buffer_len(ring_buf));
        printf("--------------------------------------------------------\n");        

        sleep(1);
    }

    return NULL;
}


pthread_t consumer_thread(void *arg)
{
    int err = 0;
    pthread_t tid;

    err = pthread_create(&tid, NULL, consumer_proc, arg);
    if(0 != err)
    {
        fprintf(stderr, "Failed to create consumer thread!\n");
        return -1;
    }
    
    return tid;
}


pthread_t producer_thread(void *arg)
{
    int err = 0;
    pthread_t tid;

    err = pthread_create(&tid, NULL, producer_proc, arg);
    if(0 != err)
    {
        fprintf(stderr, "Failed to create producer thread!\n");
        return -1;
    }
    
    return tid;        
}


int main()
{
    uint32_t u32Size = 0, len = 0;
    void *buffer = NULL;
    ST_RING_BUFFER *ring_buf = NULL;
    pthread_t consumer_tid, producer_tid;

    len = sizeof(ST_STUDENT_INFO) * BUFFER_SIZE;
    u32Size = roundup_pow_of_two(len);
    
    printf("\nlen = %u, roundup = %u\n", len, u32Size);

    buffer = (void *)malloc(u32Size);
    if (!buffer)
    {
        fprintf(stderr, "Failed to malloc buffer!\n");
        return -1;
    }

    ring_buf = ring_buffer_init(buffer, u32Size);
    if (!ring_buf)
    {
        fprintf(stderr, "Failed init ring buffer!\n");
        free(buffer);
        buffer = NULL;
        return -1;
    }
    
    producer_tid = producer_thread((void *)ring_buf);
    consumer_tid = consumer_thread((void *)ring_buf);

    printf("producer_tid = %u\n", (uint32_t)producer_tid);
    printf("consumer_tid = %u\n", (uint32_t)consumer_tid);

    pthread_join(producer_tid, NULL);
    pthread_join(consumer_tid, NULL);

    ring_buffer_free(ring_buf);

    return 0;
}

