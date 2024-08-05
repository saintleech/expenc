#include <stdlib.h>
#include <stdio.h>

#define FORMATTED_AMOUNT_MAX_LEN 24

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

void      listMovements(Movement* movements);
Movement* createMovement(MovementType type, float amount);
void      addMovement(Movement** movements, MovementType type, float amount);

float getMovementSum(Movement* movements);

int main()
{
  Movement* movements = NULL;

  addMovement(&movements, PROFIT, 10.f);
  addMovement(&movements, LOSS,   10.f);
  addMovement(&movements, LOSS,   10.f);
  addMovement(&movements, PROFIT, 10.f);
  addMovement(&movements, PROFIT, 10.f);
  listMovements(movements);

  float sum = getMovementSum(movements);
  printf("-- Sum: %s\n", formatMovementAmount(sum));

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

void listMovements(Movement* movements)
{
  Movement* head = movements;
  if (head == NULL) return;

  while (head != NULL)
  {
    printf(
      "%-6s %s %p\n",
      formatMovementType(head->type),
      formatMovementAmount(head->amount),
      head->next
    );
    head = head->next;
  }
}

Movement* createMovement(MovementType type, float amount)
{
  Movement* movement = (Movement*)malloc(sizeof(Movement));
  movement->type = type;
  movement->amount = amount;
  movement->next = NULL;
  return movement;
}

void addMovement(Movement** movements, MovementType type, float amount)
{
  Movement* newMovement = createMovement(type, amount);

  if (*movements == NULL)
  {
    *movements = newMovement;
    return;
  }

  Movement* head = *movements;
  while (head->next != NULL)
  {
    head = head->next;
  }
  head->next = newMovement;
}

float getMovementSum(Movement* movements)
{
  float sum = 0.f;
  Movement* head = movements;

  if (head == NULL) return sum;

  while (head != NULL)
  {
    sum = head->type == PROFIT ? (sum + head->amount) : (sum - head->amount);
    head = head->next;
  }
  return sum;
}
