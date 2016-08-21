#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "API.h"
#include "lcdControl.h"
#include "util.h"
#include "timer.h"

//LCD thread wait time in ms
static unsigned long lcdThreadWaitTime = 100;

//Menu for current selection
menu *currentMenu = NULL;

//Backlight blink rate (in Hz)
static unsigned int backlightBlinkRate = 0;
static bool lcdCurrentBacklight = true;

/**
 * Allocates and initializes a menu
 *
 * @param msg String for the menu to display on the LCD
 */
menu *newMenu(char *msg)
{
	menu *menu = malloc(sizeof(struct menu_t));

	if (menu == NULL)
	{
		printf("LCD malloc failed");
		return currentMenu;
	}
	else
	{
		//Set current menu to first allocated menu to give a starting point
		if (currentMenu == NULL)
		{
			currentMenu = menu;
		}

		//Initialize menu
		menu->next = NULL;
		menu->prev = NULL;
		menu->up = NULL;
		menu->down = NULL;
		menu->msg = msg;
		menu->hasInvoke = false;
		menu->invoke = NULL;

		return menu;
	}
}

/**
 * Allocates and initializes a menu
 *
 * @param msg String for the menu to display on the LCD
 * @param dispatchFunction Function to invoke for this menu
 */
menu* newMenuWithDispatch(char *msg, void (*dispatchFunction)())
{
	menu *menu = malloc(sizeof(struct menu_t));

	if (menu == NULL)
	{
		printf("LCD malloc failed");
		return currentMenu;
	}
	else
	{
		//Set current menu to first allocated menu to give a starting point
		if (currentMenu == NULL)
		{
			currentMenu = menu;
		}

		//Initialize menu
		menu->next = NULL;
		menu->prev = NULL;
		menu->up = NULL;
		menu->down = NULL;
		menu->msg = msg;
		menu->hasInvoke = true;
		menu->invoke = dispatchFunction;

		return menu;
	}
}

/**
 * Sets a blink rate (in Hz) for the lcd backlight
 *
 * @param rate LCD backlight blink rate (in Hz)
 */
void setBacklightBlinkRate(const unsigned int rate)
{
	backlightBlinkRate = rate;
}

/**
 * Changes the dispatch function of a menu
 *
 * @param menu The menu
 * @param func New dispatch function
 */
void changeFunction(menu *menu, void (*func)())
{
	menu->invoke = func;
}

/**
 * Changes the message of a menu
 *
 * @param menu The menu
 * @param msg New message
 */
void changeMessage(menu *menu, char *msg)
{
	menu->msg = msg;
}

/**
 * Childs menus to a parent
 *
 * @param parent Parent menu to gain children
 * @param childCount The number of menus specified, excluding `parent`
 * @param ... Menus to become children of `parent`
 */
void formLevel(menu *parent, const unsigned int childCount, ...)
{
	va_list valist;
	va_start (valist, childCount);

	//Temporary variable for valist
	menu *tmp;

	//Pull menu out of valist and link it
	for (int i = 0; i < childCount; i++)
	{
		tmp = va_arg(valist, menu*);
		tmp->up = parent;
		parent->down = tmp;
	}

	va_end(valist);
}

/**
 * Inserts a menu into a row
 *
 * @param m Menu to insert
 * @param next Menu to the right of `m`
 * @param prev Menu to the left of `m`
 */
void putMenuInRow(menu *m, menu *next, menu *prev)
{
	m->next = next;
	m->prev = prev;
}

/**
 * Pairs menus
 *
 * @param count The number of menus specified
 * @param ... Menus to link (of type `menu*`)
 */
void linkMenus(const unsigned int count, ...)
{
	va_list valist;
	va_start(valist, count);

	//Temporary array for menus in valist
	menu *tmp[count];

	//Pull menus out of valist
	for (int i = 0; i < count; i++)
	{
		tmp[i] = va_arg(valist, menu*);
	}

	//Link menus
	for (int i = 0; i < count - 1; i++)
	{
		if (i == 0)
		{
			tmp[i]->next = tmp[i + 1];
			tmp[i]->prev = tmp[count - 1];
		}
		else if (i == count - 1)
		{
			tmp[i]->next = tmp[0];
			tmp[i]->prev = tmp[i - 1];
		}
		else
		{
			tmp[i]->next = tmp[i + 1];
			tmp[i]->prev = tmp[i - 1];
		}
	}

	va_end(valist);
}

/**
 * Updates the LCD and responds to button presses
 */
static void updateLCDThread()
{
	//Timer for backlight blink
	timer backlightTimer;
	timer_Initialize(&backlightTimer);

	lcdInit(uart1);
	lcdClear(uart1);
	lcdSetText(uart1, 1, currentMenu->msg);
	lcdSetText(uart1, 2, SUBMENU_SELECT);

	while (true)
	{
		//Blink LCD backlight at set rate (in Hz)
		if (backlightBlinkRate == 0)
		{
			lcdSetBacklight(uart1, true);
		}
		else if (timer_Repeat(&backlightTimer, 1000.0 / backlightBlinkRate))
		{
			lcdSetBacklight(uart1, !lcdCurrentBacklight);
			lcdCurrentBacklight = !lcdCurrentBacklight;
		}

		//Left button
		if (lcdReadButtons(uart1) == LCD_BTN_LEFT)
		{
			if (currentMenu->prev != NULL)
			{
				currentMenu = currentMenu->prev;
				waitForLCDRelease();
			}
		}
		//Right button
		else if (lcdReadButtons(uart1) == LCD_BTN_RIGHT)
		{
			if (currentMenu->next != NULL)
			{
				currentMenu = currentMenu->next;
				waitForLCDRelease();
			}
		}
		//Center button
		else if (lcdReadButtons(uart1) == LCD_BTN_CENTER)
		{
			//Let the user hold the center button
			delay(250);

			//Center held
			if (lcdReadButtons(uart1) == LCD_BTN_CENTER)
			{
				//If a higher menu exists
				if (currentMenu->up != NULL)
				{
					currentMenu = currentMenu->up;

					lcdPrint(uart1, 1, currentMenu->msg);
					lcdPrint(uart1, 2, SUBMENU_SELECT);

					waitForLCDRelease();
				}
			}
			//Center not held
			else
			{
				//If a lower menu exists
				if (currentMenu->down != NULL)
				{
					currentMenu = currentMenu->down;
					waitForLCDRelease();
				}
				//No lower menu exists, check if there is a function
				else if (currentMenu->hasInvoke)
				{
					//A function exists, execute it
					currentMenu->invoke();

					waitForLCDRelease();
				}
			}
		}

		lcdPrint(uart1, 1, currentMenu->msg);
		lcdPrint(uart1, 2, SUBMENU_SELECT);

		//Slow loop time to minimize impact on other threads
		//Lower loop time for more responsive controls
		delay(lcdThreadWaitTime);
	}

	//Clear menu once the user is done
	lcdClear(uart1);
}

/*
 * Starts the LCD thread
 */
void startUpdateLCDThread()
{
	taskCreate(updateLCDThread, TASK_DEFAULT_STACK_SIZE, NULL, TASK_PRIORITY_DEFAULT);
}
