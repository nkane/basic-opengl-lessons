#pragma once

struct IndexBuffer 
{
    unsigned int RendererID;
    unsigned int Count;
};

IndexBuffer *
NewIndexBuffer(const unsigned int *data, unsigned int count);

void
DeleteIndexBuffer(IndexBuffer *i);

void
Bind(IndexBuffer *i);

void
Unbind(IndexBuffer *i);