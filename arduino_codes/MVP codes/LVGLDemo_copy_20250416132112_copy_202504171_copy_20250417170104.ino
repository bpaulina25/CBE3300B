#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"
#include "lvgl.h"

Arduino_H7_Video          Display(800, 480, GigaDisplayShield);
Arduino_GigaDisplayTouch  TouchDetector;

bool introScreenActive = true;
uint32_t lastTapTime = 0;
int tapCount = 0;

lv_obj_t * concentrationLabel;
lv_obj_t * loadingBar;
lv_obj_t * infoLabel;

/* Forward declarations */
void loadMainUIScreen();
void loadIntroScreen();
void resetToIntro();
void refreshConcentration();

/* Button callbacks */
static void btn_event_cb(lv_event_t * e) {
  static uint32_t cnt = 1;
  lv_obj_t * btn = (lv_obj_t *)lv_event_get_target(e);
  lv_obj_t * label = lv_obj_get_child(btn, 0);
  lv_label_set_text_fmt(label, "%%" LV_PRIu32, cnt);
  cnt++;
}

static void reset_btn_event_cb(lv_event_t * e) {
  resetToIntro();
}

static void refresh_btn_event_cb(lv_event_t * e) {
  lv_bar_set_value(loadingBar, 0, LV_ANIM_OFF);
  lv_obj_clear_flag(loadingBar, LV_OBJ_FLAG_HIDDEN);

  lv_anim_t a;
  lv_anim_init(&a);
  lv_anim_set_exec_cb(&a, [](void * bar, int32_t val) {
    lv_bar_set_value((lv_obj_t *)bar, val, LV_ANIM_ON);
  });
  lv_anim_set_time(&a, 3000);
  lv_anim_set_var(&a, loadingBar);
  lv_anim_set_values(&a, 0, 100);
  lv_anim_set_repeat_count(&a, 1);
  lv_anim_start(&a);

  refreshConcentration();
}

void loadIntroScreen() {
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_white(), LV_PART_MAIN);
  lv_obj_t * introLabel = lv_label_create(lv_scr_act());
  lv_label_set_text(introLabel, "Tap twice to start");
  lv_obj_center(introLabel);
}

void resetToIntro() {
  introScreenActive = true;
  tapCount = 0;
  lv_obj_clean(lv_scr_act());
  loadIntroScreen();
}

void setup() {
  Serial.begin(115200);
  Display.begin();
  TouchDetector.begin();
  loadIntroScreen();
}

void loop() {
  lv_timer_handler();

  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();

    if (line.length() > 0) {
      float val = line.toFloat();
      lv_label_set_text_fmt(concentrationLabel, "Concentration: %s mg/dL", line.c_str());


      if (val < 70) {
        lv_obj_set_style_bg_color(lv_obj_get_parent(infoLabel), lv_color_hex(0xFFF799), LV_PART_MAIN);
        lv_label_set_text(infoLabel, "Concentration is too low.");
      } else if (val < 100) {
        lv_obj_set_style_bg_color(lv_obj_get_parent(infoLabel), lv_color_hex(0x99FF99), LV_PART_MAIN);
        lv_label_set_text(infoLabel, "Normal glucose concentration,\nFasting blood, non diabetes");
      } else if (val < 126) {
        lv_obj_set_style_bg_color(lv_obj_get_parent(infoLabel), lv_color_hex(0xFF6666), LV_PART_MAIN);
        lv_label_set_text(infoLabel, "High glucose on blood, prediabetes.");
      } else if (val <= 250) {
        lv_obj_set_style_bg_color(lv_obj_get_parent(infoLabel), lv_color_hex(0xCC0000), LV_PART_MAIN);
        lv_label_set_text(infoLabel, "Super high glucose on blood, diabetes");
      } else {
        lv_obj_set_style_bg_color(lv_obj_get_parent(infoLabel), lv_color_hex(0x990000), LV_PART_MAIN);
        lv_label_set_text(infoLabel, "Dangerous concentration of glucose on blood");
      }

      Serial.print("Updated concentration to: ");
      Serial.println(val);
    }
  }

  if (introScreenActive) {
    GDTpoint_t touchPoints[5];
    uint8_t numPoints = TouchDetector.getTouchPoints(touchPoints);

    if (numPoints > 0) {
      uint32_t now = millis();
      if (now - lastTapTime < 1500) {
        tapCount++;
      } else {
        tapCount = 1;
      }

      lastTapTime = now;

      if (tapCount >= 2) {
        introScreenActive = false;
        lv_obj_clean(lv_scr_act());
        loadMainUIScreen();
      }

      delay(200);
    }
  }
}

