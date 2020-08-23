// TODO: No! cos and sqrt
#include <math.h>

#define PI 3.14159265

#define MBA_STUFFING 99
#define ESCAPE 98
#define EOB 97
#define EMPTY -128

struct vlctable
{
    int length;
    int maxvalue;
    char **data;
};

struct vlctable MBATable;
struct vlctable MTYPETable;
int IntraDcTable[256];
struct vlctable TCOEFFFirstIndexTable;
struct vlctable TCOEFFIndexTable;
int FirstLevelValues[127];
int FirstRunValues[127];
int LevelValues[128];
int RunValues[128];
int ZigZagTableX[65];
int ZigZagTableY[65];
double DCTTables[8][8][8][8];
struct vlctable CBPTable;
struct vlctable MVDTable;

void initTables();