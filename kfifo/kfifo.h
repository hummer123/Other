#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <pthread.h>

typedef unsigned int uint32_t;

#define is_power_of_2(x)    (((x) != 0) && ((x & (x - 1)) == 0))
#define min(x, y)           (((x) < (y)) ? (x) : (y))

typedef struct ring_buffer
{
    void *buffer;
    uint32_t size;
    uint32_t in;
    uint32_t out;
    pthread_mutex_t *f_lock;
}ST_RING_BUFFER;


#if 0
#endif

extern uint32_t ring_buffer_put(ST_RING_BUFFER *ring_buf, void *buffer, uint32_t size);
extern uint32_t ring_buffer_get(ST_RING_BUFFER *ring_buf, void *buffer, uint32_t size);
extern uint32_t ring_buffer_len(ST_RING_BUFFER *ring_buf);
extern ST_RING_BUFFER* ring_buffer_init(void *buffer, uint32_t size);
extern void ring_buffer_free(ST_RING_BUFFER *ring_buf);

