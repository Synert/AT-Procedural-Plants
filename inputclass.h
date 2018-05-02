#pragma once

struct mouse
{
	int m_moved[2];
	int m_position[2];
	bool m_buttons[2];
};

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	void Initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

	void SetMouseClicked(bool set);
	bool GetMouseClicked();

	void SetMousePosition(int x, int y);
	int GetMousePosition(int axis);

private:
	bool m_keys[256];
	mouse m_mouse;
};