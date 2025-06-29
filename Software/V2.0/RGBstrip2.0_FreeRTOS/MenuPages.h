/// place for menu pages

#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0]) // macro for finding length of already made array

short int scene = 1;
short int cursor = 0;

struct Scene {
  const String* items;
  int length;
};

/*
scenes:
0 - none (for running other pgorams that don't need a menu)
1 - main menu
*/

const String mainMenu[] = {
  "1. System Status",
  "2. Lightning",
  "3. System Settings",
  "4. Serial Settings"
};


const Scene scenes[] = {
  { nullptr, 0 }, // scene 0
  { mainMenu, ARRAY_SIZE(mainMenu) },
};

//more scenes comming soon