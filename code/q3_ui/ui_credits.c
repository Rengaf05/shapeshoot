#include "ui_local.h"

#define SCROLLSPEED 2.0f //scrolling speed in pixels per second

//#define BACKGROUND_SHADER "YOUR BACKGROUND SHADER HERE" //uncomment to use background shader else solid color

typedef struct {
	menuframework_s menu;
} creditsmenu_t;

static creditsmenu_t credits_menu;

int starttime; //game time at which credits are started
float mvolume; //volume backup

vec4_t color_crbackground = {1.0f, 1.0f, 1.0f, 1.0f};
vec4_t color_crtext = {0.0f, 0.0f, 0.0f, 1.0f};
vec4_t color_crfooter = {1.0f, 0.0f, 0.0f, 1.0f};

qhandle_t backgroundShader;

typedef struct {
	char *text;
	int style;
	vec4_t *colour;
} cr_line;

cr_line credits_lines[] = {
	//actual credits
	{"id Software is:", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"", UI_CENTER|UI_SMALLFONT, &color_crbackground},
	{"Programming", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"John Carmack, Robert A. Duffy, Jim Dose'", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"", UI_CENTER|UI_SMALLFONT, &color_crbackground},
	{"Art", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Adrian Carmack, Kevin Cloud,", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Kenneth Scott, Seneca Menard, Fred Nilsson", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"", UI_CENTER|UI_SMALLFONT, &color_crbackground},
	{"Game Designer", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Graeme Devine", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"", UI_CENTER|UI_SMALLFONT, &color_crbackground},
	{"Level Design", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Tim Willits, Christian Antkow, Paul Jaquays", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"", UI_CENTER|UI_SMALLFONT, &color_crbackground},
	{"CEO", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Todd Hollenshead", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"", UI_CENTER|UI_SMALLFONT, &color_crbackground},
	{"Director of Business Development", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Marty Stratton", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"", UI_CENTER|UI_SMALLFONT, &color_crbackground},
	{"Biz Assist and id Mom", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Donna Jackson", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"", UI_CENTER|UI_SMALLFONT, &color_crbackground},
	{"Development Assistance", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Eric Webb", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"", UI_CENTER|UI_SMALLFONT, &color_crbackground},
	{"To order: 1-800-idgames", UI_CENTER|UI_SMALLFONT, &color_crfooter},
	{"www.quake3arena.com www.idsoftware.com", UI_CENTER|UI_SMALLFONT, &color_crfooter},
	{"Quake III Arena(c) 1999-2000", UI_CENTER|UI_SMALLFONT, &color_crfooter},
	{"Id Software, Inc. All Rights Reserved", UI_CENTER|UI_SMALLFONT, &color_crfooter},
	{"", UI_CENTER|UI_SMALLFONT, &color_crbackground},
	{"ioquake3 contributors:", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"", UI_CENTER|UI_SMALLFONT, &color_crbackground},
	{"Tim Angus", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"James Canete", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Vincent Cojot", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Ryan C. Gordon", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Aaron Gyes", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Zack Middleton", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Ludwig Nussel", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Julian Priestley", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Scirocco Six", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Thilo Schulz", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Zachary J. Slater", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"Tony J. White", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"...and many, many others!", UI_CENTER|UI_SMALLFONT, &color_crtext},
	{"", UI_CENTER|UI_SMALLFONT, &color_crbackground},
	{"http://www.ioquake3.org/", UI_CENTER|UI_SMALLFONT, &color_crfooter},
	{NULL}
};

/*
=================
UI_CreditMenu_Key
=================
*/
static sfxHandle_t UI_CreditMenu_Key(int key) {
	//pressing esc or clicking mouse will exit, we also reset music volume
	if (key & K_CHAR_FLAG) {
		return 0;
	}
	
	trap_Cmd_ExecuteText(EXEC_APPEND, va("s_musicvolume %f; quit\n", mvolume)); 
	return 0;
}

/*
===============
Credits_Draw
===============
*/
static void Credits_Draw(void) {
	int x = SCREEN_WIDTH / 2, y, i;
	
	//draw background
#ifdef BACKGROUND_SHADER
	UI_DrawHandlePic(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, backgroundShader);
#else
	UI_FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color_crbackground);
#endif
	//main drawing
	//intial y
	y = SCREEN_HEIGHT - SCROLLSPEED * (float)(uis.realtime - starttime) / 100;
	
	//draw the credits
	for(i = 0; credits_lines[i].text; ++i) {
		//draw if string is not null and within screen
		if (*credits_lines[i].text && (y > -(PROP_HEIGHT * (1 / PROP_SMALL_SIZE_SCALE))))
			UI_DrawProportionalString(x, y, credits_lines[i].text, credits_lines[i].style, *credits_lines[i].colour);
		//adjust y position
		if (credits_lines[i].style & UI_SMALLFONT)
			y += PROP_HEIGHT * PROP_SMALL_SIZE_SCALE;
		else if (credits_lines[i].style & UI_BIGFONT)
			y += PROP_HEIGHT;
		else if (credits_lines[i].style & UI_GIANTFONT)
			y += PROP_HEIGHT * (1/PROP_SMALL_SIZE_SCALE);

		//break if out of screen
		if (y > SCREEN_HEIGHT) break;
	}
	//reset volume and quit if entire credits are of screen
	if (y < -16) trap_Cmd_ExecuteText(EXEC_APPEND, va("s_musicvolume %f; quit\n", mvolume));
}

/*
===============
UI_CreditMenu
===============
*/
void UI_CreditMenu(void) {
	//clear menu struct
	memset(&credits_menu, 0, sizeof(credits_menu));
	
	//init menu
	credits_menu.menu.draw = Credits_Draw;
	credits_menu.menu.key = UI_CreditMenu_Key;
	credits_menu.menu.fullscreen = qtrue;
	UI_PushMenu(&credits_menu.menu);

	//save time
	starttime = uis.realtime;

	//setup music
	mvolume = trap_Cvar_VariableValue("s_musicvolume");
	if (mvolume < 0.5)
		trap_Cmd_ExecuteText(EXEC_APPEND, "s_musicvolume 0.5\n");
	trap_Cmd_ExecuteText(EXEC_APPEND, "music music/fla22k_02\n");

	//load backgorund shader
#ifdef BACKGROUND_SHADER
	backgroundShader = trap_R_RegisterShaderNoMip(BACKGROUND_SHADER);
#endif
}
