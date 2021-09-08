/* Protocol Definition file: box.x */
/* the input box dimensions */

struct dimensions
{
    float length;
    float width;
    float height;
};

/* returned results */
struct box_results
{
    float volume;
    float surface;
};

program RPC_BOX
{
    /* define version 1 of our remote program and its procedures */
    version BOXVERSION1
    {
        box_results BOX_CALC(dimensions) = 1;
    } = 1;
} = 0x40049453;