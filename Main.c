#include <stdlib.h>
#include <stdio.h>

#define FORMATTED_AMOUNT_MAX_LEN 32

const char* CURRENCY_STRING = "$";

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

const char* formatMovementType(MovementType type);
char*       formatMovementAmount(float amount);

int main()
{
  Movement movement =
  {
    .type = PROFIT,
    .amount = 10.f,
    .next = NULL,
  };
  printf(
    "%s %s %p\n",
    formatMovementType(movement.type),
    formatMovementAmount(movement.amount),
    movement.next
  );

  return EXIT_SUCCESS;
}

const char* formatMovementType(MovementType type)
{
  switch (type)
  {
    case PROFIT:
      return "Profit";
    case LOSS:
      return "Loss";
  }
  return "N/A";
}

char* formatMovementAmount(float amount)
{
  static char formattedString[FORMATTED_AMOUNT_MAX_LEN];
  sprintf(formattedString, "%.2f %s", amount, CURRENCY_STRING);
  return formattedString;
}
