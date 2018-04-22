#include "kfifo.h"

uint32_t __ring_buffer_put(ST_RING_BUFFER *ring_buf, void *buffer, uint32_t size)
{
    uint32_t len = 0;

    size = min(size, (ring_buf->size - ring_buf->in + ring_buf->out));
    len  = min(size, (ring_buf->size - (ring_buf->in & (ring_buf->size - 1))));
    memcpy(ring_buf->buffer + (ring_buf->in & (ring_buf->size - 1)), buffer, len);
    memcpy(ring_buf->buffer, buffer + len, size - len);
    ring_buf->in += size;

    return size;
}

uint32_t __ring_buffer_get(ST_RING_BUFFER *ring_buf, void *buffer, uint32_t size)
{
    uint32_t len = 0;

    size = min(size, ring_buf->in - ring_buf->out);
    len  = min(size, ring_buf->size - (ring_buf->out & (ring_buf->size - 1)));
    memcpy(buffer, ring_buf->buffer + (ring_buf->out &(ring_buf->size -1)), len);
    memcpy(buffer + len, ring_buf->buffer, size - len);
    ring_buf->out += size;
    
    return size;
}

uint32_t __ring_buffer_len(ST_RING_BUFFER *ring_buf)
{
    return (ring_buf->in - ring_buf->out);
}


#if 0
#endif

uint32_t ring_buffer_put(ST_RING_BUFFER *ring_buf, void *buffer, uint32_t size)
{
    assert(ring_buf || buffer);

    uint32_t u32Ret = 0;

    pthread_mutex_lock(ring_buf->f_lock);
    u32Ret = __ring_buffer_put(ring_buf, buffer, size);
    pthread_mutex_unlock(ring_buf->f_lock);

    return u32Ret;
}

uint32_t ring_buffer_get(ST_RING_BUFFER *ring_buf, void *buffer, uint32_t size)
{
    assert(ring_buf || buffer);

    uint32_t u32Ret = 0;

    pthread_mutex_lock(ring_buf->f_lock);
    u32Ret = __ring_buffer_get(ring_buf, buffer, size);

    if(ring_buf->in == ring_buf->out)
    {
        ring_buf->in  = 0;
        ring_buf->out = 0;
    }
    
    pthread_mutex_unlock(ring_buf->f_lock);

    return u32Ret;
}

uint32_t ring_buffer_len(ST_RING_BUFFER *ring_buf)
{
    assert(ring_buf);

    uint32_t u32Ret = 0;

    pthread_mutex_lock(ring_buf->f_lock);
    u32Ret = __ring_buffer_len(ring_buf);
    pthread_mutex_unlock(ring_buf->f_lock);

    return u32Ret;
}


ST_RING_BUFFER* ring_buffer_init(void *buffer, uint32_t size)
{
    ST_RING_BUFFER *ring_buf = NULL;
    
    assert(buffer);

    if (!is_power_of_2(size))
    {
        fprintf(stderr, "size must be power of 2.\n");
        return ring_buf;
    }
    
    ring_buf = (ST_RING_BUFFER *)malloc(sizeof(ST_RING_BUFFER));
    if(ring_buf == NULL)
    {
        fprintf(stderr, "Failed to malloc!\n");
        return NULL;
    }
    memset(ring_buf, 0, sizeof(ST_RING_BUFFER));

    ring_buf->f_lock = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
    if (0 != pthread_mutex_init(ring_buf->f_lock, NULL))
    {
        fprintf(stderr, "Failed init f_lock!\n");
        free(ring_buf);
        return NULL;
    }

    ring_buf->buffer = buffer;
    ring_buf->size = size;
    ring_buf->in = 0;
    ring_buf->out = 0; 

    return ring_buf;
}


void ring_buffer_free(ST_RING_BUFFER *ring_buf)
{
    if(ring_buf)
    {
        if(ring_buf->f_lock)
        {
            pthread_mutex_destroy(ring_buf->f_lock);
            free(ring_buf->f_lock);
            ring_buf->f_lock = NULL;
        }
        if(ring_buf->buffer)
        {
            free(ring_buf->buffer);
            ring_buf->buffer = NULL;
        }

        free(ring_buf);
        ring_buf = NULL;
    }
}

