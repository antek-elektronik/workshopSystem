/// place for menu pages

#define ARRAY_SIZE(x) sizeof(x)/sizeof(x[0]) // macro for finding length of already made array


short int cursor = 0;

struct Scene {
  const String* items;
  int length;
};

const String mainMenu[] = {
  "1. System Status",
  "2. Lightning",
  "3. System Settings",
  "4. Serial Settings"
};

const String lightningMenu[] = {
  "1.manual",
  "2.auto modes",
  "3.tests",
  "4.back"
};

const String systemStatusMenu[] = {
  "1.back",
};


const Scene scenes[] = {
  { nullptr, 0 }, // scene 0
  { mainMenu, ARRAY_SIZE(mainMenu) }, //scene 1, main menu
  { systemStatusMenu, ARRAY_SIZE(systemStatusMenu) },
  { lightningMenu, ARRAY_SIZE(lightningMenu) },
};

Scene activeScene = scenes[1];

int findSceneNumber(String element) {
  for (int i = 0; i < ARRAY_SIZE(scenes); i++) {
      if (scenes[i].items == &element) {
        return i;
      }
  }
}

int findSceneNumber(Scene element){
  for (int i = 0; i < ARRAY_SIZE(scenes); i++) {
      if (scenes[i].items == &element.items) {
        return i;
      }
  }
}

//more scenes comming soon