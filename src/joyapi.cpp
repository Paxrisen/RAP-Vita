#include "SDL.h"
#include "i_video.h"
#include "joyapi.h"
#include "ptrapi.h"
#include "kbdapi.h"
#include "input.h"

int joy_ack;

int joyinput[MAX_CONTROLLERS][16];
int joyinputmap[MAX_CONTROLLERS][4];

SDL_GameController* ControllerHandles[MAX_CONTROLLERS];

int MaxJoysticks;
int ControllerIndex;
int JoystickIndex;

static unsigned int lastTime = 0;

/***************************************************************************
IPT_CalJoy() - Open Gamecontroller
 ***************************************************************************/
void 
IPT_CalJoy(
	void
)
{
	if (SDL_Init(SDL_INIT_GAMECONTROLLER) < 0)
		return;

	MaxJoysticks = SDL_NumJoysticks();
	ControllerIndex = 0;
	
	for (JoystickIndex = 0; JoystickIndex < MaxJoysticks; ++JoystickIndex)
	{
		if (!SDL_IsGameController(JoystickIndex))
		{
			continue;
		}
		if (ControllerIndex >= MAX_CONTROLLERS)
		{
			break;
		}
		
		ControllerHandles[ControllerIndex] = SDL_GameControllerOpen(JoystickIndex);
		
		ControllerIndex++;
	}
}

/***************************************************************************
IPT_CloJoy() - Close Gamecontroller
 ***************************************************************************/
void  
IPT_CloJoy(
	int closeall
)
{
	for (ControllerIndex = 0; ControllerIndex < MAX_CONTROLLERS; ++ControllerIndex)
	{
		for (int i = 0; i < 16; i++)
			joyinput[ControllerIndex][i] = 0;

		for (int i = 0; i < 4; i++)
			joyinputmap[ControllerIndex][i] = 0;

		if (ControllerHandles[ControllerIndex] && !SDL_GameControllerGetAttached(ControllerHandles[ControllerIndex]) && !closeall)
		{
			SDL_GameControllerClose(ControllerHandles[ControllerIndex]);
		}
		else if (ControllerHandles[ControllerIndex] && closeall)
		{
			SDL_GameControllerClose(ControllerHandles[ControllerIndex]);
		}
	}
}

/***************************************************************************
IPT_ConvertAxisValue() - Convert raw values from axis to usable input values
 ***************************************************************************/
static int
IPT_ConvertAxisValue(
	int AxisValue,        //Raw value from axis
	int RangeDivison      //Defines axis value range division
)
{
	int deadzone = 8000;
	int result;

	if (AxisValue == 0 ||
		(AxisValue > 0 && AxisValue < deadzone) ||
		(AxisValue < 0 && AxisValue > -deadzone))
		return 0;

	if (AxisValue > 0)
		result = (AxisValue - deadzone) * RangeDivison / (32767 - deadzone);
	else
		result = (AxisValue + deadzone) * RangeDivison / (32768 - deadzone);

	return result;
}

/***************************************************************************
I_HandleJoystickEvent() - Get current button or axis status
 ***************************************************************************/
void 
I_HandleJoystickEvent(
	SDL_Event *sdlevent
)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex] != 0 && SDL_GameControllerGetAttached(ControllerHandles[ControllerIndex]))
		{
			joyinput[ControllerIndex][JOYUP] = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_UP);
			joyinput[ControllerIndex][JOYDOWN] = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_DOWN);
			joyinput[ControllerIndex][JOYLEFT] = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_LEFT);
			joyinput[ControllerIndex][JOYRIGHT] = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
			joyinput[ControllerIndex][JOYSTART] = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_START);
			joyinput[ControllerIndex][JOYBACK] = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_BACK);
			joyinput[ControllerIndex][JOYLEFTSHOULDER] = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
			joyinput[ControllerIndex][JOYRIGHTSHOULDER] = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
			joyinput[ControllerIndex][JOYA] = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_A);
			joyinput[ControllerIndex][JOYB] = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_B);
			joyinput[ControllerIndex][JOYX] = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_X);
			joyinput[ControllerIndex][JOYY] = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_Y);

#ifdef __vita__
			{ extern void vita_sync_controls(void); vita_sync_controls(); }
#endif
			joyinputmap[ControllerIndex][FIRE] = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], (SDL_GameControllerButton)j_lookup[0]);
			joyinputmap[ControllerIndex][CHWEAPON] = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], (SDL_GameControllerButton)j_lookup[1]);
			joyinputmap[ControllerIndex][MEGABOMB] = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], (SDL_GameControllerButton)j_lookup[2]);
			joyinputmap[ControllerIndex][MEGAFIRE] = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], (SDL_GameControllerButton)j_lookup[3]);

			if (!g_drawcursor)
			{
#ifdef __vita__
				extern int vita_movestick[3]; extern int vita_eff_rot(void);
				int rs = vita_movestick[vita_eff_rot()];
				joyinput[ControllerIndex][JOYSTICKX] = IPT_ConvertAxisValue(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], rs ? SDL_CONTROLLER_AXIS_RIGHTX : SDL_CONTROLLER_AXIS_LEFTX), 10);
				joyinput[ControllerIndex][JOYSTICKY] = IPT_ConvertAxisValue(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], rs ? SDL_CONTROLLER_AXIS_RIGHTY : SDL_CONTROLLER_AXIS_LEFTY), 8);
