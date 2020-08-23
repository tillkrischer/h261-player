
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define READER_BUFFER_SIZE 12

#define IS_INTRA 1
#define IS_MQUANT (1 << 1)
#define IS_MVD (1 << 2)
#define IS_CBP (1 << 3)
#define IS_FIL (1 << 4)

enum blockType
{
    Y1,
    Y2,
    Y3,
    Y4,
    Cb,
    Cr
};

struct reader
{
    int fd;
    unsigned char data[READER_BUFFER_SIZE];
    int i;
    int currbit;
    int stack;
    long total;
};

struct picture
{
    int height;
    int width;
    double Y[288][352];
    double Cb[144][176];
    double Cr[144][176];
};

void initTables();
void initReader(struct reader *r, char *file);
void readPicture(struct reader *r, struct picture *pic, struct picture *prev);