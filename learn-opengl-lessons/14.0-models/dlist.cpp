typedef struct _dlist_link
{
    _dlist_link *previous;
    _dlist_link *next;
    void * data;
} DListLink;

DListLink *
CreateSentinel(void *data)
{
    DListLink *result = (DListLink *)malloc(sizeof(DListLink));
    result->next = result;
    result->data = data;
    result->previous = result;
    return result;
}

void 
DListInsert(DListLink *sentinel, DListLink *element)
{
    element->next = sentinel->next;
    element->previous = sentinel;
    element->next->previous = element;
    element->previous->next = element;
}