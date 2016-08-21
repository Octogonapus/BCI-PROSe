#ifndef LCDCONTROL_H_
#define LCDCONTROL_H_

#include <stdbool.h>
#include <stdarg.h>

//Default text
#define DEFAULT_LINE       "<<            >>"
#define SUBMENU_SELECT     "<<   SELECT   >>"
#define EXIT_MENU_SELECT   "<<     OK     >>"
#define SUBMENU_BACK       "<<    BACK    >>"
#define SAFETY_TRIG        "   SAFETY_TRIG  "
#define INCORRECT_MENU_NUM "  BAD MENU_NUM  "

//Menu representation
typedef struct menu_t
{
	//Pointer to next menu (right button)
	struct menu_t *next;
	//Pointer to previous menu (left button)
	struct menu_t *prev;
	//Pointer to higher menu (center button)
	struct menu_t *up;
	//Pointer to deeper menu (center button)
	struct menu_t *down;

	//Displayed text
	char *msg;

	//Dispatch function
	bool hasInvoke;
	void (*invoke)();
} menu;

/**
 * Allocates and initializes a menu
 *
 * @param msg String for the menu to display on the LCD
 */
menu *newMenu(char *msg);

/**
 * Allocates and initializes a menu
 *
 * @param msg String for the menu to display on the LCD
 * @param dispatchFunction Function to invoke for this menu
 */
menu* newMenuWithDispatch(char *msg, void (*dispatchFunction)());

/**
 * Sets a blink rate (in Hz) for the lcd backlight
 *
 * @param rate LCD backlight blink rate (in Hz)
 */
inline void setBacklightBlinkRate(const unsigned int rate);

/**
 * Changes the dispatch function of a menu
 *
 * @param menu The menu
 * @param func New dispatch function
 */
inline void changeFunction(menu *menu, void (*func)());

/**
 * Changes the message of a menu
 *
 * @param menu The menu
 * @param msg New message
 */
inline void changeMessage(menu *menu, char *msg);

/**
 * Childs menus to a parent
 *
 * @param parent Parent menu to gain children
 * @param childCount The number of menus specified, excluding `parent`
 * @param ... Menus to become children of `parent`
 */
void formLevel(menu *parent, const unsigned int childCount, ...);

/**
 * Inserts a menu into a row
 *
 * @param m Menu to insert
 * @param next Menu to the right of `m`
 * @param prev Menu to the left of `m`
 */
inline void putMenuInRow(menu *m, menu *next, menu *prev);

/**
 * Pairs menus
 *
 * @param count The number of menus specified
 * @param ... Menus to link (of type `menu*`)
 */
void linkMenus(const unsigned int count, ...);

/**
 * Updates the LCD and responds to button presses
 */
inline void startUpdateLCDThread();

#endif
