#include <stdio.h>
#include <windows.h>
#include <fcntl.h>
#include <stdbool.h>
#include "tables.h"
#include "math.h"

#include "video.h"

void unreadBits(struct reader *r, int bits)
{
    if (bits > 0)
    {
        int back = bits / 8;
        if (bits % 8 > r->currbit)
            back++;

        r->stack += back;
        r->i = (r->i - back + READER_BUFFER_SIZE) % READER_BUFFER_SIZE;

        r->currbit -= bits % 8;
        if (r->currbit < 0)
            r->currbit += 8;

        r->total -= bits;
    }
}

long readBits(struct reader *r, int bits)
{
    long result = 0;
    r->total += bits;

    while (bits > 0)
    {
        if (r->currbit == 8)
        {
            r->i = (r->i + 1) % READER_BUFFER_SIZE;
            if (r->stack > 0)
                r->stack--;
            else
            {
                int br = read(r->fd, r->data + r->i, 1);
            }

            r->currbit = 0;
        }
        int start = r->currbit;
        int end = MIN(8, r->currbit + bits);
        int length = end - start;
        unsigned char mask = (255 >> start) & (255 << (8 - end));
        long mdata = (r->data[r->i] & mask) >> (8 - end);
        result |= mdata << (bits - length);
        r->currbit = end;
        bits -= length;
    }

    return result;
}

// jump to next full byte
void syncRead(struct reader *r)
{
    r->currbit = 8;
}

bool readVLC(struct reader *r, struct vlctable *table, int *result)
{
    int value, i, l;
    l = i = 0;
    i = 0;
    while (l < table->length && (value = table->data[l][i]) == EMPTY)
    {
        i = (i << 1) | readBits(r, 1);
        l++;
    }
    if (l == table->length)
    {
        unreadBits(r, l - 1);
        return false;
    }

    *result = value;
    return true;
}

int reconstruct(int level, int quant)
{
    if (quant % 2)
    {
        if (level > 0)
            return MIN(2047, quant * (2 * level + 1));
        else
            return MAX(-2048, quant * (2 * level - 1));
    }
    else
    {
        if (level > 0)
            return MIN(2047, quant * (2 * level + 1) - 1);
        else
            return MAX(-2048, quant * (2 * level - 1) + 1);
    }
}

void IDCT(double f[8][8], int F, int u, int v)
{
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            f[y][x] += F * DCTTables[v][u][y][x];
}

void readCoeff(struct reader *r, double f[8][8], int quant, bool intra)
{
    int FLC, pos, run, level, u, v, F, index;

    pos = 1;

    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            f[y][x] = 0.0;

    // first coefficient
    // intra
    if (intra)
    {
        FLC = readBits(r, 8);
        F = IntraDcTable[FLC];
        IDCT(f, F, 0, 0);
    }
    // inter
    else
    {
        readVLC(r, &TCOEFFFirstIndexTable, &index);
        level = FirstLevelValues[index];
        run = FirstRunValues[index];
        if (run == ESCAPE)
        {
            run = readBits(r, 6);
            level = (char)readBits(r, 8);
        }

        // printf("run: %d level: %d\n", run, level);

        pos += run;
        // pos = 1;
        F = reconstruct(level, quant);
        v = ZigZagTableY[pos];
        u = ZigZagTableX[pos];

        IDCT(f, F, u, v);
    }
    // remaining coefficients
    pos = 2;
    readVLC(r, &TCOEFFIndexTable, &index);
    level = LevelValues[index];
    run = RunValues[index];

    while (run != EOB)
    {
        if (run == ESCAPE)
        {

            run = readBits(r, 6);
            level = (char)readBits(r, 8);
        }

        // printf("run: %d level: %d\n", run, level);

        pos += run;

        F = reconstruct(level, quant);
        v = ZigZagTableY[pos];
        u = ZigZagTableX[pos];

        IDCT(f, F, u, v);

        pos++;

        readVLC(r, &TCOEFFIndexTable, &index);
        level = LevelValues[index];
        run = RunValues[index];
    }

    // printf("EOB\n");
}

void addPixel(double *dest, double f)
{
    int intf = (int)MIN(255, MAX(-256, f));
    *dest = MIN(255, MAX(0, *dest + intf));
}

void putPixel(double *dest, double f)
{
    int intf = (int)MIN(255, MAX(-256, f + 0.5));
    *dest = MIN(255, MAX(0, intf));
}

void addBlock(struct picture *pic, double f[8][8], int xoff, int yoff, enum blocktype type)
{
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
        {
            if (type == Y1)
                addPixel(pic->Y[yoff + y] + xoff + x, f[y][x]);
            else if (type == Y2)
                addPixel(pic->Y[yoff + y] + xoff + 8 + x, f[y][x]);
            else if (type == Y3)
                addPixel(pic->Y[yoff + 8 + y] + xoff + x, f[y][x]);
            else if (type == Y4)
                addPixel(pic->Y[yoff + 8 + y] + xoff + 8 + x, f[y][x]);
            else if (type == Cb)
                addPixel(pic->Cb[yoff / 2 + y] + xoff / 2 + x, f[y][x]);
            else if (type == Cr)
                addPixel(pic->Cr[yoff / 2 + y] + xoff / 2 + x, f[y][x]);
        }
}