void loadMainUIScreen() {
  static lv_coord_t col_dsc[] = {370, 370, LV_GRID_TEMPLATE_LAST};
  static lv_coord_t row_dsc[] = {215, 215, LV_GRID_TEMPLATE_LAST};
  lv_obj_t * cont = lv_obj_create(lv_scr_act());
  lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
  lv_obj_set_size(cont, Display.width(), Display.height());
  lv_obj_set_style_bg_color(cont, lv_color_hex(0x03989e), LV_PART_MAIN);
  lv_obj_center(cont);

  lv_obj_t * label;
  lv_obj_t * obj;

  obj = lv_obj_create(cont);
  lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 0, 1,
                       LV_GRID_ALIGN_STRETCH, 0, 1);
  concentrationLabel = lv_label_create(obj);
  lv_label_set_text_fmt(concentrationLabel, "Concentration: --");
  lv_obj_center(concentrationLabel);

  obj = lv_obj_create(cont);
  lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 1, 1,
                       LV_GRID_ALIGN_STRETCH, 0, 1);
  lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_size(obj, 360, 220);

  const char *names[] = {"Kayla", "Pau", "Theertha", "Osuji", "Joe", "Extra usher"};
  for (int i = 0; i < 6; i++) {
    lv_obj_t * cb = lv_checkbox_create(obj);
    lv_checkbox_set_text(cb, names[i]);
    if (i == 1 || i == 3) lv_obj_add_state(cb, LV_STATE_CHECKED);
  }

  lv_obj_t * btn = lv_btn_create(obj);
  lv_obj_set_size(btn, 100, 40);
  lv_obj_center(btn);
  lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);
  label = lv_label_create(btn);
  lv_label_set_text(label, "Click me!");
  lv_obj_center(label);

  obj = lv_obj_create(cont);
  lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_CENTER, 0, 1,
                       LV_GRID_ALIGN_CENTER, 1, 1);
  lv_obj_set_size(obj, 360, 220);
  infoLabel = lv_label_create(obj);
  lv_label_set_text(infoLabel, "Waiting for reading...");
  lv_obj_align(infoLabel, LV_ALIGN_CENTER, 0, 0);

  obj = lv_obj_create(cont);
  lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_CENTER, 1, 1,
                       LV_GRID_ALIGN_CENTER, 1, 1);
  lv_obj_set_size(obj, 360, 220);

  loadingBar = lv_bar_create(obj);
  lv_obj_set_size(loadingBar, 200, 20);
  lv_obj_align(loadingBar, LV_ALIGN_CENTER, 0, 20);
  lv_bar_set_value(loadingBar, 0, LV_ANIM_OFF);
  lv_obj_add_flag(loadingBar, LV_OBJ_FLAG_HIDDEN);

  lv_obj_t * refreshBtn = lv_btn_create(obj);
  lv_obj_set_size(refreshBtn, 140, 40);
  lv_obj_align(refreshBtn, LV_ALIGN_BOTTOM_MID, -80, -10);
  lv_obj_add_event_cb(refreshBtn, refresh_btn_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t * refreshLabel = lv_label_create(refreshBtn);
  lv_label_set_text(refreshLabel, "Refresh Reading");
  lv_obj_center(refreshLabel);

  lv_obj_t * resetBtn = lv_btn_create(obj);
  lv_obj_set_size(resetBtn, 140, 40);
  lv_obj_align(resetBtn, LV_ALIGN_BOTTOM_MID, 80, -10);
  lv_obj_add_event_cb(resetBtn, reset_btn_event_cb, LV_EVENT_CLICKED, NULL);
  lv_obj_t * resetLabel = lv_label_create(resetBtn);
  lv_label_set_text(resetLabel, "Home Screen");
  lv_obj_center(resetLabel);
}

