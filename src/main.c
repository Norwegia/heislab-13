#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "driver/elevio.h"

#include "elevator.h"

int
main ()
{

    Elevator *elevator             = (Elevator *)malloc(sizeof(Elevator));
    Queue    *queue                = (Queue *)malloc(sizeof(Queue));
    clock_t  *stop_start_time      = NULL;
    clock_t  *servicing_start_time = NULL;

    elevio_init();
    while (elevio_floorSensor() == -1)
    {
        elevio_motorDirection(DIRN_DOWN);
    }
    elevio_motorDirection(DIRN_STOP);
    elevator->m_state         = IDLE_CLOSED;
    elevator->m_direction     = DIRN_DOWN;
    elevator->m_current_floor = elevio_floorSensor();
    elevator->m_door_open     = 0;

    queue->m_start = NULL;
    queue->m_stop  = NULL;

    while (1)
    {

        for (int floor = 0; floor < N_FLOORS; floor++)
        {
            for (int button = 0; button < N_BUTTONS; button++)
            {
                int button_pressed = elevio_callButton(floor, button);
                elevio_buttonLamp(floor, button, button_pressed);
                if (button_pressed)
                {
                    Order new_order;
                    new_order.m_floor = floor;
                    new_order.m_direction
                        = saturate(-1, 1, floor - elevator->m_current_floor);
                    if (button == BUTTON_CAB)
                    {
                        add_order_front(new_order, queue);
                    }
                    else
                    {
                        add_order_back(new_order, queue);
                    }
                }
            }
        }

        switch (elevator->m_state)
        {
            case MOVING:
                if (elevio_stopButton())
                {
                    elevio_motorDirection(DIRN_STOP);
                    elevio_stopLamp(1);
                    elevator->m_state = STOPPED;
                    delete_all_orders(queue);
                    if (elevio_floorSensor() == -1)
                    {
                        elevator->m_door_open = 0;
                        elevio_doorOpenLamp(0);
                    }
                    else
                    {
                        elevator->m_door_open = 1;
                        elevio_doorOpenLamp(1);
                    }
                }

                if (elevio_floorSensor() != elevator->m_current_floor)
                {
                    elevator->m_current_floor = elevio_floorSensor();
                    elevio_floorIndicator(elevator->m_current_floor);
                    if (check_orders(elevator, queue))
                    {
                        elevio_motorDirection(DIRN_STOP);
                        elevator->m_state = SERVICING;
                    }
                }
                break;
            case STOPPED:
                if (elevio_stopButton)
                {
                    stop_start_time = NULL;
                    elevio_stopLamp(1);
                }
                else if (elevio_floorSensor != -1)
                {
                    elevio_stopLamp(0);
                    if (stop_start_time == NULL)
                    {
                        stop_start_time  = (clock_t *)malloc(sizeof(clock_t));
                        *stop_start_time = clock();
                    }
                    else
                    {
                        double time_elapsed
                            = ((double)(clock() - *stop_start_time))
                              / CLOCKS_PER_SEC;
                        if (time_elapsed >= 3)
                        {
                            if (elevio_obstruction)
                            {
                                *stop_start_time = clock();
                            }
                            else
                            {
                                stop_start_time       = NULL;
                                elevator->m_state     = IDLE_CLOSED;
                                elevator->m_door_open = 0;
                                elevio_doorOpenLamp(0);
                                free(stop_start_time);
                                stop_start_time = NULL;
                            }
                        }
                    }
                }
                else
                {
                    elevio_stopLamp(0);
                    elevator->m_state = IDLE_CLOSED;
                    Order return_to_story_order;
                    return_to_story_order.m_floor = elevator->m_current_floor;
                    return_to_story_order.m_direction
                        = elevator->m_direction * -1;
                    add_order_front(return_to_story_order, queue);
                }
                break;

            case IDLE_CLOSED:
                if (elevio_stopButton())
                {
                    elevio_motorDirection(DIRN_STOP);
                    elevio_stopLamp(1);
                    elevator->m_state = STOPPED;
                    delete_all_orders(queue);
                    if (elevio_floorSensor() == -1)
                    {
                        elevator->m_door_open = 0;
                        elevio_doorOpenLamp(0);
                    }
                    else
                    {
                        elevator->m_door_open = 1;
                        elevio_doorOpenLamp(1);
                    }
                }
                else if (queue->m_start != NULL)
                {
                    if (elevio_floorSensor() != -1)
                    {
                        if (check_orders(elevator, queue))
                        {
                            elevator->m_state     = SERVICING;
                            elevator->m_door_open = 1;
                            elevio_doorOpenLamp(1);
                        }
                    }

                    else if (elevator->m_current_floor
                             > queue->m_start->m_order.m_floor)
                    {
                        elevator->m_state     = MOVING;
                        elevator->m_direction = DIRN_DOWN;
                        elevio_motorDirection(DIRN_DOWN);
                    }

                    else if (elevator->m_current_floor
                             < queue->m_start->m_order.m_floor)
                    {
                        elevator->m_state     = MOVING;
                        elevator->m_direction = DIRN_UP;
                        elevio_motorDirection(DIRN_UP);
                    }
                }
                break;

            case SERVICING:
                if (elevio_stopButton())
                {
                    servicing_start_time = NULL;
                    elevator->m_state    = STOPPED;
                    delete_all_orders(queue);
                    if (elevio_floorSensor() == -1)
                    {
                        elevator->m_door_open = 0;
                        elevio_doorOpenLamp(0);
                    }
                    else
                    {
                        elevator->m_door_open = 1;
                        elevio_doorOpenLamp(1);
                    }
                }
                else if (servicing_start_time == NULL)
                {
                    servicing_start_time  = (clock_t *)malloc(sizeof(clock_t));
                    *servicing_start_time = clock();
                }
                else
                {
                    double time_elapsed = ((double)(clock() - *stop_start_time))
                                          / CLOCKS_PER_SEC;
                    if (time_elapsed >= 3)
                    {
                        if (elevio_obstruction())
                        {

                            *servicing_start_time = clock();
                        }
                        else
                        {
                            servicing_start_time  = NULL;
                            elevator->m_state     = IDLE_CLOSED;
                            elevator->m_door_open = 0;
                            elevio_doorOpenLamp(0);
                            free(servicing_start_time);
                            servicing_start_time = NULL;
                            delete_serviced_orders(elevator, queue);
                        }
                    }
                }
                break;
        }
    }
    return 0;
}