void putBlock(struct picture *pic, double f[8][8], int xoff, int yoff, enum blocktype type)
{
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
        {
            if (type == Y1)
                putPixel(pic->Y[yoff + y] + xoff + x, f[y][x]);
            else if (type == Y2)
                putPixel(pic->Y[yoff + y] + xoff + 8 + x, f[y][x]);
            else if (type == Y3)
                putPixel(pic->Y[yoff + 8 + y] + xoff + x, f[y][x]);
            else if (type == Y4)
                putPixel(pic->Y[yoff + 8 + y] + xoff + 8 + x, f[y][x]);
            else if (type == Cb)
                putPixel(pic->Cb[yoff / 2 + y] + xoff / 2 + x, f[y][x]);
            else if (type == Cr)
                putPixel(pic->Cr[yoff / 2 + y] + xoff / 2 + x, f[y][x]);
        }
}

moveBlock(struct picture *pic, struct picture *prev, int xoff, int yoff, enum blockType type, int mvx, int mvy)
{
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
        {
            if (type == Y1)
                pic->Y[yoff + y][xoff + x] = prev->Y[yoff + y + mvy][xoff + x + mvx];
            else if (type == Y2)
                pic->Y[yoff + y][xoff + 8 + x] = prev->Y[yoff + y + mvy][xoff + 8 + x + mvx];
            else if (type == Y3)
                pic->Y[yoff + 8 + y][xoff + x] = prev->Y[yoff + 8 + y + mvy][xoff + x + mvx];
            else if (type == Y4)
                pic->Y[yoff + 8 + y][xoff + 8 + x] = prev->Y[yoff + 8 + y + mvy][xoff + 8 + x + mvx];
            else if (type == Cb)
                pic->Cb[yoff / 2 + y][xoff / 2 + x] = prev->Cb[yoff / 2 + y + mvy / 2][xoff / 2 + x + mvx / 2];
            else if (type == Cr)
                pic->Cr[yoff / 2 + y][xoff / 2 + x] = prev->Cr[yoff / 2 + y + mvy / 2][xoff / 2 + x + mvx / 2];
        }
}

int decodeMTYPE(int MTYPE)
{
    switch (MTYPE)
    {
    case 1:
        return IS_INTRA;
    case 2:
        return IS_INTRA | IS_MQUANT;
    case 3:
        return IS_CBP;
    case 4:
        return IS_MQUANT | IS_CBP;
    case 5:
        return IS_MVD;
    case 6:
        return IS_MVD | IS_CBP;
    case 7:
        return IS_MQUANT | IS_MVD | IS_CBP;
    case 8:
        return IS_MVD | IS_FIL;
    case 9:
        return IS_MVD | IS_CBP | IS_FIL;
    case 10:
        return IS_MQUANT | IS_MVD | IS_CBP | IS_FIL;
    default:
        return 0;
    }
}

