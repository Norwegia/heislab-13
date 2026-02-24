
/**
 * @file queue.h
 * @brief Queue system for managing elevator orders using a doubly linked list.
 */

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void
add_order_front (Order s_order, Queue *queue)
{
    DllNode *s_new_node = (DllNode *)malloc(sizeof(DllNode));
    s_new_node->m_prev  = NULL;
    s_new_node->m_next  = queue->m_start;
    s_new_node->m_value = s_order;
    queue->m_start      = s_new_node;
}

void
add_order_back (Order s_order, Queue *queue)
{
    DllNode *s_new_node = (DllNode *)malloc(sizeof(DllNode));
    s_new_node->m_next  = NULL;
    s_new_node->m_prev  = queue->m_stop;
    s_new_node->m_value = s_order;
    queue->m_stop       = s_new_node;
}

void
remove_order (Order *s_order, Queue *queue)
{
    DllNode *s_prev_node = s_order->m_prev;
    DllNode *s_next_node = s_order->m_next;

    if (s_order->m_prev == NULL)
    {
        queue->m_start = s_order->m_next;
    }

    if (s_order->m_next == NULL)
    {
        queue->m_stop = s_order->m_prev;
    }

    free(s_order);
    s_order = NULL;

    s_prev_node->m_next = s_next_node;
    s_next_node->m_prev = s_prev_node;
}

bool
check_orders (int current_floor, ElevatorState state, Queue * queue)
{
    DllNode  *current_node = queue->start;
    Direction direction;

    switch (state)
    {
        case MOVING_UP:
            direction = UP;
            break;
        case MOVING_DOWN:
            direction = DOWN;
            break;
    }

    while (current_node != NULL)
    {

        if (current_node->m_value.m_direction == direction
            && current_node->m_value.m_floor == current_floor)
        {
            return true;
        }

        current_node = current_node->m_next;
    }

    return false;
}

void
delete_orders (int current_floor, ElevatorState state; Queue * queue)
{
    DllNode  *current_node = queue->start;
    Direction direction;

    switch (state)
    {
        case MOVING_UP:
            direction = UP;
            break;
        case MOVING_DOWN:
            direction = DOWN;
            break;
    }

    while (current_node != NULL)
    {

        if (current_node->m_value.m_direction == direction
            && current_node->m_value.m_floor == current_floor)
        {
            current_node = current_node->m_next;
            remove_order(current_node->m_prev, queue)
        }

        current_node = current_node->m_next;
    }

    return false;
}
}