void refreshConcentration() {
  lv_label_set_text(concentrationLabel, "Concentration: --");
  lv_label_set_text(infoLabel, "Waiting for reading...");
  lv_obj_set_style_bg_color(lv_obj_get_parent(infoLabel), lv_color_white(), LV_PART_MAIN);
}



//////this one works with the potentiostat, receiving the number from it and displaying it.


// #include "Arduino_H7_Video.h"
// #include "Arduino_GigaDisplayTouch.h"
// #include "lvgl.h"

// Arduino_H7_Video          Display(800, 480, GigaDisplayShield);
// Arduino_GigaDisplayTouch  TouchDetector;

// bool introScreenActive = true;
// uint32_t lastTapTime = 0;
// int tapCount = 0;

// lv_obj_t * concentrationLabel;
// lv_obj_t * loadingBar;

// /* Forward declarations */
// void loadMainUIScreen();
// void loadIntroScreen();
// void resetToIntro();
// void refreshConcentration();

// /* Button callbacks */
// static void btn_event_cb(lv_event_t * e) {
//   static uint32_t cnt = 1;
//   lv_obj_t * btn = (lv_obj_t *)lv_event_get_target(e);
//   lv_obj_t * label = lv_obj_get_child(btn, 0);
//   lv_label_set_text_fmt(label, "%"LV_PRIu32, cnt);
//   cnt++;
// }

// static void reset_btn_event_cb(lv_event_t * e) {
//   resetToIntro();
// }

// static void refresh_btn_event_cb(lv_event_t * e) {
//   refreshConcentration();
// }

// void loadIntroScreen() {
//   lv_obj_set_style_bg_color(lv_scr_act(), lv_color_white(), LV_PART_MAIN);
//   lv_obj_t * introLabel = lv_label_create(lv_scr_act());
//   lv_label_set_text(introLabel, "Tap twice to start");
//   lv_obj_center(introLabel);
// }

// void resetToIntro() {
//   introScreenActive = true;
//   tapCount = 0;
//   lv_obj_clean(lv_scr_act());
//   loadIntroScreen();
// }

// void setup() {
//   Serial.begin(115200);
//   Display.begin();
//   TouchDetector.begin();
//   loadIntroScreen();
// }

// void loop() {
//   lv_timer_handler();

//   // ✅ Handle CSV values sent from Python via Serial
//   if (Serial.available()) {
//     String line = Serial.readStringUntil('\n');
//     line.trim();

//     if (line.length() > 0) {
//       lv_label_set_text_fmt(concentrationLabel, "Concentration: %s", line.c_str());
//       Serial.print("Updated concentration to: ");
//       Serial.println(line);
//     }
//   }

//   // Handle double-tap to start
//   if (introScreenActive) {
//     GDTpoint_t touchPoints[5];
//     uint8_t numPoints = TouchDetector.getTouchPoints(touchPoints);

//     if (numPoints > 0) {
//       uint32_t now = millis();
//       if (now - lastTapTime < 1500) {
//         tapCount++;
//       } else {
//         tapCount = 1;
//       }

//       lastTapTime = now;

//       if (tapCount >= 2) {
//         introScreenActive = false;
//         lv_obj_clean(lv_scr_act());
//         loadMainUIScreen();
//       }

//       delay(200);
//     }
//   }
// }

// void loadMainUIScreen() {
//   static lv_coord_t col_dsc[] = {370, 370, LV_GRID_TEMPLATE_LAST};
//   static lv_coord_t row_dsc[] = {215, 215, LV_GRID_TEMPLATE_LAST};
//   lv_obj_t * cont = lv_obj_create(lv_scr_act());
//   lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
//   lv_obj_set_size(cont, Display.width(), Display.height());
//   lv_obj_set_style_bg_color(cont, lv_color_hex(0x03989e), LV_PART_MAIN);
//   lv_obj_center(cont);

//   lv_obj_t * label;
//   lv_obj_t * obj;

//   // [0;0] - Concentration display
//   obj = lv_obj_create(cont);
//   lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 0, 1,
//                        LV_GRID_ALIGN_STRETCH, 0, 1);
//   concentrationLabel = lv_label_create(obj);
//   lv_label_set_text_fmt(concentrationLabel, "Concentration: --");
//   lv_obj_center(concentrationLabel);

