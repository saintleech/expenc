#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define FORMATTED_AMOUNT_MAX_LEN 24
#define FORMATTED_TIME_MAX_LEN   24

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
  time_t time;
  struct Movement* next;
} Movement;

time_t getTimeFromString(const char* timeString);
char*  getStringFromTime(time_t* time);

const char* formatMovementType(MovementType type);
char*       formatMovementAmount(float amount);

void      listMovements(Movement* movements);
Movement* createMovement(MovementType type, float amount, time_t time);
void      addMovement(Movement** movements, MovementType type, float amount, time_t time);

float getMovementSum(Movement* movements);

int main()
{
  Movement* movements = NULL;

  addMovement(&movements, PROFIT, 10.f, getTimeFromString("2024-01-01 12:00:00"));
  addMovement(&movements, LOSS,   10.f, getTimeFromString("2024-01-01 12:01:00"));
  addMovement(&movements, LOSS,   10.f, getTimeFromString("2024-01-01 12:02:00"));
  addMovement(&movements, PROFIT, 10.f, getTimeFromString("2024-01-01 12:03:00"));
  addMovement(&movements, PROFIT, 10.f, getTimeFromString("2024-01-01 12:04:00"));
  listMovements(movements);

  float sum = getMovementSum(movements);
  printf("-- Sum: %s\n", formatMovementAmount(sum));

  return EXIT_SUCCESS;
}

time_t getTimeFromString(const char* timeString)
{
  struct tm tm = {0};
  char* result;

  result = strptime(timeString, "%Y-%m-%d %H:%M:%S", &tm);
  if (result == NULL)
  {
    exit(EXIT_FAILURE);
  }
  time_t time = mktime(&tm);
  if (time == -1)
  {
    exit(EXIT_FAILURE);
  }
  return time;
}

char* getStringFromTime(time_t* time)
{
  static char formattedString[FORMATTED_TIME_MAX_LEN];
  struct tm* localTime = localtime(time);
  sprintf(
    formattedString,
    "%d/%02d/%02d %02d:%02d:%02d",
    localTime->tm_year + 1900,
    localTime->tm_mon + 1,
    localTime->tm_mday,
    localTime->tm_hour,
    localTime->tm_min,
    localTime->tm_sec
  );
  return formattedString;
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
      "%-6s %s %s %p\n",
      formatMovementType(head->type),
      formatMovementAmount(head->amount),
      getStringFromTime(&head->time),
      head->next
    );
    head = head->next;
  }
}

Movement* createMovement(MovementType type, float amount, time_t time)
{
  Movement* movement = (Movement*)malloc(sizeof(Movement));
  movement->type = type;
  movement->amount = amount;
  movement->time = time;
  movement->next = NULL;
  return movement;
}

void addMovement(Movement** movements, MovementType type, float amount, time_t time)
{
  Movement* newMovement = createMovement(type, amount, time);

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
