#include "inputclass.h"


InputClass::InputClass()
{
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}


void InputClass::Initialize()
{
	// Initialize all the keys to being released and not pressed.
	for (int i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	//initialize the mouse
	for (int i = 0; i < 2; i++)
	{
		m_mouse.m_buttons[i] = false;
		m_mouse.m_moved[i] = 0;
	}

	return;
}


void InputClass::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
	return;
}


void InputClass::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
	return;
}


bool InputClass::IsKeyDown(unsigned int key)
{
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}

void InputClass::SetMouseClicked(bool set)
{
	m_mouse.m_buttons[0] = set;
}

bool InputClass::GetMouseClicked()
{
	return m_mouse.m_buttons[0];
}

void InputClass::SetMousePosition(int x, int y)
{
	m_mouse.m_moved[0] = x;
	m_mouse.m_moved[1] = y;
}

int InputClass::GetMousePosition(int axis)
{
	if (axis != 0 && axis != 1) return 0;
	return m_mouse.m_moved[axis];
}