#pragma once

struct VertexBuffer
{
    unsigned int RendererID;
};

VertexBuffer *
NewVertexBuffer(const void *data, unsigned int size);

void
DeleteVertexBuffer(VertexBuffer *v);

void
Bind(VertexBuffer *v);

void
Unbind(VertexBuffer *v);