
#include "elevator.h"

enum Direction {
  UP,
  DOWN,
  INTERNAL,
};

typedef struct {
  Direction m_direction;
  int m_floor;
} Order;

// node struct for double linked list that will be used for the queue system
typedef struct {
  DllNode* m_prev;
  DllNode* m_next;
  Order m_value;
} DllNode;

// sturct for the queue system
typedef struct {
  DllNode* m_start;
  DllNode* m_stop;
} Queue;

// add an order to the front of the queue
void add_order_front(Order s_order, Queue* queue);

// add an order to the back of the queue
void add_order_back(Order s_order, Queue* queue);

// remove an order from the queue
void remove_order(Order* s_order, Queue* queue);

// check if there are orders in the current story that could be serviced and return a Queue struct with those objects
bool check_orders(int current_floor, ElevatorState state, Queue* queue);

// delete all orders that match the current story and movement direction
void delete_orders(int current_floor, ElevatorState state; Queue* queue) {
