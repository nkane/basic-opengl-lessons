typedef struct _texture
{
    unsigned int id;
    unsigned int slot;
    int width;
    int height;
    int channels;
    unsigned char *data;
} Texture;