//   // [1;0] - Checkboxes and counter button
//   obj = lv_obj_create(cont);
//   lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 1, 1,
//                        LV_GRID_ALIGN_STRETCH, 0, 1);
//   lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);

//   const char *names[] = {"Kayla", "Pau", "Theertha", "Osuji", "Joe", "Extra usher"};
//   for (int i = 0; i < 6; i++) {
//     lv_obj_t * cb = lv_checkbox_create(obj);
//     lv_checkbox_set_text(cb, names[i]);
//     if (i == 1 || i == 3) lv_obj_add_state(cb, LV_STATE_CHECKED);
//   }

//   lv_obj_t * btn = lv_btn_create(obj);
//   lv_obj_set_size(btn, 100, 40);
//   lv_obj_center(btn);
//   lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);
//   label = lv_label_create(btn);
//   lv_label_set_text(label, "Click me!");
//   lv_obj_center(label);

//   // [0;1] - Reset button
//   obj = lv_obj_create(cont);
//   lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_CENTER, 0, 1,
//                        LV_GRID_ALIGN_CENTER, 1, 1);
//   lv_obj_t * resetBtn = lv_btn_create(obj);
//   lv_obj_set_size(resetBtn, 120, 50);
//   lv_obj_add_event_cb(resetBtn, reset_btn_event_cb, LV_EVENT_CLICKED, NULL);
//   label = lv_label_create(resetBtn);
//   lv_label_set_text(label, "Reset");
//   lv_obj_center(label);

//   // [1;1] - "Running Sample" label and loading bar
//   obj = lv_obj_create(cont);
//   lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_CENTER, 1, 1,
//                        LV_GRID_ALIGN_CENTER, 1, 1);
//   label = lv_label_create(obj);
//   lv_label_set_text(label, "Running Sample");
//   lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

//   loadingBar = lv_bar_create(obj);
//   lv_obj_set_size(loadingBar, 200, 20);
//   lv_obj_align(loadingBar, LV_ALIGN_CENTER, 0, 20);
//   lv_bar_set_value(loadingBar, 70, LV_ANIM_OFF);

//   lv_anim_t a;
//   lv_anim_init(&a);
//   lv_anim_set_exec_cb(&a, [](void * bar, int32_t val) {
//     lv_bar_set_value((lv_obj_t *)bar, val, LV_ANIM_ON);
//   });
//   lv_anim_set_time(&a, 3000);
//   lv_anim_set_playback_time(&a, 3000);
//   lv_anim_set_var(&a, loadingBar);
//   lv_anim_set_values(&a, 0, 100);
//   lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
//   lv_anim_start(&a);

//   // Refresh button
//   lv_obj_t * refreshBtn = lv_btn_create(obj);
//   lv_obj_set_size(refreshBtn, 120, 40);
//   lv_obj_align(refreshBtn, LV_ALIGN_BOTTOM_MID, 0, -10);
//   lv_obj_add_event_cb(refreshBtn, refresh_btn_event_cb, LV_EVENT_CLICKED, NULL);
//   lv_obj_t * refreshLabel = lv_label_create(refreshBtn);
//   lv_label_set_text(refreshLabel, "Refresh CSV");
//   lv_obj_center(refreshLabel);
// }

// void refreshConcentration() {
//   lv_label_set_text_fmt(concentrationLabel, "Concentration: --");
// }


///SDS card and fake concentration

// #include "Arduino_H7_Video.h"
// #include "Arduino_GigaDisplayTouch.h"
// #include "lvgl.h"

// Arduino_H7_Video          Display(800, 480, GigaDisplayShield);
// Arduino_GigaDisplayTouch  TouchDetector;

// bool introScreenActive = true;
// uint32_t lastTapTime = 0;
// int tapCount = 0;

// lv_obj_t * concentrationLabel;
// lv_obj_t * loadingBar;

// /* Simulated CSV value */
// String simulatedCSV = "42.7";

// /* Forward declarations */
// void loadMainUIScreen();
// void loadIntroScreen();
// String readConcentrationFromCSV();
// void resetToIntro();
// void refreshConcentration();

// /* Button callbacks */
// static void btn_event_cb(lv_event_t * e) {
//   static uint32_t cnt = 1;
//   lv_obj_t * btn = (lv_obj_t *)lv_event_get_target(e);
//   lv_obj_t * label = lv_obj_get_child(btn, 0);
//   lv_label_set_text_fmt(label, "%"LV_PRIu32, cnt);
//   cnt++;
// }