int readMB(struct reader *r, struct picture *pic, struct picture *prev, int *blocknumber, int gxoff, int gyoff, int *quant, int *mvx, int *mvy)
{
    int MBA, MTYPE, MQUANT, CBP, MVDX, MVDY;
    int xoff, yoff, mtype;
    bool doReadCoeff[8];
    double f[8][8];
    enum blockType types[6] = {Y1, Y2, Y3, Y4, Cb, Cr};

    // MBA
    if (!readVLC(r, &MBATable, &MBA))
    {
        // OutputDebugString("syncing\n");
        syncRead(r);
        readVLC(r, &MBATable, &MBA);
    }
    if (MBA == ESCAPE)
    {
        unreadBits(r, 16);
        return 0;
    }
    if (MBA == MBA_STUFFING)
        return 1;

    *blocknumber += MBA;

    // printf("MB#: %d\n", *blocknumber);

    // MTYPE
    readVLC(r, &MTYPETable, &MTYPE);
    mtype = decodeMTYPE(MTYPE);

    // printf("MTYPE# %d\n", MTYPE);
    if (!(MTYPE == 1 || MTYPE == 3 || MTYPE == 5 || MTYPE == 6))
        MTYPE = MTYPE;

    // MQUANT
    if (mtype & IS_MQUANT)
    {
        MQUANT = readBits(r, 5);
        *quant = MQUANT;
    }

    // MVD
    if (mtype & IS_MVD)
    {
        if (*blocknumber == 1 || *blocknumber == 12 || *blocknumber == 23 || MBA != 1)
        {
            *mvx = 0;
            *mvy = 0;
        }
        readVLC(r, &MVDTable, &MVDX);
        // printf("MVDX#: %d\n", MVDX);
        readVLC(r, &MVDTable, &MVDY);
        // printf("MVDY#: %d\n", MVDY);
        *mvx += MVDX;
        *mvy += MVDY;
        if (*mvx < -15)
            *mvx += 32;
        if (*mvy < -15)
            *mvy += 32;
        if (*mvx > 15)
            *mvx -= 32;
        if (*mvy > 15)
            *mvy -= 32;
    }
    else
    {
        *mvx = 0;
        *mvy = 0;
    }

    // CBP
    if (mtype & IS_INTRA)
    {
        for (int i = 0; i < 6; i++)
            doReadCoeff[i] = true;
    }
    else if (!(mtype & IS_CBP))
    {
        for (int i = 0; i < 6; i++)
            doReadCoeff[i] = false;
    }
    else
    {
        readVLC(r, &CBPTable, &CBP);
        // printf("CBP# %d\n", CBP);
        for (int i = 0; i < 6; i++)
            doReadCoeff[i] = CBP & (1 << (5 - i)) ? true : false;
    }

    xoff = (*blocknumber - 1) % 11 * 16;
    yoff = (*blocknumber - 1) / 11 * 16;

    for (int i = 0; i < 6; i++)
    {
        if (*mvx != 0 || *mvy != 0)
        {
            moveBlock(pic, prev, gxoff + xoff, gyoff + yoff, types[i], *mvx, *mvy);
        }
        if (doReadCoeff[i])
        {
            readCoeff(r, f, *quant, mtype & IS_INTRA);
            if (mtype & IS_INTRA)
                putBlock(pic, f, gxoff + xoff, gyoff + yoff, types[i]);
            else
                addBlock(pic, f, gxoff + xoff, gyoff + yoff, types[i]);
        }
    }

    return 1;
}

void readGOB(struct reader *r, struct picture *pic, struct picture *prev)
{
    int GBSC, GN, GQUANT, PEI, quant, xoff, yoff, blocknumber, mvx, mvy;

    GBSC = readBits(r, 16);
    GN = readBits(r, 4);
    GQUANT = readBits(r, 5);
    quant = GQUANT;
    PEI = readBits(r, 1);
    while (PEI)
    {
        readBits(r, 9);
        PEI = readBits(r, 1);
    }

    // printf("GN#: %d\n", GN);

    xoff = ((GN - 1) % 2) * 176;
    yoff = ((GN - 1) / 2) * 48;
    blocknumber = 0;
    mvx = mvy = 0;
    while (readMB(r, pic, prev, &blocknumber, xoff, yoff, &quant, &mvx, &mvy))
        ;
}

void readPicture(struct reader *r, struct picture *pic, struct picture *prev)
{
    int PSC, TR, PTYPE, GEI;
    PSC = readBits(r, 20);
    TR = readBits(r, 5);
    PTYPE = readBits(r, 6);
    GEI = readBits(r, 1);
    while (GEI)
    {
        readBits(r, 9);
        GEI = readBits(r, 1);
    }

    // CIF
    if (PTYPE & (1 << 2))
    {
        pic->height = 288;
        pic->width = 352;
        for (int i = 0; i < 12; i++)
            readGOB(r, pic, prev);
    }
    // QCIF
    else
    {
        // not tested
        pic->height = 144;
        pic->width = 176;
        for (int i = 0; i < 3; i++)
            readGOB(r, pic, prev);
    }
}

void initReader(struct reader *r, char *file)
{
    r->fd = open(file, O_RDONLY | O_BINARY);
    r->i = READER_BUFFER_SIZE - 1;
    r->stack = 0;
    r->currbit = 8;
    r->total = 0;
}

static void pgm_save2(double buf[288][352], int xsize, int ysize,
                      char *filename)
{
    FILE *f;
    int i, j;

    f = fopen(filename, "w");
    fprintf(f, "P2\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
    {
        for (j = 0; j < xsize; j++)
            fprintf(f, "%d ", (unsigned char)buf[i][j]);
        fprintf(f, "\n");
    }
    fclose(f);
}

// int main()
// {
//     initTables();

//     struct reader r;
//     // initReader(&r, "C:\\Users\\Till\\Desktop\\small.h261");
//     initReader(&r, "C:\\Users\\Till\\Desktop\\akiyo.h261");

//     struct picture *pic, *prev;

//     pic = (struct picture *)malloc(sizeof(struct picture));
//     prev = (struct picture *)malloc(sizeof(struct picture));

//     for (int i = 1; i <= 2; i++)
//     {
//         readPicture(&r, pic, prev);
//         char framefile[100];
//         sprintf(framefile, "C:\\Users\\Till\\Desktop\\video-frames\\akiyo-%d.pgm", i);
//         pgm_save2(pic->Y, 352, 288, framefile);
//         *prev = *pic;
//     }

//     free(pic);
//     free(prev);

//     close(r.fd);
// }