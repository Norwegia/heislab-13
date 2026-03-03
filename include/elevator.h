/**
 * @file elevator.h
 * @brief utility functions for managing the elevator and queue
 */

#pragma once
#include <stdbool.h>

/**
 * @brief Represents the operating state of the elevator.
 */
typedef enum
{
    IDLE_CLOSED,
    IDLE_OPEN,
    MOVING_UP,
    MOVING_DOWN
} ElevatorState;

/**
 * @brief Represents the elevator and its current state.
 */
typedef struct Elevator
{
    ElevatorState m_state;
    int           m_current_floor;

} Elevator;

/**
 * @brief Represents the direction of an elevator order.
 */
typedef enum
{
    UP,       /**< Order going up. */
    DOWN,     /**< Order going down. */
    INTERNAL, /**< Internal cabin order (no direction). */
} Direction;

/**
 * @brief Represents a single elevator order.
 */
typedef struct Order
{
    Direction m_direction; /**< Direction associated with the order. */
    int       m_floor;     /**< Target floor for the order. */
} Order;

/**
 * @brief Node in a doubly linked list used by the queue system.
 */
typedef struct DllNode
{
    struct DllNode *m_prev;  /**< Pointer to the previous node. */
    struct DllNode *m_next;  /**< Pointer to the next node. */
    Order           m_order; /**< The order stored in this node. */
} DllNode;

/**
 * @brief Queue implemented as a doubly linked list of orders.
 */
typedef struct Queue
{
    DllNode *m_start; /**< Pointer to the first node in the queue. */
    DllNode *m_stop;  /**< Pointer to the last node in the queue. */
} Queue;

/**
 * @brief Adds an order to the front of the queue.
 *
 * @param s_order The order to add.
 * @param s_queue Pointer to the queue.
 */
void add_order_front(Order s_order, Queue *s_queue);

/**
 * @brief Adds an order to the back of the queue.
 *
 * @param s_order The order to add.
 * @param s_queue Pointer to the queue.
 */
void add_order_back(Order s_order, Queue *s_queue);

/**
 * @brief Removes a specific order node from the queue and frees its memory.
 *
 * @param s_order_dll_node Pointer to the order node to remove.
 * @param s_queue          Pointer to the queue.
 */
void remove_order(DllNode *s_order_dll_node, Queue *s_queue);

/**
 * @brief Checks whether any queued order matches the current floor and elevator
 * direction.
 *
 * @param current_floor The floor the elevator is currently on.
 * @param state         The current state of the elevator.
 * @param s_queue       Pointer to the queue.
 * @return true  if a matching order exists, false otherwise.
 */
bool check_orders(int current_floor, ElevatorState state, Queue *s_queue);

/**
 * @brief Deletes all orders that match the current floor and elevator movement
 * direction.
 *
 * @param current_floor The floor the elevator is currently on.
 * @param state         The current state of the elevator.
 * @param s_queue       Pointer to the queue.
 */
void delete_orders(int current_floor, ElevatorState state, Queue *s_queue);

/**
 * @brief Services the elevator at the current floor, opening doors and
 * removing fulfilled orders from the queue.
 *
 * @param s_elevator Pointer to the elevator.
 * @param s_queue    Pointer to the queue.
 */
void Service(Elevator *s_elevator, Queue *s_queue);
