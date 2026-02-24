enum ElevatorState
{
    IDLE_CLOSED,
    IDLE_OPEN,
    MOVING_UP,
    MOVING_DOWN
};


typedef struct Elevator
{
    enum ElevatorState m_state;
    int m_current_floor;
    
}Elevator;

void Service(Elevator* elevator);
