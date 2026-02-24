
/**
 * @file queue.h
 * @brief Queue system for managing elevator orders using a doubly linked list.
 */

#include "elevator.h"

/**
 * @brief Represents the direction of an elevator order.
 */
enum Direction
{
    UP,       /**< Order going up. */
    DOWN,     /**< Order going down. */
    INTERNAL, /**< Internal cabin order (no direction). */
};

/**
 * @brief Represents a single elevator order.
 */
typedef struct
{
    Direction m_direction; /**< Direction associated with the order. */
    int       m_floor;     /**< Target floor for the order. */
} Order;

/**
 * @brief Node in a doubly linked list used by the queue system.
 */
typedef struct
{
    DllNode *m_prev;  /**< Pointer to the previous node. */
    DllNode *m_next;  /**< Pointer to the next node. */
    Order    m_value; /**< The order stored in this node. */
} DllNode;

/**
 * @brief Queue implemented as a doubly linked list of orders.
 */
typedef struct
{
    DllNode *m_start; /**< Pointer to the first node in the queue. */
    DllNode *m_stop;  /**< Pointer to the last node in the queue. */
} Queue;

/**
 * @brief Adds an order to the front of the queue.
 *
 * @param s_order The order to add.
 * @param queue   Pointer to the queue.
 */
void add_order_front(Order s_order, Queue *queue);

/**
 * @brief Adds an order to the back of the queue.
 *
 * @param s_order The order to add.
 * @param queue   Pointer to the queue.
 */
void add_order_back(Order s_order, Queue *queue);

/**
 * @brief Removes a specific order node from the queue and frees its memory.
 *
 * @param s_order Pointer to the order node to remove.
 * @param queue   Pointer to the queue.
 */
void remove_order(Order *s_order, Queue *queue);

/**
 * @brief Checks whether any queued order matches the current floor and elevator
 * direction.
 *
 * @param current_floor The floor the elevator is currently on.
 * @param state         The current state of the elevator.
 * @param queue         Pointer to the queue.
 * @return true  if a matching order exists, false otherwise.
 */
bool check_orders(int current_floor, ElevatorState state, Queue *queue);

/**
 * @brief Deletes all orders that match the current floor and elevator movement
 * direction.
 *
 * @param current_floor The floor the elevator is currently on.
 * @param state         The current state of the elevator.
 * @param queue         Pointer to the queue.
 */
void
delete_orders (int current_floor, ElevatorState state; Queue * queue)
{
