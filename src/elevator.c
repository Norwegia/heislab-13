/**
 * @file elevator.c
 * @brief implementation of utility functions in elevator.h
 */
#include "elevator.h"
#include <stdlib.h>
#include <stdio.h>

void
add_order_front (Order s_order, Queue *s_queue)
{
    DllNode *s_new_node = (DllNode *)malloc(sizeof(DllNode));
    s_new_node->m_prev  = NULL;
    s_new_node->m_next  = s_queue->m_start;
    s_new_node->m_order = s_order;
    s_queue->m_start    = s_new_node;
}

void
add_order_back (Order s_order, Queue *s_queue)
{
    DllNode *s_new_node = (DllNode *)malloc(sizeof(DllNode));
    s_new_node->m_next  = NULL;
    s_new_node->m_prev  = s_queue->m_stop;
    s_new_node->m_order = s_order;
    s_queue->m_stop     = s_new_node;
}

void
remove_order (DllNode *s_order_dll_node, Queue *s_queue)
{
    DllNode *s_prev_node = s_order_dll_node->m_prev;
    DllNode *s_next_node = s_order_dll_node->m_next;

    if (s_order_dll_node->m_prev == NULL)
    {
        s_queue->m_start = s_order_dll_node->m_next;
    }

    if (s_order_dll_node->m_next == NULL)
    {
        s_queue->m_stop = s_order_dll_node->m_prev;
    }

    free(s_order_dll_node);
    s_order_dll_node = NULL;

    s_prev_node->m_next = s_next_node;
    s_next_node->m_prev = s_prev_node;
}

bool
check_orders (int current_floor, ElevatorState state, Queue *s_queue)
{
    DllNode  *current_node = s_queue->m_start;
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

        if (current_node->m_order.m_direction == direction
            && current_node->m_order.m_floor == current_floor)
        {
            return true;
        }

        current_node = current_node->m_next;
    }

    return false;
}

void
delete_orders (int current_floor, ElevatorState state, Queue *s_queue)
{
    DllNode  *current_node = s_queue->m_start;
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

        if (current_node->m_order.m_direction == direction
            && current_node->m_order.m_floor == current_floor)
        {
            current_node = current_node->m_next;
            remove_order(current_node->m_prev, s_queue);
        }

        current_node = current_node->m_next;
    }
}

Elevator elevator;

void
Determine_Movement (Elevator *elevator, Queue *queue)
{

    if (queue->m_start == NULL)
    {
        elevator->m_state = IDLE_CLOSED;
    }

    else if (elevator->m_current_floor < queue->m_start->m_order.m_floor)
    {
        elevator->m_state = MOVING_UP;
    }

    else if (elevator->m_current_floor > queue->m_start->m_order.m_floor)
    {
        elevator->m_state = MOVING_DOWN;
    }

    else if (elevator->m_current_floor == queue->m_start->m_order.m_floor
             || check_orders(
                 elevator->m_current_floor, elevator->m_state, queue))
    {
        Service(elevator, queue);
    }
}

void
Service (Elevator *s_elevator, Queue *s_queue)
{
}