// static void reset_btn_event_cb(lv_event_t * e) {
//   resetToIntro();
// }

// static void refresh_btn_event_cb(lv_event_t * e) {
//   refreshConcentration();
// }

// void loadIntroScreen() {
//   lv_obj_set_style_bg_color(lv_scr_act(), lv_color_white(), LV_PART_MAIN);
//   lv_obj_t * introLabel = lv_label_create(lv_scr_act());
//   lv_label_set_text(introLabel, "Tap twice to start");
//   lv_obj_center(introLabel);
// }

// void resetToIntro() {
//   introScreenActive = true;
//   tapCount = 0;
//   lv_obj_clean(lv_scr_act());
//   loadIntroScreen();
// }

// void setup() {
//   Serial.begin(115200);
//   Display.begin();
//   TouchDetector.begin();
//   loadIntroScreen();
// }

// void loop() {
//   lv_timer_handler();

//   if (introScreenActive) {
//     GDTpoint_t touchPoints[5];
//     uint8_t numPoints = TouchDetector.getTouchPoints(touchPoints);

//     if (numPoints > 0) {
//       uint32_t now = millis();
//       if (now - lastTapTime < 1500) {
//         tapCount++;
//       } else {
//         tapCount = 1;
//       }

//       lastTapTime = now;

//       if (tapCount >= 2) {
//         introScreenActive = false;
//         lv_obj_clean(lv_scr_act());
//         loadMainUIScreen();
//       }

//       delay(200);
//     }
//   }
// }

// void loadMainUIScreen() {
//   static lv_coord_t col_dsc[] = {370, 370, LV_GRID_TEMPLATE_LAST};
//   static lv_coord_t row_dsc[] = {215, 215, LV_GRID_TEMPLATE_LAST};
//   lv_obj_t * cont = lv_obj_create(lv_scr_act());
//   lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
//   lv_obj_set_size(cont, Display.width(), Display.height());
//   lv_obj_set_style_bg_color(cont, lv_color_hex(0x03989e), LV_PART_MAIN);
//   lv_obj_center(cont);

//   lv_obj_t * label;
//   lv_obj_t * obj;

//   // [0;0] - Concentration display
//   obj = lv_obj_create(cont);
//   lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 0, 1,
//                        LV_GRID_ALIGN_STRETCH, 0, 1);
//   concentrationLabel = lv_label_create(obj);
//   lv_label_set_text_fmt(concentrationLabel, "Concentration: %s", readConcentrationFromCSV().c_str());
//   lv_obj_center(concentrationLabel);

//   // [1;0] - Checkboxes and counter button
//   obj = lv_obj_create(cont);
//   lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 1, 1,
//                        LV_GRID_ALIGN_STRETCH, 0, 1);
//   lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);

//   const char *names[] = {"Kayla", "Pau", "Theertha", "Osuji", "Joe", "Extra usher"};
//   for (int i = 0; i < 6; i++) {
//     lv_obj_t * cb = lv_checkbox_create(obj);
//     lv_checkbox_set_text(cb, names[i]);
//     if (i == 1 || i == 3) lv_obj_add_state(cb, LV_STATE_CHECKED);
//   }

//   lv_obj_t * btn = lv_btn_create(obj);
//   lv_obj_set_size(btn, 100, 40);
//   lv_obj_center(btn);
//   lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);
//   label = lv_label_create(btn);
//   lv_label_set_text(label, "Click me!");
//   lv_obj_center(label);

//   // [0;1] - Reset button
//   obj = lv_obj_create(cont);
//   lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_CENTER, 0, 1,
//                        LV_GRID_ALIGN_CENTER, 1, 1);
//   lv_obj_t * resetBtn = lv_btn_create(obj);
//   lv_obj_set_size(resetBtn, 120, 50);
//   lv_obj_add_event_cb(resetBtn, reset_btn_event_cb, LV_EVENT_CLICKED, NULL);
//   label = lv_label_create(resetBtn);
//   lv_label_set_text(label, "Reset");
//   lv_obj_center(label);

