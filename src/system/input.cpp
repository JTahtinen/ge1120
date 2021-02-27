#include "input.h"
#include "../globals.h"

Input::Input()
{
    this->mouseX = 0;
    this->mouseY = 0;
    this->mouseDeltaX = 0;
    this->mouseDeltaY = 0;
    for (int i = 0; i < KEY_AMOUNT; ++i)
    {
        this->keysPressed[i] = false;
        this->keysTyped[i] = false;
        this->keysReleased[i] = false;
    }
}

static Key convertSDLKey(SDL_Keycode key)
{
    switch (key)
	{
	case SDLK_a:
		return KEY_A;
	case SDLK_b:
		return KEY_B;
	case SDLK_c:
		return KEY_C;
	case SDLK_d:
		return KEY_D;
	case SDLK_e:
		return KEY_E;
	case SDLK_f:
		return KEY_F;
	case SDLK_g:
		return KEY_G;
	case SDLK_h:
		return KEY_H;
	case SDLK_i:
		return KEY_I;
	case SDLK_j:
		return KEY_J;
	case SDLK_k:
		return KEY_K;
	case SDLK_l:
		return KEY_L;
	case SDLK_m:
		return KEY_M;
	case SDLK_n:
		return KEY_N;
	case SDLK_o:
		return KEY_O;
	case SDLK_p:
		return KEY_P;
	case SDLK_q:
		return KEY_Q;
	case SDLK_r:
		return KEY_R;
	case SDLK_s:
		return KEY_S;
	case SDLK_t:
		return KEY_T;
	case SDLK_u:
		return KEY_U;
	case SDLK_v:
		return KEY_V;
	case SDLK_w:
		return KEY_W;
	case SDLK_x:
		return KEY_X;
	case SDLK_y:
		return KEY_Y;
	case SDLK_z:
		return KEY_Z;
	case SDLK_UP:
		return KEY_UP;
	case SDLK_DOWN:
		return KEY_DOWN;
	case SDLK_LEFT:
		return KEY_LEFT;
	case SDLK_RIGHT:
		return KEY_RIGHT;
	case SDLK_SPACE:
		return KEY_SPACE;
	case SDLK_RETURN:
		return KEY_ENTER;
	case SDLK_ESCAPE:
		return KEY_ESCAPE;
	case SDLK_LCTRL:
		return KEY_LCTRL;
	case SDLK_RCTRL:
		return KEY_RCTRL;
	case SDLK_LALT:
		return KEY_LALT;
	case SDLK_RALT:
		return KEY_RALT;
	case SDLK_0:
		return KEY_0;
	case SDLK_1:
		return KEY_1;
	case SDLK_2:
		return KEY_2;
	case SDLK_3:
		return KEY_3;
	case SDLK_4:
		return KEY_4;
	case SDLK_5:
		return KEY_5;
	case SDLK_6:
		return KEY_6;
	case SDLK_7:
		return KEY_7;
	case SDLK_8:
		return KEY_8;
	case SDLK_9:
		return KEY_9;
	case SDLK_LSHIFT:
		return KEY_LSHIFT;
	case SDLK_RSHIFT:
		return KEY_RSHIFT;
    case SDLK_F1:
        return KEY_F1;
    case SDLK_F2:
        return KEY_F2;
    case SDLK_F3:
        return KEY_F3;
    case SDLK_F4:
        return KEY_F4;
    case SDLK_F5:
        return KEY_F5;
    case SDLK_F6:
        return KEY_F6;
    case SDLK_F7:
        return KEY_F7;
    case SDLK_F8:
        return KEY_F8;
    case SDLK_F9:
        return KEY_F9;
	default:
		return KEY_NULL;
	}
}

void Input::update()
{
    this->mouseDeltaX = 0;
    this->mouseDeltaY = 0;
    for (int i = 0; i < KEY_AMOUNT; ++i)
    {
        this->keysTyped[i] = false;
        this->keysReleased[i] = false;
    }

    for (int i = 0; i < g_events.numInputEvents; ++i)
    {
        SDL_Event ev = g_events.inputEvents[i];

        switch(ev.type)
        {
            case SDL_KEYDOWN:
            {
                Key key = convertSDLKey(ev.key.keysym.sym);
    
                this->keysPressed[key] = true;
                this->keysTyped[key] = true;

                break;
            }
            case SDL_KEYUP:
            {
                Key key = convertSDLKey(ev.key.keysym.sym);
                this->keysPressed[key] = false;
                this->keysReleased[key] = true;
                break;
            }
            case SDL_MOUSEMOTION:
            {
                SDL_GetMouseState(&this->mouseX, &this->mouseY);
                this->mouseDeltaX = ev.motion.xrel;
                this->mouseDeltaY = ev.motion.yrel;
                break;
            }
        }
    }
}

bool Input::isKeyPressed(unsigned int key) const
{
    return this->keysPressed[key];
}

bool Input::isKeyTyped(unsigned int key) const
{
    return this->keysTyped[key];
}

bool Input::isKeyReleased(unsigned int key) const
{
    return this->keysReleased[key];
}
