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
    time_t   *stop_start_time      = NULL;
    time_t   *servicing_start_time = NULL;

    elevio_init();
    while (elevio_floorSensor() == -1)
    {
        elevio_motorDirection(DIRN_DOWN);
    }

    elevator->m_state         = IDLE_CLOSED;
    elevator->m_direction     = DIRN_DOWN;
    elevator->m_current_floor = elevio_floorSensor();
    elevator->m_door_open     = 0;

    elevio_motorDirection(DIRN_STOP);
    elevio_floorIndicator(elevator->m_current_floor);
    elevio_doorOpenLamp(0);
    elevio_stopLamp(0);

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
                if (button_pressed && elevator->m_state != STOPPED)
                {
                    Order new_order;
                    new_order.m_floor     = floor;
                    new_order.m_direction = button;
                    switch (button)
                    {
                        case BUTTON_HALL_UP:
                            new_order.m_direction = DIRN_UP;
                            add_order_back(new_order, queue);
                            break;
                        case BUTTON_HALL_DOWN:
                            new_order.m_direction = DIRN_DOWN;
                            add_order_back(new_order, queue);
                            break;
                        case BUTTON_CAB:
                            new_order.m_direction = DIRN_STOP;
                            add_order_front(new_order, queue);
                            break;
                    }
                }
            }
        }

        switch (elevator->m_state)
        {
            case MOVING:
                if (elevio_stopButton())
                {
                    stop_elevator(elevator, queue);
                }

                if (elevio_floorSensor() != -1
                    && elevio_floorSensor() != elevator->m_current_floor)
                {
                    printf("at new floor: %d\n", elevio_floorSensor());
                    elevator->m_current_floor = elevio_floorSensor();
                    printf("set new floor: %d\n", elevator->m_current_floor);
                    elevio_floorIndicator(elevator->m_current_floor);
                    if (check_orders(elevator, queue))
                    {
                        elevio_motorDirection(DIRN_STOP);
                        elevator->m_state     = SERVICING;
                        elevator->m_door_open = 1;
                        elevio_doorOpenLamp(1);
                    }
                }
                break;
            case STOPPED:
                if (elevio_stopButton())
                {
                    stop_start_time = NULL;
                    elevio_stopLamp(1);
                }
                else if (elevio_floorSensor() != -1)
                {
                    elevio_stopLamp(0);
                    if (stop_start_time == NULL)
                    {
                        stop_start_time  = (time_t *)malloc(sizeof(time_t));
                        *stop_start_time = time(0);
                    }
                    else
                    {
                        double time_elapsed
                            = ((double)(time(0) - *stop_start_time));
                        if (time_elapsed >= 3)
                        {
                            if (elevio_obstruction())
                            {
                                *stop_start_time = time(0);
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
                    if (elevator->m_current_floor == N_FLOORS - 1)
                    {
                        return_to_story_order.m_floor     = N_FLOORS - 2;
                        return_to_story_order.m_direction = DIRN_DOWN;
                    }
                    else if (elevator->m_current_floor == 0)
                    {
                        return_to_story_order.m_floor     = 1;
                        return_to_story_order.m_direction = DIRN_UP;
                    }
                    else
                    {
                        return_to_story_order.m_floor
                            = elevator->m_current_floor - 1;
                        return_to_story_order.m_direction = DIRN_DOWN;
                    }

                    add_order_front(return_to_story_order, queue);
                }
                break;

            case IDLE_CLOSED:
                if (elevio_stopButton())
                {
                    stop_elevator(elevator, queue);
                }
                else if (queue->m_start != NULL)
                {
                    printf("non empty queue\n");
                    if (elevio_floorSensor() != -1)
                    {
                        if (check_orders(elevator, queue))
                        {
                            elevator->m_state     = SERVICING;
                            elevator->m_door_open = 1;
                            elevio_doorOpenLamp(1);
                        }
                    }

                    if (elevator->m_current_floor
                        > queue->m_start->m_order.m_floor)
                    {
                        printf("lets go down\n");
                        elevator->m_state     = MOVING;
                        elevator->m_direction = DIRN_DOWN;
                        elevio_motorDirection(DIRN_DOWN);
                    }

                    else if (elevator->m_current_floor
                             < queue->m_start->m_order.m_floor)
                    {
                        printf("lets go up\n");
                        elevator->m_state     = MOVING;
                        elevator->m_direction = DIRN_UP;
                        elevio_motorDirection(DIRN_UP);
                    }
                }
                break;

            case SERVICING:
                if (elevio_stopButton())
                {
                    stop_elevator(elevator, queue);
                }
                else if (servicing_start_time == NULL)
                {
                    printf("starting servicing timer\n");
                    servicing_start_time  = (time_t *)malloc(sizeof(time_t));
                    *servicing_start_time = time(0);
                }
                else
                {
                    double time_elapsed
                        = ((double)(time(0) - *servicing_start_time));
                    printf("time elapsed: %f\n", time_elapsed);
                    if (time_elapsed >= 3.0)
                    {
                        printf("nuff time has elapsed\n");
                        if (elevio_obstruction())
                        {
                            *servicing_start_time = time(0);
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

    delete_all_orders(queue);
    free(elevator);
    free(queue);
    free(stop_start_time);
    free(servicing_start_time);

    return 0;
}