#else
				joyinput[ControllerIndex][JOYSTICKX] = IPT_ConvertAxisValue(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTX), 10);
				joyinput[ControllerIndex][JOYSTICKY] = IPT_ConvertAxisValue(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTY), 8);
#endif
			}
			else
			{
#ifdef __vita__
				extern int vita_ptr_range(void); extern int vita_rstick;
				int vr = vita_ptr_range();
				int lx = IPT_ConvertAxisValue(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTX), vr);
				int ly = IPT_ConvertAxisValue(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTY), vr);
				if (!lx && !ly && vita_rstick)
				{
					lx = IPT_ConvertAxisValue(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_RIGHTX), vr);
					ly = IPT_ConvertAxisValue(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_RIGHTY), vr);
				}
				joyinput[ControllerIndex][JOYSTICKX] = lx;
				joyinput[ControllerIndex][JOYSTICKY] = ly;
#else
				joyinput[ControllerIndex][JOYSTICKX] = IPT_ConvertAxisValue(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTX), 4);
				joyinput[ControllerIndex][JOYSTICKY] = IPT_ConvertAxisValue(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTY), 4);
#endif
			}
			
#ifdef __vita__
			{   // portrait: rotate directions
				extern int vita_eff_rot(void);
				int vrot = vita_eff_rot();
				if (!g_drawcursor)
				{   // D-pad buttons mapped to actions are not used for flying
					for (int k = 0; k < 4; k++)
					{
						if (j_lookup[k] == SDL_CONTROLLER_BUTTON_DPAD_UP) joyinput[ControllerIndex][JOYUP] = 0;
						if (j_lookup[k] == SDL_CONTROLLER_BUTTON_DPAD_DOWN) joyinput[ControllerIndex][JOYDOWN] = 0;
						if (j_lookup[k] == SDL_CONTROLLER_BUTTON_DPAD_LEFT) joyinput[ControllerIndex][JOYLEFT] = 0;
						if (j_lookup[k] == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) joyinput[ControllerIndex][JOYRIGHT] = 0;
					}
				}
				if (vrot)
				{
					int ax = joyinput[ControllerIndex][JOYSTICKX], ay = joyinput[ControllerIndex][JOYSTICKY];
					int u = joyinput[ControllerIndex][JOYUP], d = joyinput[ControllerIndex][JOYDOWN];
					int l = joyinput[ControllerIndex][JOYLEFT], r = joyinput[ControllerIndex][JOYRIGHT];
					if (vrot == 1)   // image rotated clockwise
					{
						joyinput[ControllerIndex][JOYSTICKX] = ay; joyinput[ControllerIndex][JOYSTICKY] = -ax;
						joyinput[ControllerIndex][JOYUP] = r; joyinput[ControllerIndex][JOYRIGHT] = d;
						joyinput[ControllerIndex][JOYDOWN] = l; joyinput[ControllerIndex][JOYLEFT] = u;
					}
					else
					{
						joyinput[ControllerIndex][JOYSTICKX] = -ay; joyinput[ControllerIndex][JOYSTICKY] = ax;
						joyinput[ControllerIndex][JOYUP] = l; joyinput[ControllerIndex][JOYRIGHT] = u;
						joyinput[ControllerIndex][JOYDOWN] = r; joyinput[ControllerIndex][JOYLEFT] = d;
					}
				}
			}
#endif
			joyinput[ControllerIndex][JOYTRIGGERLEFT] = IPT_ConvertAxisValue(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_TRIGGERLEFT), 4);
			joyinput[ControllerIndex][JOYTRIGGERRIGHT] = IPT_ConvertAxisValue(SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_TRIGGERRIGHT), 4);
		}
		
		if (sdlevent->type == SDL_CONTROLLERBUTTONUP) 
			joy_ack = 0;
		
		if (sdlevent->type == SDL_CONTROLLERBUTTONDOWN) 
			joy_ack = 1;
	}
}

/***************************************************************************
IPT_CalJoyRumbleLow() - Gamecontroller rumbles low
 ***************************************************************************/
void 
IPT_CalJoyRumbleLow(
	void
)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
			SDL_GameControllerRumble(ControllerHandles[ControllerIndex], 0x3fff, 0x3fff, 1000);
	}
}

/***************************************************************************
IPT_CalJoyRumbleMedium() - Gamecontroller rumbles medium
 ***************************************************************************/
void 
IPT_CalJoyRumbleMedium(
	void
)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
		    SDL_GameControllerRumble(ControllerHandles[ControllerIndex], 0x7ffe, 0x7ffe, 1000);
	}
}

