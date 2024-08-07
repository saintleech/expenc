#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define C_RESET  "\033[0m"
#define C_RED    "\033[31m"
#define C_GREEN  "\033[32m"
#define C_YELLOW "\033[33m"
#define C_CYAN   "\033[36m"

#define DB_LINE_MAX_LEN          256
#define LABEL_MAX_LEN            128
#define FORMATTED_AMOUNT_MAX_LEN 24
#define FORMATTED_TIME_MAX_LEN   24

const char* CURRENCY_STRING = "$";

typedef enum MovementType
{
  PROFIT = 0,
  LOSS,
} MovementType;

typedef enum MovementCategory
{
  JOB = 0,
  FOOD,
  TRANSPORT,
  OTHER,
} MovementCategory;

typedef struct Movement
{
  MovementType type;
  MovementCategory category;
  char* label;
  float amount;
  time_t time;
  struct Movement* next;
} Movement;

typedef bool (*MovementFilterFn)(Movement* movement);
typedef bool (*MovementFilterWithParameterFn)(Movement* movement, void* parameter);

typedef uint8_t MovementListingFlag;
const MovementListingFlag SHOW_TYPE     = 1 << 0;
const MovementListingFlag SHOW_CATEGORY = 1 << 1;
const MovementListingFlag SHOW_LABEL    = 1 << 2;
const MovementListingFlag SHOW_AMOUNT   = 1 << 3;
const MovementListingFlag SHOW_TIME     = 1 << 4;

time_t getTimeFromString(const char* timeString);
char*  getStringFromTime(time_t* time);

const char* formatMovementType(MovementType type);
const char* formatMovementCategory(MovementCategory category);
char*       formatMovementAmount(float amount);

Movement* readMovements(const char* filePath);
void      listMovements(Movement* movements);
void      listMovementsWithOptions(Movement* movements, MovementListingFlag flags);
Movement* createMovement(MovementType type, MovementCategory category, char* label, float amount, time_t time);
void      addMovement(Movement** movements, MovementType type, MovementCategory category, char* label, float amount, time_t time);
void      addMovementToDatabase(Movement* movement, const char* filePath);

float getMovementSum(Movement* movements);
float getMovementSum(Movement* movements);

bool isProfit(Movement* movement);
bool isLoss(Movement* movement);
bool isOfCategory(Movement* movement, void* category);

Movement* filterMovements(Movement* movements, MovementFilterFn predicate);
Movement* filterMovementsWithParameter(Movement* movements, MovementFilterWithParameterFn predicate, void* parameter);