//   // [1;1] - "Running Sample" label + loading bar
//   obj = lv_obj_create(cont);
//   lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_CENTER, 1, 1,
//                        LV_GRID_ALIGN_CENTER, 1, 1);
//   label = lv_label_create(obj);
//   lv_label_set_text(label, "Running Sample");
//   lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

//   loadingBar = lv_bar_create(obj);
//   lv_obj_set_size(loadingBar, 200, 20);
//   lv_obj_align(loadingBar, LV_ALIGN_CENTER, 0, 20);
//   lv_bar_set_value(loadingBar, 70, LV_ANIM_OFF);

//   lv_anim_t a;
//   lv_anim_init(&a);
//   lv_anim_set_exec_cb(&a, [](void * bar, int32_t val) {
//     lv_bar_set_value((lv_obj_t *)bar, val, LV_ANIM_ON);
//   });
//   lv_anim_set_time(&a, 3000);
//   lv_anim_set_playback_time(&a, 3000);
//   lv_anim_set_var(&a, loadingBar);
//   lv_anim_set_values(&a, 0, 100);
//   lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
//   lv_anim_start(&a);

//   // Refresh Button
//   lv_obj_t * refreshBtn = lv_btn_create(obj);
//   lv_obj_set_size(refreshBtn, 120, 40);
//   lv_obj_align(refreshBtn, LV_ALIGN_BOTTOM_MID, 0, -10);
//   lv_obj_add_event_cb(refreshBtn, refresh_btn_event_cb, LV_EVENT_CLICKED, NULL);
//   lv_obj_t * refreshLabel = lv_label_create(refreshBtn);
//   lv_label_set_text(refreshLabel, "Refresh CSV");
//   lv_obj_center(refreshLabel);
// }

// String readConcentrationFromCSV() {
//   // 🔁 Simulate reading from CSV
//   Serial.println("Reading simulated CSV value...");
//   return simulatedCSV;
// }

// void refreshConcentration() {
//   String val = readConcentrationFromCSV();
//   lv_label_set_text_fmt(concentrationLabel, "Concentration: %s", val.c_str());
// }



///////////////////
// /*
//   LVGLDemo - Double Tap to Enter UI, Reset with Slider

//   Created 17 Apr 2023
//   Modified 16 Apr 2025 by Pau & ChatGPT
// */

// #include "Arduino_H7_Video.h"
// #include "Arduino_GigaDisplayTouch.h"
// #include "lvgl.h"

// Arduino_H7_Video          Display(800, 480, GigaDisplayShield);
// Arduino_GigaDisplayTouch  TouchDetector;

// bool introScreenActive = true;
// uint32_t lastTapTime = 0;
// int tapCount = 0;

// // Optional global slider pointer (only needed if resetting its value manually)
// // lv_obj_t * globalSlider; // Uncomment if needed for further control

// /* Forward declaration */
// void loadMainUIScreen();
// void loadIntroScreen();

// /* Button click event callback */
// static void btn_event_cb(lv_event_t * e) {
//   static uint32_t cnt = 1;
//   lv_obj_t * btn = (lv_obj_t *)lv_event_get_target(e);
//   lv_obj_t * label = lv_obj_get_child(btn, 0);
//   lv_label_set_text_fmt(label, "%"LV_PRIu32, cnt);
//   cnt++;
// }

// /* Slider update value handler */
// static void set_slider_val(void * bar, int32_t val) {
//   lv_bar_set_value((lv_obj_t *)bar, val, LV_ANIM_ON);

//   if (val >= 100 && !introScreenActive) {
//     introScreenActive = true;
//     tapCount = 0;
//     lv_obj_clean(lv_scr_act()); // Clear screen
//     loadIntroScreen();          // Reload intro screen
//   }
// }

// /* Intro screen logic */
// void loadIntroScreen() {
//   lv_obj_set_style_bg_color(lv_scr_act(), lv_color_white(), LV_PART_MAIN);
//   lv_obj_t * introLabel = lv_label_create(lv_scr_act());
//   lv_label_set_text(introLabel, "Tap twice to start");
//   lv_obj_center(introLabel);
// }

// void setup() {
//   Serial.begin(115200);
//   Display.begin();
//   TouchDetector.begin();

//   loadIntroScreen(); // Start on intro screen
// }

// void loop() {
//   lv_timer_handler();

