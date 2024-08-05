#include <stdlib.h>
#include <stdio.h>

typedef enum MovementType
{
  PROFIT = 0,
  LOSS,
} MovementType;

typedef struct Movement
{
  MovementType type;
  float amount;
  struct Movement* next;
} Movement;

int main()
{
  Movement movement =
  {
    .type = PROFIT,
    .amount = 10.f,
    .next = NULL,
  };
  printf("%d %f %p\n", movement.type, movement.amount, movement.next);

  return EXIT_SUCCESS;
}
