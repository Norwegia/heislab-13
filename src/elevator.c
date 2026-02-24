#include "elevator.h"
#include <stdlib.h>


Elevator elevator;





void Determine_Movement(Elevator* elevator, Queue* queue)
{
    
    if(queue->m_start == NULL) {IDLE_CLOSED}
    
    elif(elevator->m_current_floor < queue->m_start->m_value);
    {elevator->m_state = MOVING_UP;}

    elif(elevator->m_current_floor > queue->m_start->m_value);
    {elevator->m_state = MOVING_DOWN;}

    elif(elevator->m_current_floor == queue->m_start->m_value
    || check_orders(elevator->m_current_floor, elevator->m_state)); 
    {Service(elevator, queue);}
}

void Service(Elevator* elevator, Queue* queue) 
{

}  