//   if (introScreenActive) {
//     GDTpoint_t touchPoints[5];
//     uint8_t numPoints = TouchDetector.getTouchPoints(touchPoints);

//     if (numPoints > 0) {
//       uint32_t now = millis();
//       if (now - lastTapTime < 1500) {
//         tapCount++;
//       } else {
//         tapCount = 1;
//       }

//       lastTapTime = now;

//       if (tapCount >= 2) {
//         introScreenActive = false;
//         lv_obj_clean(lv_scr_act()); // Clear intro screen
//         loadMainUIScreen();
//       }

//       delay(200); // debounce
//     }
//   }
// }

// /* Load full UI after double tap */
// void loadMainUIScreen() {
//   static lv_coord_t col_dsc[] = {370, 370, LV_GRID_TEMPLATE_LAST};
//   static lv_coord_t row_dsc[] = {215, 215, LV_GRID_TEMPLATE_LAST};
//   lv_obj_t * cont = lv_obj_create(lv_scr_act());
//   lv_obj_set_grid_dsc_array(cont, col_dsc, row_dsc);
//   lv_obj_set_size(cont, Display.width(), Display.height());
//   lv_obj_set_style_bg_color(cont, lv_color_hex(0x03989e), LV_PART_MAIN);
//   lv_obj_center(cont);

//   lv_obj_t * label;
//   lv_obj_t * obj;

//   // [0;0] - Concentration box
//   obj = lv_obj_create(cont);
//   lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 0, 1,
//                         LV_GRID_ALIGN_STRETCH, 0, 1);
//   label = lv_label_create(obj);
//   lv_label_set_text(label, "Concentration:");
//   lv_obj_center(label);

//   // [1;0] - Checkboxes and button
//   obj = lv_obj_create(cont);
//   lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 1, 1,
//                         LV_GRID_ALIGN_STRETCH, 0, 1);
//   lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_COLUMN);

//   const char *names[] = {"Kayla", "Pau", "Theertha", "Osuji", "Joe", "Extra usher"};
//   for (int i = 0; i < 6; i++) {
//     lv_obj_t * cb = lv_checkbox_create(obj);
//     lv_checkbox_set_text(cb, names[i]);
//     if (i == 1 || i == 3) lv_obj_add_state(cb, LV_STATE_CHECKED);
//   }

//   lv_obj_t * btn = lv_btn_create(obj);
//   lv_obj_set_size(btn, 100, 40);
//   lv_obj_center(btn);
//   lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);
//   label = lv_label_create(btn);
//   lv_label_set_text(label, "Click me!");
//   lv_obj_center(label);

//   // [0;1] - Slider
//   obj = lv_obj_create(cont);
//   lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 0, 1,
//                         LV_GRID_ALIGN_STRETCH, 1, 1);

//   lv_obj_t * slider = lv_slider_create(obj);
//   lv_slider_set_value(slider, 75, LV_ANIM_OFF);
//   lv_obj_center(slider);
//   label = lv_label_create(obj);
//   lv_label_set_text(label, "Drag me!");
//   lv_obj_align_to(label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

//   // Optionally store slider reference globally
//   // globalSlider = slider;

//   // [1;1] - Bar and label
//   obj = lv_obj_create(cont);
//   lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_STRETCH, 1, 1,
//                         LV_GRID_ALIGN_STRETCH, 1, 1);

//   lv_obj_t * bar_label = lv_label_create(obj);
//   lv_label_set_text(bar_label, "Running Sample");
//   lv_obj_align(bar_label, LV_ALIGN_TOP_MID, 0, 10);

//   lv_obj_t * bar = lv_bar_create(obj);
//   lv_obj_set_size(bar, 200, 20);
//   lv_obj_align(bar, LV_ALIGN_CENTER, 0, 20);
//   lv_bar_set_value(bar, 70, LV_ANIM_OFF);

//   lv_anim_t a;
//   lv_anim_init(&a);
//   lv_anim_set_exec_cb(&a, set_slider_val);
//   lv_anim_set_time(&a, 3000);
//   lv_anim_set_playback_time(&a, 3000);
//   lv_anim_set_var(&a, bar);
//   lv_anim_set_values(&a, 0, 100);
//   lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
//   lv_anim_start(&a);
// }

