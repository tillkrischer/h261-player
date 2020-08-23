#include "tables.h"

void initEmptyTable(struct vlctable *table)
{
    int length = table->length;
    int maxvalue = table->maxvalue;

    table->data = (char **)malloc(length * sizeof(char *));
    for (int i = 0; i < length; i++)
    {
        table->data[i] = (char *)malloc(maxvalue * sizeof(char));
        memset(table->data[i], EMPTY, maxvalue * sizeof(char));
    }
}

void initMBATable()
{
    MBATable.length = 17;
    MBATable.maxvalue = 36;
    initEmptyTable(&MBATable);

    int lengths[33] = {1, 3, 3, 4, 4, 5, 5, 7, 7, 8, 8, 8, 8, 8, 8, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11};
    int indicies[33] = {1, 3, 2, 3, 2, 3, 2, 7, 6, 11, 10, 9, 8, 7, 6, 23, 22, 21, 20, 19, 18, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24};
    int values[33] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33};

    for (int i = 0; i < 33; i++)
        MBATable.data[lengths[i]][indicies[i]] = values[i];

    MBATable.data[11][15] = MBA_STUFFING;
    MBATable.data[16][1] = ESCAPE;
}

void initMTYPETable()
{
    MTYPETable.length = 11;
    MTYPETable.maxvalue = 2;

    initEmptyTable(&MTYPETable);

    int lengths[10] = {4, 7, 1, 5, 9, 8, 10, 3, 2, 6};
    int indicies[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    int values[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for (int i = 0; i < 10; i++)
        MTYPETable.data[lengths[i]][indicies[i]] = values[i];
}

void initIntraDcTable()
{
    for (int i = 1; i <= 254; i++)
        IntraDcTable[i] = 8 * i;
    IntraDcTable[255] = 1024;
}

void initTCOEFFFirstIndexTable()
{
    TCOEFFFirstIndexTable.length = 15;
    TCOEFFFirstIndexTable.maxvalue = 80;

    initEmptyTable(&TCOEFFFirstIndexTable);

    int lengths[127] = {2, 2, 5, 5, 6, 6, 8, 8, 9, 9, 9, 9, 11, 11, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 4, 4, 7, 7, 9, 9, 11, 11, 13, 13, 14, 14, 14, 14, 5, 5, 8, 8, 11, 11, 13, 13, 14, 14, 6, 6, 9, 9, 13, 13, 14, 14, 6, 6, 11, 11, 13, 13, 7, 7, 11, 11, 14, 14, 7, 7, 13, 13, 7, 7, 13, 13, 8, 8, 13, 13, 8, 8, 14, 14, 9, 9, 14, 14, 9, 9, 9, 9, 9, 9, 11, 11, 11, 11, 11, 11, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 6};
    int indicies[127] = {2, 3, 8, 9, 10, 11, 12, 13, 76, 77, 66, 67, 20, 21, 58, 59, 48, 49, 38, 39, 32, 33, 52, 53, 50, 51, 48, 49, 46, 47, 6, 7, 12, 13, 74, 75, 24, 25, 54, 55, 44, 45, 42, 43, 10, 11, 8, 9, 22, 23, 40, 41, 40, 41, 14, 15, 72, 73, 56, 57, 38, 39, 12, 13, 30, 31, 36, 37, 14, 15, 18, 19, 36, 37, 10, 11, 60, 61, 8, 9, 42, 43, 14, 15, 34, 35, 10, 11, 34, 35, 78, 79, 32, 33, 70, 71, 68, 69, 64, 65, 28, 29, 26, 27, 16, 17, 62, 63, 52, 53, 50, 51, 46, 47, 44, 45, 62, 63, 60, 61, 58, 59, 56, 57, 54, 55, 1};

    for (int i = 0; i < 127; i++)
    {
        TCOEFFFirstIndexTable.data[lengths[i]][indicies[i]] = i;
    }
}

void initTCOEFFIndexTable()
{
    TCOEFFIndexTable.length = 15;
    TCOEFFIndexTable.maxvalue = 80;

    initEmptyTable(&TCOEFFIndexTable);

    int lengths[128] = {2, 3, 3, 5, 5, 6, 6, 8, 8, 9, 9, 9, 9, 11, 11, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 4, 4, 7, 7, 9, 9, 11, 11, 13, 13, 14, 14, 14, 14, 5, 5, 8, 8, 11, 11, 13, 13, 14, 14, 6, 6, 9, 9, 13, 13, 14, 14, 6, 6, 11, 11, 13, 13, 7, 7, 11, 11, 14, 14, 7, 7, 13, 13, 7, 7, 13, 13, 8, 8, 13, 13, 8, 8, 14, 14, 9, 9, 14, 14, 9, 9, 9, 9, 9, 9, 11, 11, 11, 11, 11, 11, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 6};
    int indicies[128] = {2, 6, 7, 8, 9, 10, 11, 12, 13, 76, 77, 66, 67, 20, 21, 58, 59, 48, 49, 38, 39, 32, 33, 52, 53, 50, 51, 48, 49, 46, 47, 6, 7, 12, 13, 74, 75, 24, 25, 54, 55, 44, 45, 42, 43, 10, 11, 8, 9, 22, 23, 40, 41, 40, 41, 14, 15, 72, 73, 56, 57, 38, 39, 12, 13, 30, 31, 36, 37, 14, 15, 18, 19, 36, 37, 10, 11, 60, 61, 8, 9, 42, 43, 14, 15, 34, 35, 10, 11, 34, 35, 78, 79, 32, 33, 70, 71, 68, 69, 64, 65, 28, 29, 26, 27, 16, 17, 62, 63, 52, 53, 50, 51, 46, 47, 44, 45, 62, 63, 60, 61, 58, 59, 56, 57, 54, 55, 1};

    for (int i = 0; i < 128; i++)
    {
        TCOEFFIndexTable.data[lengths[i]][indicies[i]] = i;
    }
}
void initTCOEFFValues()
{
    int defaultFirstLevelValues[127] = {1, -1, 2, -2, 3, -3, 4, -4, 5, -5, 6, -6, 7, -7, 8, -8, 9, -9, 10, -10, 11, -11, 12, -12, 13, -13, 14, -14, 15, -15, 1, -1, 2, -2, 3, -3, 4, -4, 5, -5, 6, -6, 7, -7, 1, -1, 2, -2, 3, -3, 4, -4, 5, -5, 1, -1, 2, -2, 3, -3, 4, -4, 1, -1, 2, -2, 3, -3, 1, -1, 2, -2, 3, -3, 1, -1, 2, -2, 1, -1, 2, -2, 1, -1, 2, -2, 1, -1, 2, -2, 1, -1, 2, -2, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, ESCAPE};
    int defaultFirstRunValues[127] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, ESCAPE};
    int defaultLevelValues[128] = {EOB, 1, -1, 2, -2, 3, -3, 4, -4, 5, -5, 6, -6, 7, -7, 8, -8, 9, -9, 10, -10, 11, -11, 12, -12, 13, -13, 14, -14, 15, -15, 1, -1, 2, -2, 3, -3, 4, -4, 5, -5, 6, -6, 7, -7, 1, -1, 2, -2, 3, -3, 4, -4, 5, -5, 1, -1, 2, -2, 3, -3, 4, -4, 1, -1, 2, -2, 3, -3, 1, -1, 2, -2, 3, -3, 1, -1, 2, -2, 1, -1, 2, -2, 1, -1, 2, -2, 1, -1, 2, -2, 1, -1, 2, -2, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, ESCAPE};
    int defaultRunValues[128] = {EOB, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, ESCAPE};

    memcpy(FirstLevelValues, defaultFirstLevelValues, sizeof(defaultFirstLevelValues));
    memcpy(FirstRunValues, defaultFirstRunValues, sizeof(defaultFirstRunValues));
    memcpy(LevelValues, defaultLevelValues, sizeof(defaultLevelValues));
    memcpy(RunValues, defaultRunValues, sizeof(defaultRunValues));
}

void initZigZagTables()
{
    int zz[8][8] = {
        {1, 2, 6, 7, 15, 16, 28, 29},
        {3, 5, 8, 14, 17, 27, 30, 43},
        {4, 9, 13, 18, 26, 31, 42, 44},
        {10, 12, 19, 25, 32, 41, 45, 54},
        {11, 20, 24, 33, 40, 46, 53, 55},
        {21, 23, 34, 39, 47, 52, 56, 61},
        {22, 35, 38, 48, 51, 57, 60, 62},
        {36, 37, 49, 50, 58, 59, 63, 64}};
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
        {
            int v = zz[y][x];
            ZigZagTableY[v] = y;
            ZigZagTableX[v] = x;
        }

    0;
}

void initDCTTables()
{
    for (int v = 0; v < 8; v++)
        for (int u = 0; u < 8; u++)
            for (int y = 0; y < 8; y++)
                for (int x = 0; x < 8; x++)
                {
                    double Cu = (u == 0) ? 1 / sqrt(2) : 1.0;
                    double Cv = (v == 0) ? 1 / sqrt(2) : 1.0;
                    DCTTables[v][u][y][x] = 0.25 * Cu * Cv * cos(PI * (2 * x + 1) * u / 16.0) * cos(PI * (2 * y + 1) * v / 16.0);
                }

    return;
}

void initCBPTable()
{
    CBPTable.length = 10;
    CBPTable.maxvalue = 32;

    initEmptyTable(&CBPTable);

    int lengths[63] = {3, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9};
    int indicies[63] = {7, 13, 12, 11, 10, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 15, 14, 13, 12, 23, 22, 21, 20, 19, 18, 17, 16, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 7, 6, 5, 4, 3, 2};
    int values[63] = {60, 4, 8, 16, 32, 12, 48, 20, 40, 28, 44, 52, 56, 1, 61, 2, 62, 24, 36, 3, 63, 5, 9, 17, 33, 6, 10, 18, 34, 7, 11, 19, 35, 13, 49, 21, 41, 14, 50, 22, 42, 15, 51, 23, 43, 25, 37, 26, 38, 29, 45, 53, 57, 30, 46, 54, 58, 31, 47, 55, 59, 27, 39};

    for (int i = 0; i < 63; i++)
        CBPTable.data[lengths[i]][indicies[i]] = values[i];
}

void initMVDTable()
{
    MVDTable.length = 12;
    MVDTable.maxvalue = 36;

    initEmptyTable(&MVDTable);

    int lengths[32] = {11, 11, 11, 11, 11, 11, 10, 10, 10, 8, 8, 8, 7, 5, 4, 3, 1, 3, 4, 5, 7, 8, 8, 8, 10, 10, 10, 11, 11, 11, 11};
    int indicies[32] = {25, 27, 29, 31, 33, 35, 19, 21, 23, 7, 9, 11, 7, 3, 3, 3, 1, 2, 2, 2, 6, 10, 8, 6, 22, 20, 18, 34, 32, 30, 28, 26};
    int values[32] = {-16, -15, -14, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    for (int i = 0; i < 32; i++)
        MVDTable.data[lengths[i]][indicies[i]] = values[i];
}

void initTables()
{
    initMBATable();
    initMTYPETable();
    initIntraDcTable();
    initTCOEFFFirstIndexTable();
    initTCOEFFIndexTable();
    initTCOEFFValues();
    initZigZagTables();
    initDCTTables();
    initCBPTable();
    initMVDTable();
}