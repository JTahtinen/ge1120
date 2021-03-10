#pragma once

struct Line
{
    Vec2 p1;
    Vec2 p2;
};

struct SpriteData
{
    Vec3 pos;
    Vec2 texCoord;
    Vec3 model[3];
    Vec3 view[3];
};

struct QuadData
{
    Vec3 pos;
    Vec4 color;
    Vec3 model[3];
    Vec3 view[3];
};

struct LineData
{
    Vec2 pos;
    Vec3 model[3];
    Vec3 view[3];
};

struct LetterData
{
    Vec3 pos;
    Vec2 texCoord;
    Vec3 view[3];
};

#define MAX_RENDERABLES (60000)

#define SPRITE_VERTEX_SIZE (sizeof(SpriteData))
#define SPRITE_SIZE (SPRITE_VERTEX_SIZE * 2)
#define SPRITE_BUFFER_SIZE (SPRITE_SIZE * MAX_RENDERABLES)
#define SPRITE_INDICES_SIZE (MAX_RENDERABLES * 2)

#define LINE_VERTEX_SIZE (sizeof(LineData))
#define LINE_SIZE (LINE_VERTEX_SIZE * 2)
#define LINE_BUFFER_SIZE (LINE_SIZE * MAX_RENDERABLES)
#define LINE_INDICES_SIZE (MAX_RENDERABLES * 2)

#define QUAD_VERTEX_SIZE (sizeof(QuadData))
#define QUAD_SIZE (QUAD_VERTEX_SIZE * 4)
#define QUAD_BUFFER_SIZE (QUAD_SIZE * MAX_RENDERABLES)
#define QUAD_INDICES_SIZE (MAX_RENDERABLES * 6)

#define LETTER_VERTEX_SIZE (sizeof(LetterData))
#define LETTER_SIZE (LETTER_VERTEX_SIZE * 4)
#define LETTER_BUFFER_SIZE (LETTER_SIZE * MAX_RENDERABLES)
#define LETTER_INDICES_SIZE (MAX_RENDERABLES * 6)