int main(int argc, char** argv)
{
  Movement* movements = readMovements("example.expc");
  addMovement(&movements, PROFIT, JOB, "Stole coins from work", 0.52f, getTimeFromString("2024-01-01 12:00:00"));
  addMovement(&movements, LOSS, FOOD, "McDonald's Fries", 1.90f, getTimeFromString("2024-01-01 12:01:00"));
  addMovement(&movements, LOSS, TRANSPORT, "Train to Regensburg", 79.9f, getTimeFromString("2024-01-01 12:02:00"));
  addMovement(&movements, PROFIT, JOB, "Stole a laptop from work", 220.f, getTimeFromString("2024-01-01 12:03:00"));
  addMovement(&movements, PROFIT, JOB, "Salary", 10.f, getTimeFromString("2024-01-01 12:04:00"));
  listMovements(movements);

  float sum = getMovementSum(movements);
  printf("-- Sum: %s\n", formatMovementAmount(sum));
  printf("-- Job Movements\n");

  Movement* jobMovements = filterMovementsWithParameter(
    movements,
    isOfCategory,
    JOB
  );
  listMovements(jobMovements);

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

const char* formatMovementCategory(MovementCategory category)
{
  switch (category)
  {
    case JOB:
      return "Job";
    case FOOD:
      return "Food";
    case TRANSPORT:
      return "Transport";
    case OTHER:
      return "Other";
  }
  return "N/A";
}

char* formatMovementAmount(float amount)
{
  static char formattedString[FORMATTED_AMOUNT_MAX_LEN];
  sprintf(formattedString, "%.2f %s", amount, CURRENCY_STRING);
  return formattedString;
}

Movement* readMovements(const char* filePath)
{
  FILE* movementsFile = fopen(filePath, "r");
  if (movementsFile == NULL)
  {
    return NULL;
  }

  char line[DB_LINE_MAX_LEN];
  Movement* head = NULL;

  while (fgets(line, sizeof(line), movementsFile))
  {
    MovementType type;
    MovementCategory category;
    float amount;
    time_t time;
    char* label = malloc(sizeof(char) * LABEL_MAX_LEN);

    sscanf(
      line,
      "%u;%u;%f;%ld;%[^\n]",
      &type,
      &category,
      &amount,
      &time,
      label
    );
    addMovement(&head, type, category, label, amount, time);
  }

  fclose(movementsFile);
  return head;
}

void listMovements(Movement* movements)
{
  listMovementsWithOptions(
    movements,
    SHOW_TYPE | SHOW_CATEGORY | SHOW_LABEL | SHOW_AMOUNT | SHOW_TIME
  );
}

void listMovementsWithOptions(Movement* movements, MovementListingFlag flags)
{
  Movement* head = movements;
  if (head == NULL) return;

  int maxTypeLen     = 0;
  int maxCategoryLen = 0;
  int maxLabelLen    = 0;
  int maxAmountLen   = 0;

  while (head != NULL)
  {
    if (flags & SHOW_TYPE)
    {
      int len = strlen(formatMovementType(head->type));
      if (len > maxTypeLen) maxTypeLen = len;
    }
    if (flags & SHOW_CATEGORY)
    {
      int len = strlen(formatMovementCategory(head->category));
      if (len > maxCategoryLen) maxCategoryLen = len;
    }
    if (flags & SHOW_LABEL)
    {
      int len = strlen(head->label);
      if (len > maxLabelLen) maxLabelLen = len;
    }
    if (flags & SHOW_AMOUNT)
    {
      int len = strlen(formatMovementAmount(head->amount));
      if (len > maxAmountLen) maxAmountLen = len;
    }
    head = head->next;
  }
  head = movements;

  while (head != NULL)
  {
    bool isLoss = head->type == LOSS;
    if (flags & SHOW_TYPE)
    {
      printf(
        "%s%-*s%s",
        isLoss ? C_RED : C_GREEN,
        maxTypeLen,
        formatMovementType(head->type),
        C_RESET
      );
    }
    if (flags & SHOW_CATEGORY)
    {
      if (flags & 0b1)
        printf(" ");
      printf(
        "%s%-*s%s",
        C_CYAN,
        maxCategoryLen,
        formatMovementCategory(head->category),
        C_RESET
      );
    }
    if (flags & SHOW_LABEL)
    {
      if (flags & 0b11)
        printf(" ");
      printf("%-*s", maxLabelLen, head->label);
    }
    if (flags & SHOW_AMOUNT)
    {
      if (flags & 0b111)
        printf(" ");
      printf(
        "%s%*s%s",
        isLoss ? C_RED : C_GREEN,
        maxAmountLen,
        formatMovementAmount(head->amount),
        C_RESET
      );
    }
    if (flags & SHOW_TIME)
    {
      if (flags & 0b1111)
        printf(" ");
      printf(
        "%s",
        getStringFromTime(&head->time)
      );
    }
    printf("\n");
    head = head->next;
  }
}

Movement* createMovement(MovementType type, MovementCategory category, char* label, float amount, time_t time)
{
  Movement* movement = (Movement*)malloc(sizeof(Movement));
  movement->type = type;
  movement->category = category;
  movement->label = label;
  movement->amount = amount;
  movement->time = time;
  movement->next = NULL;
  return movement;
}

void addMovement(Movement** movements, MovementType type, MovementCategory category, char* label, float amount, time_t time)
{
  Movement* newMovement = createMovement(type, category, label, amount, time);

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

void addMovementToDatabase(Movement* movement, const char* filePath)
{
  FILE* movementsFile = fopen(filePath, "a");
  if (movementsFile == NULL)
  {
    return;
  }

  fprintf(
    movementsFile,
    "%u;%u;%.2f;%ld;%s\n",
    movement->type,
    movement->category,
    movement->amount,
    movement->time,
    movement->label
  );
  fclose(movementsFile);
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

bool isProfit(Movement* movement)
{
  return movement->type == PROFIT;
}

bool isLoss(Movement* movement)
{
  return movement->type == LOSS;
}

bool isOfCategory(Movement* movement, void* category)
{
  return movement->category == (MovementCategory)category;
}

Movement* filterMovements(Movement* movements, MovementFilterFn predicate)
{
  Movement* head = NULL;
  Movement* tail = NULL;

  while (movements != NULL)
  {
    if (predicate(movements))
    {
      Movement* movement = createMovement(movements->type, movements->category, movements->label, movements->amount, movements->time);
      if (head == NULL)
      {
        head = movement;
        tail = head;
      }
      else
      {
        tail->next = movement;
        tail = movement;
      }
    }
    movements = movements->next;
  }

  return head;
}

Movement* filterMovementsWithParameter(Movement* movements, MovementFilterWithParameterFn predicate, void* parameter)
{
  Movement* head = NULL;
  Movement* tail = NULL;

  while (movements != NULL)
  {
    if (predicate(movements, parameter))
    {
      Movement* movement = createMovement(movements->type, movements->category, movements->label, movements->amount, movements->time);
      if (head == NULL)
      {
        head = movement;
        tail = head;
      }
      else
      {
        tail->next = movement;
        tail = movement;
      }
    }
    movements = movements->next;
  }

  return head;
}
