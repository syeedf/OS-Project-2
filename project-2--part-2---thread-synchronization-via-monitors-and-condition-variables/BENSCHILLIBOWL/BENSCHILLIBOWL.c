#include "BENSCHILLIBOWL.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

bool IsEmpty(BENSCHILLIBOWL* mcg);
bool IsFull(BENSCHILLIBOWL* mcg);
void AddOrderToBack(Order **orders, Order *order);

MenuItem BENSCHILLIBOWLMenu[] = { 
    "BensChilli", 
    "BensHalfSmoke", 
    "BensHotDog", 
    "BensChilliCheeseFries", 
    "BensShake",
    "BensHotCakes",
    "BensCake",
    "BensHamburger",
    "BensVeggieBurger",
    "BensOnionRings"
};
int BENSCHILLIBOWLMenuLength = 10;

/* Select a random item from the Menu and return it */
MenuItem PickRandomMenuItem() {
		MenuItem random; 
    srand(time(NULL));
    random = BENSCHILLIBOWLMenu[rand() % BENSCHILLIBOWLMenuLength];
    return random;
}

/* Allocate memory for the Restaurant, then create the mutex and condition variables needed to instantiate the Restaurant */

BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
    printf("Restaurant is open!\n");
    BENSCHILLIBOWL *restaurant = malloc(sizeof(BENSCHILLIBOWL));
    restaurant->orders= NULL;
    restaurant->current_size=0;
    restaurant->max_size=max_size;
    restaurant->next_order_number=1;
    restaurant->orders_handled=0;
    restaurant->expected_num_orders= expected_num_orders;
    
    pthread_mutex_init(&(restaurant->mutex), NULL);
    pthread_cond_init(&(restaurant->can_add_orders),NULL);
    pthread_cond_init(&(restaurant->can_get_orders),NULL);
    return restaurant;
  
}


/* check that the number of orders received is equal to the number handled (ie.fullfilled). Remember to deallocate your resources */

void CloseRestaurant(BENSCHILLIBOWL* mcg) {
    if(mcg->expected_num_orders== mcg->orders_handled)
    {
      pthread_mutex_destroy(&(mcg->mutex));
      pthread_cond_destroy(&(mcg->can_add_orders));
      pthread_cond_destroy(&(mcg->can_get_orders));
    }
    free(mcg);
    printf("Restaurant is closed!\n");
    
}

// Optional helper functions (you can implement if you think they would be useful)
bool IsEmpty(BENSCHILLIBOWL* mcg) {
  return mcg->current_size==0;
}

bool IsFull(BENSCHILLIBOWL* mcg) {
  return mcg->current_size==mcg->max_size;
}
/* add an order to the back of queue */
int AddOrder(BENSCHILLIBOWL* mcg, Order* order) {
  pthread_mutex_lock(&(mcg->mutex));
  while(IsFull(mcg))
  {
    pthread_cond_wait(&(mcg->can_add_orders), &(mcg->mutex));
  }
  mcg->current_size= mcg-> current_size+ 1;
  order->order_number = mcg->next_order_number +1;
  AddOrderToBack(&(mcg->orders), order);
  pthread_cond_signal(&(mcg->can_get_orders));
  pthread_mutex_unlock(&(mcg->mutex));
  return order-> order_number;
}

/* remove an order from the queue */
Order *GetOrder(BENSCHILLIBOWL* mcg) {
  pthread_mutex_lock(&(mcg->mutex));
  while(IsEmpty(mcg))
  {
    pthread_cond_wait(&(mcg->can_get_orders), &(mcg->mutex));
  }
  Order *ord = mcg->orders;
  mcg->orders= mcg->orders->next;
  mcg->current_size= mcg->current_size-1;
  mcg->orders_handled= mcg->orders_handled+1;
  pthread_cond_signal(&(mcg->can_get_orders));
  if(mcg->orders_handled==mcg->expected_num_orders)
  {
    pthread_cond_broadcast(&(mcg->can_add_orders));
  }
  pthread_mutex_unlock(&(mcg->mutex));
  return ord;
}


/* this methods adds order to rear of queue */
void AddOrderToBack(Order **orders, Order *order) {
    if(!orders[0])
    {
      orders[0] =order;
    }
    Order *temp = orders[0];
    while(temp->next != NULL)
    {
      temp = temp->next;
    }
    temp->next = order;
    order->next=NULL;
    return; 
}


