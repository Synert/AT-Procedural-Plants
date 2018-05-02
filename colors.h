#pragma once

//colors
#define COL_WHITE XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)
#define COL_GRAY XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f)
#define COL_BLACK XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)

#define COL_GREEN XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)
#define COL_RED XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)
#define COL_BLUE XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)
#define COL_PURPLE XMFLOAT4(0.75f, 0.0f, 1.0f, 1.0f)
#define COL_ORANGE XMFLOAT4(1.0f, 0.75f, 0.0f, 1.0f)
#define COL_PINK XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)
#define COL_YELLOW XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)
#define COL_CYAN XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)
#define COL_TURQUOISE XMFLOAT4(0.0f, 1.0f, 0.75f, 1.0f)
#define COL_BROWN XMFLOAT4(0.7f, 0.3f, 0.2f, 1.0f)
#define COL_TAN XMFLOAT4(1.0f, 0.75f, 0.5f, 1.0f)

#define COL_DARKGREEN XMFLOAT4(0.0f, 0.2f, 0.0f, 1.0f)
#define COL_DARKRED XMFLOAT4(0.2f, 0.0f, 0.0f, 1.0f)
#define COL_DARKBLUE XMFLOAT4(0.0f, 0.0f, 0.2f, 1.0f)
#define COL_DARKPURPLE XMFLOAT4(0.15f, 0.0f, 0.2f, 1.0f)
#define COL_DARKORANGE XMFLOAT4(0.2f, 0.15f, 0.0f, 1.0f)
#define COL_DARKPINK XMFLOAT4(0.2f, 0.0f, 0.2f, 1.0f)
#define COL_DARKYELLOW XMFLOAT4(0.2f, 0.2f, 0.0f, 1.0f)
#define COL_DARKCYAN XMFLOAT4(0.0f, 0.2f, 0.2f, 1.0f)
#define COL_DARKTURQUOISE XMFLOAT4(0.0f, 0.2f, 0.15f, 1.0f)
#define COL_DARKBROWN XMFLOAT4(0.2f, 0.1f, 0.05f, 1.0f)
#define COL_DARKTAN XMFLOAT4(0.2f, 0.15f, 0.1f, 1.0f)

#define COL_LEAFGREEN XMFLOAT4(0.05f, 0.2f, 0.05f, 1.0f)
#define COL_TRUNKBROWN XMFLOAT4(0.6f, 0.3f, 0.15f, 1.0f)

#define COL_RANDOM XMFLOAT4((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f, 1.0f)
#define COL_DARKRANDOM XMFLOAT4((rand() % 100) / 200.0f, (rand() % 100) / 200.0f, (rand() % 100) / 200.0f, 1.0f)
#define COL_LIGHTRANDOM XMFLOAT4(0.5f + (rand() % 100) / 200.0f, 0.5f + (rand() % 100) / 200.0f, 0.5f + (rand() % 100) / 200.0f, 1.0f)