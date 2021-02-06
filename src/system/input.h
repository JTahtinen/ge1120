#pragma once

#define NUM_INPUT_KEYS 256

enum Key
{
	KEY_A = 0,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_SPACE,
	KEY_ENTER,
	KEY_ESCAPE,
	KEY_LCTRL,
	KEY_RCTRL,
	KEY_LALT,
	KEY_RALT,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_AMOUNT,
	KEY_NULL
};

struct Input
{
    bool keysPressed[KEY_AMOUNT];
    bool keysTyped[KEY_AMOUNT];
    bool keysReleased[KEY_AMOUNT];

    int mouseX;
    int mouseY;
    int mouseDeltaX;
    int mouseDeltaY;

    Input();
    void update();
    bool isKeyPressed(unsigned int key) const;
    bool isKeyTyped(unsigned int key) const;
    bool isKeyReleased(unsigned int key) const;
};
