
#include "Arduino_H7_Video.h"
#include "Arduino_GigaDisplayTouch.h"
#include "ArduinoGraphics.h"
#include "lvgl.h"
#include "incbin.h"


INCBIN(logo, "/Users/paulinabargallo/Documents/Arduino/Display Glucometer/LCD test/may5/comb/sketch_may5b/BinarySweeTech.bin");

Arduino_H7_Video Display(800, 480, GigaDisplayShield);
Arduino_GigaDisplayTouch TouchDetector;

Image img_logo(ENCODING_RGB16, (uint8_t *) logoData, 800, 480);

bool introScreenActive = true;
uint32_t lastTapTime = 0;
int tapCount = 0;

lv_obj_t *concentrationLabel;
lv_obj_t *loadingBar;
lv_obj_t *infoLabel;
lv_obj_t *introLabel;

void loadMainUIScreen();
void loadIntroScreen();
void resetToIntro();
void refreshConcentration();

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
  Display.beginDraw();
  Display.image(img_logo, 0, 0);
  Display.endDraw();

  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_white(), LV_PART_MAIN);
  introLabel = lv_label_create(lv_scr_act());
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
  lv_init();
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