/***************************************************************************
IPT_CalJoyRumbleHigh() - Gamecontroller rumbles high
 ***************************************************************************/
void 
IPT_CalJoyRumbleHigh(
	void
)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
			SDL_GameControllerRumble(ControllerHandles[ControllerIndex], 0xbffd, 0xbffd, 1000);
	}
}

/***************************************************************************
JOY_Wait() - Waits for button to be released
 ***************************************************************************/
void 
JOY_Wait(
	int index,
	int button
)
{
	while (joyinput[index][button])
	{
		I_GetEvent();
	}
}

/***************************************************************************
JOY_IsKey() - Tests to see if button is down if so waits for release
 ***************************************************************************/
int 
JOY_IsKey(
	int button
)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (joyinput[ControllerIndex][button])
		{
			JOY_Wait(ControllerIndex, button);
			return 1;
		}
	}
	
	return 0;
}

/***************************************************************************
JOY_MapsInput() - Maps input to key
 ***************************************************************************/
int
JOY_MapsInput(
	void
)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (joyinput[ControllerIndex][JOYSTICKX] || joyinput[ControllerIndex][JOYSTICKY] ||
			joyinput[ControllerIndex][JOYUP] || joyinput[ControllerIndex][JOYDOWN] ||
			joyinput[ControllerIndex][JOYLEFT] || joyinput[ControllerIndex][JOYRIGHT] ||
			joyinput[ControllerIndex][JOYSTART] || joyinput[ControllerIndex][JOYBACK] ||
			joyinput[ControllerIndex][JOYA] || joyinput[ControllerIndex][JOYB] ||
			joyinput[ControllerIndex][JOYX] || joyinput[ControllerIndex][JOYY] ||
			joyinput[ControllerIndex][JOYRIGHTSHOULDER] || joyinput[ControllerIndex][JOYLEFTSHOULDER])
		{
			unsigned int currentTime;
			currentTime = SDL_GetTicks();

			if (currentTime > lastTime + 200)
			{
				lastTime = currentTime;

				if (joyinput[ControllerIndex][JOYSTICKX] > 0 ||
					joyinput[ControllerIndex][JOYRIGHT])
					return SC_RIGHT;
				if (joyinput[ControllerIndex][JOYSTICKX] < 0 ||
					joyinput[ControllerIndex][JOYLEFT])
					return SC_LEFT;
				if (joyinput[ControllerIndex][JOYSTICKY] > 0 ||
					joyinput[ControllerIndex][JOYDOWN])
					return SC_DOWN;
				if (joyinput[ControllerIndex][JOYSTICKY] < 0 ||
					joyinput[ControllerIndex][JOYUP])
					return SC_UP;
				if (joyinput[ControllerIndex][JOYA] ||
					joyinput[ControllerIndex][JOYSTART])
					return SC_ENTER;
				if (joyinput[ControllerIndex][JOYB] ||
					joyinput[ControllerIndex][JOYBACK])
					return SC_ESC;
				if (joyinput[ControllerIndex][JOYX])
					return SC_DELETE;
				if (joyinput[ControllerIndex][JOYLEFTSHOULDER])
					return SC_CTRL;
				if (joyinput[ControllerIndex][JOYRIGHTSHOULDER])
					return SC_F1;
			}
			return 0;
		}
	}
	return 0;
}

/***************************************************************************
JOY_GetInput() - Get input from joystick
 ***************************************************************************/
int
JOY_GetInput(
	void
)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (joyinput[ControllerIndex][JOYUP] || joyinput[ControllerIndex][JOYDOWN] ||
			joyinput[ControllerIndex][JOYLEFT] || joyinput[ControllerIndex][JOYRIGHT] ||
			joyinput[ControllerIndex][JOYSTART] || joyinput[ControllerIndex][JOYBACK] ||
			joyinput[ControllerIndex][JOYA] || joyinput[ControllerIndex][JOYB] ||
			joyinput[ControllerIndex][JOYX] || joyinput[ControllerIndex][JOYY] ||
			joyinput[ControllerIndex][JOYRIGHTSHOULDER] || joyinput[ControllerIndex][JOYLEFTSHOULDER])
		{
			return 1;
		}
	}
	return 0;
}

/***************************************************************************
JOY_GetMappedButton() - Get mapped button status from joystick
 ***************************************************************************/
int
JOY_GetMappedButton(
	int button
)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (joyinputmap[ControllerIndex][button])
			return 1;
	}
	return 0;
}

/***************************************************************************
JOY_GetButton() - Get button status from joystick
 ***************************************************************************/
int
JOY_GetButton(
	int button
)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (joyinput[ControllerIndex][button])
			return 1;
	}
	return 0;
}

/***************************************************************************
JOY_GetAxis() - Get axis status from joystick
 ***************************************************************************/
int
JOY_GetAxis(
	int axis
)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (joyinput[ControllerIndex][axis])
			return joyinput[ControllerIndex][axis];
	}
	return 0;
}