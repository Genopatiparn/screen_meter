#include <Waveshare_LCD.h>

// สร้าง Global Variable ไว้สำหรับอัปเดตค่าตัวเลขบนหน้าจอภายหลัง
lv_obj_t * label_kwh_value;
lv_obj_t * label_water_value;
lv_obj_t * label_status;

// ฟังก์ชันสำหรับสร้าง UI หน้าจอหลัก
void build_meter_ui() {

  lv_obj_t * scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x1E1E1E), 0);

  lv_obj_t * label_title = lv_label_create(scr);
  lv_label_set_text(label_title, "CHNT DDS666 Monitor");
  lv_obj_set_style_text_color(label_title, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 30);

  lv_obj_t * card = lv_obj_create(scr);
  lv_obj_set_size(card, 240, 120);
  lv_obj_set_style_bg_color(card, lv_color_hex(0x2D2D2D), 0);
  lv_obj_set_style_border_width(card, 0, 0);
  lv_obj_set_style_radius(card, 15, 0);
  lv_obj_align(card, LV_ALIGN_CENTER, 0, -60);

  label_kwh_value = lv_label_create(card);
  lv_label_set_text(label_kwh_value, "000000.00");

  lv_obj_set_style_text_color(label_kwh_value, lv_color_hex(0x00FF00), 0);
  lv_obj_align(label_kwh_value, LV_ALIGN_CENTER, 0, -10);

  lv_obj_t * label_unit = lv_label_create(card);
  lv_label_set_text(label_unit, "kWh");
  lv_obj_set_style_text_color(label_unit, lv_color_hex(0xAAAAAA), 0);
  lv_obj_align_to(label_unit, label_kwh_value, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  // กล่องวัดค่าน้ำ
  lv_obj_t * card_water = lv_obj_create(scr);
  lv_obj_set_size(card_water, 240, 120);
  lv_obj_set_style_bg_color(card_water, lv_color_hex(0x2D2D2D), 0);
  lv_obj_set_style_border_width(card_water, 0, 0);
  lv_obj_set_style_radius(card_water, 15, 0);
  lv_obj_align(card_water, LV_ALIGN_CENTER, 0, 70);

  label_water_value = lv_label_create(card_water);
  lv_label_set_text(label_water_value, "0000.00");
  lv_obj_set_style_text_color(label_water_value, lv_color_hex(0x00BFFF), 0);
  lv_obj_align(label_water_value, LV_ALIGN_CENTER, 0, -10);

  lv_obj_t * label_water_unit = lv_label_create(card_water);
  lv_label_set_text(label_water_unit, "m³");
  lv_obj_set_style_text_color(label_water_unit, lv_color_hex(0xAAAAAA), 0);
  lv_obj_align_to(label_water_unit, label_water_value, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

  label_status = lv_label_create(scr);
  lv_label_set_text(label_status, "Status: Waiting for Data...");
  lv_obj_set_style_text_color(label_status, lv_color_hex(0xFFA500), 0);
  lv_obj_align(label_status, LV_ALIGN_BOTTOM_MID, 0, -30);
}

void update_kwh_display(float value) {
  if (label_kwh_value != NULL) {
    char buf[32];
    sprintf(buf, "%.2f", value);

    lv_label_set_text(label_kwh_value, buf);

    lv_label_set_text(label_status, "Status: Connected");
    lv_obj_set_style_text_color(label_status, lv_color_hex(0x00FF00), 0);
  }
}

void update_water_display(float value) {
  if (label_water_value != NULL) {
    char buf[32];
    sprintf(buf, "%.2f", value);
    lv_label_set_text(label_water_value, buf);
  }
}

void setup() {
  Waveshare_Init_All();
  
  // ต้องเรียกฟังก์ชันนี้ด้วยครับ UI ถึงจะถูกวาดขึ้นมา
  build_meter_ui(); 
}

void loop() {

  lv_timer_handler();

  static float mock_value = 12345.67;
  static float mock_water_value = 123.45;

  update_kwh_display(mock_value);
  update_water_display(mock_water_value);

  mock_value += 0.01;
  mock_water_value += 0.05;

  delay(1000);
}