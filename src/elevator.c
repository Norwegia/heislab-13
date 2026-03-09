/**
 * @file elevator.c
 * @brief implementation of utility functions in elevator.h
 */
#include "elevator.h"
#include <stdlib.h>
#include <stdio.h>

void
move_elevator_to_defined_state (Elevator *s_elevator)
{
    while (elevio_floorSensor() == -1)
    {
        elevio_motorDirection(DIRN_DOWN);
    }

    s_elevator->m_state         = IDLE_CLOSED;
    s_elevator->m_direction     = DIRN_DOWN;
    s_elevator->m_current_floor = elevio_floorSensor();
    s_elevator->m_door_open     = 0;

    elevio_motorDirection(DIRN_STOP);
    elevio_floorIndicator(s_elevator->m_current_floor);
    elevio_doorOpenLamp(0);
    elevio_stopLamp(0);
}

void
stop_elevator (Elevator *s_elevator, Queue *s_queue)
{

    elevio_motorDirection(DIRN_STOP);
    elevio_stopLamp(1);
    s_elevator->m_state = STOPPED;
    delete_all_orders(s_queue);
    if (elevio_floorSensor() == -1)
    {
        s_elevator->m_door_open = 0;
        elevio_doorOpenLamp(0);
    }
    else
    {
        s_elevator->m_door_open = 1;
        elevio_doorOpenLamp(1);
    }
}

void
add_order_front (Order s_order, Queue *s_queue)
{
    Elevator s_elevator;
    s_elevator.m_current_floor = s_order.m_floor;
    s_elevator.m_direction     = s_order.m_direction;
    printf("adding order\n");
    if (!check_orders(&s_elevator, s_queue))
    {
        DllNode *s_new_node = (DllNode *)malloc(sizeof(DllNode));
        printf("order not already in elevator\n");
        s_new_node->m_prev  = NULL;
        s_new_node->m_next  = s_queue->m_start;
        s_new_node->m_order = s_order;
        if (s_queue->m_start != NULL)
        {
            s_queue->m_start->m_prev = s_new_node;
        }
        s_queue->m_start = s_new_node;
        if (s_queue->m_stop == NULL)
        {
            s_queue->m_stop = s_new_node;
        }
    }
}

void
add_order_back (Order s_order, Queue *s_queue)
{
    Elevator s_elevator;
    s_elevator.m_current_floor = s_order.m_floor;
    s_elevator.m_direction     = s_order.m_direction;
    printf("adding order\n");
    if (!check_orders(&s_elevator, s_queue))
    {
        DllNode *s_new_node = (DllNode *)malloc(sizeof(DllNode));
        printf("order not already in elevator\n");
        s_new_node->m_next  = NULL;
        s_new_node->m_prev  = s_queue->m_stop;
        s_new_node->m_order = s_order;
        if (s_queue->m_stop != NULL)
        {
            s_queue->m_stop->m_next = s_new_node;
        }
        s_queue->m_stop = s_new_node;
        if (s_queue->m_start == NULL)
        {
            s_queue->m_start = s_new_node;
        }
    }
}

void
remove_order (DllNode *s_order_dll_node, Queue *s_queue)
{
    DllNode *s_prev_node = s_order_dll_node->m_prev;
    DllNode *s_next_node = s_order_dll_node->m_next;

    printf("previos node: %p\n", s_prev_node);
    printf("next node: %p\n", s_next_node);

    if (s_prev_node == NULL)
    {
        if (s_next_node != NULL)
        {
            s_next_node->m_prev = NULL;
            s_queue->m_start    = s_next_node;
        }
        else
        {
            s_queue->m_start = NULL;
            s_queue->m_stop  = NULL;
        }
    }

    if (s_next_node == NULL)
    {
        if (s_prev_node != NULL)
        {
            s_prev_node->m_next = NULL;
            s_queue->m_stop     = s_prev_node;
        }
        else
        {
            s_queue->m_start = NULL;
            s_queue->m_stop  = NULL;
        }
    }
    else if (s_prev_node != NULL)
    {
        s_prev_node->m_next = s_next_node;
        s_next_node->m_prev = s_prev_node;
    }

    free(s_order_dll_node);
    s_order_dll_node = NULL;
}

bool
check_orders (Elevator *s_elevator, Queue *s_queue)
{
    printf("checking orders\n");
    DllNode *current_node = s_queue->m_start;

    while (current_node != NULL)
    {
        printf("current node: %p\n", current_node);
        if (s_elevator->m_current_floor == s_queue->m_start->m_order.m_floor)
        {
            printf("reached target order\n");
            return true;
        }
        else if (current_node->m_order.m_direction == s_elevator->m_direction
                 && current_node->m_order.m_floor
                        == s_elevator->m_current_floor)
        {
            printf("found matching order at floor %d going in direction %d\n",
                   current_node->m_order.m_floor,
                   current_node->m_order.m_direction);
            return true;
        }

        current_node = current_node->m_next;
    }

    return false;
}

void
delete_serviced_orders (Elevator *s_elevator, Queue *s_queue)
{
    DllNode *current_node = s_queue->m_start;

    while (current_node != NULL)
    {

        if (current_node->m_order.m_floor == s_elevator->m_current_floor)
        {
            if (current_node->m_next == NULL)
            {
                remove_order(current_node, s_queue);
                return;
            }

            current_node = current_node->m_next;
            remove_order(current_node->m_prev, s_queue);
            continue;
        }

        current_node = current_node->m_next;
    }
}

void
delete_all_orders (Queue *s_queue)
{
    while (s_queue->m_start != NULL)
    {
        remove_order(s_queue->m_start, s_queue);
    }
}
