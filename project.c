#include <stdio.h>
#include <stdint.h>

#define BUFFER_SIZE 8U

typedef enum
{
    RINGBUF_OK = 0,
    RINGBUF_ERROR
} ringbuf_status_t;

typedef struct
{
    uint8_t buffer[BUFFER_SIZE];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} ringbuf_t;


void ringbuf_init(ringbuf_t *rb);
ringbuf_status_t ringbuf_write(ringbuf_t *rb, uint8_t data);
ringbuf_status_t ringbuf_read(ringbuf_t *rb, uint8_t *data);
uint8_t ringbuf_count(const ringbuf_t *rb);
uint8_t ringbuf_is_full(const ringbuf_t *rb);
uint8_t ringbuf_is_empty(const ringbuf_t *rb);


void ringbuf_init(ringbuf_t *rb)
{
    rb->head = 0U;
    rb->tail = 0U;
    rb->count = 0U;
}


ringbuf_status_t ringbuf_write(ringbuf_t *rb, uint8_t data)
{
    if (ringbuf_is_full(rb))
    {
        return RINGBUF_ERROR;
    }

    rb->buffer[rb->head] = data;

    rb->head = (rb->head + 1U) & (BUFFER_SIZE - 1U);

    rb->count++;

    return RINGBUF_OK;
}

ringbuf_status_t ringbuf_read(ringbuf_t *rb, uint8_t *data)
{
    if (ringbuf_is_empty(rb))
    {
        return RINGBUF_ERROR;
    }

    *data = rb->buffer[rb->tail];

    rb->tail = (rb->tail + 1U) & (BUFFER_SIZE - 1U);

    rb->count--;

    return RINGBUF_OK;
}


uint8_t ringbuf_count(const ringbuf_t *rb)
{
    return rb->count;
}


uint8_t ringbuf_is_full(const ringbuf_t *rb)
{
    return (rb->count == BUFFER_SIZE);
}


uint8_t ringbuf_is_empty(const ringbuf_t *rb)
{
    return (rb->count == 0U);
}

int main(void)
{
    ringbuf_t rb;
    uint8_t data;
    uint8_t i;

    uint8_t first_write_data[BUFFER_SIZE] =
    {
        0x41, 0x42, 0x43, 0x44,
        0x45, 0x46, 0x47, 0x48
    };

    uint8_t second_write_data[3] =
    {
        0x49, 0x4A, 0x4B
    };

    ringbuf_init(&rb);

    for (i = 0U; i < BUFFER_SIZE; i++)
    {
        if (ringbuf_write(&rb, first_write_data[i]) == RINGBUF_OK)
        {
            printf("[WRITE] 0x%02X -> OK  (count=%u)",
                   first_write_data[i],
                   ringbuf_count(&rb));

            if (ringbuf_is_full(&rb))
            {
                printf(" FULL");
            }

            printf("\n");
        }
    }


    if (ringbuf_write(&rb, 0x99U) == RINGBUF_ERROR)
    {
        printf("[WRITE] 0x99 -> FAIL (buffer full)\n");
    }


    for (i = 0U; i < 3U; i++)
    {
        if (ringbuf_read(&rb, &data) == RINGBUF_OK)
        {
            printf("[READ]        -> 0x%02X  (count=%u)\n",
                   data,
                   ringbuf_count(&rb));
        }
    }


    for (i = 0U; i < 3U; i++)
    {
        if (ringbuf_write(&rb, second_write_data[i]) == RINGBUF_OK)
        {
            printf("[WRITE] 0x%02X -> OK  (count=%u)",
                   second_write_data[i],
                   ringbuf_count(&rb));

            if (ringbuf_is_full(&rb))
            {
                printf(" FULL");
            }

            printf("\n");
        }
    }

    while (!ringbuf_is_empty(&rb))
    {
        if (ringbuf_read(&rb, &data) == RINGBUF_OK)
        {
            printf("[READ]        -> 0x%02X  (count=%u)\n",
                   data,
                   ringbuf_count(&rb));
        }
    }

    if (ringbuf_read(&rb, &data) == RINGBUF_ERROR)
    {
        printf("[READ]  (empty) -> FAIL (buffer empty)\n");
    }

    return 0;
}
