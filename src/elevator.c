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

bool
add_order_front (Order s_order, Queue *s_queue)
{
    Elevator s_elevator;
    s_elevator.m_current_floor = s_order.m_floor;
    s_elevator.m_direction     = s_order.m_direction;
    if (!check_orders(&s_elevator, s_queue))
    {
        DllNode *s_new_node = (DllNode *)malloc(sizeof(DllNode));
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
        return true;
    }
    return false;
}

bool
add_order_back (Order s_order, Queue *s_queue)
{
    Elevator s_elevator;
    s_elevator.m_current_floor = s_order.m_floor;
    s_elevator.m_direction     = s_order.m_direction;
    if (!check_orders(&s_elevator, s_queue))
    {
        DllNode *s_new_node = (DllNode *)malloc(sizeof(DllNode));
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
        return true;
    }
    return false;
}

void
remove_order (DllNode *s_order_dll_node, Queue *s_queue)
{
    DllNode *s_prev_node = s_order_dll_node->m_prev;
    DllNode *s_next_node = s_order_dll_node->m_next;

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
    DllNode *current_node = s_queue->m_start;

    while (current_node != NULL)
    {
        if (s_elevator->m_current_floor == s_queue->m_start->m_order.m_floor)
        {
            return true;
        }
        else if (current_node->m_order.m_direction == DIRN_STOP
                 && current_node->m_order.m_floor
                        == s_elevator->m_current_floor)
        {
            return true;
        }
        else if (current_node->m_order.m_direction == s_elevator->m_direction
                 && current_node->m_order.m_floor
                        == s_elevator->m_current_floor)
        {
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
                break;
            }

            current_node = current_node->m_next;
            remove_order(current_node->m_prev, s_queue);
            continue;
        }

        current_node = current_node->m_next;
    }
    for (int button = 0; button < N_BUTTONS; button++)
    {
        elevio_buttonLamp(s_elevator->m_current_floor, button, 0);
    }
}

void
delete_all_orders (Queue *s_queue)
{
    while (s_queue->m_start != NULL)
    {
        remove_order(s_queue->m_start, s_queue);
    }
    turn_off_all_button_lights();
}

void
turn_off_all_button_lights ()
{

    for (int floor = 0; floor < N_FLOORS; floor++)
    {
        for (int button = 0; button < N_BUTTONS; button++)
        {
            elevio_buttonLamp(floor, button, 0);
        }
    }
}
