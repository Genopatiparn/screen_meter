#include <Waveshare_LCD.h>

// สร้าง Global Variable ไว้สำหรับอัปเดตค่าตัวเลขบนหน้าจอภายหลัง
lv_obj_t * label_kwh_value;
lv_obj_t * label_water_value;
lv_obj_t * label_status;
lv_obj_t * label_clock;
lv_obj_t * label_date;

// ฟังก์ชันสำหรับสร้าง UI หน้าจอหลัก
void build_meter_ui() {

  lv_obj_t * scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x1E1E1E), 0);

  lv_obj_t * label_title = lv_label_create(scr);
  lv_label_set_text(label_title, "CHNT DDS666 Monitor");
  lv_obj_set_style_text_color(label_title, lv_color_hex(0xFFFFFF), 0);
  lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 15);

  // วันที่
  label_date = lv_label_create(scr);
  lv_label_set_text(label_date, "01/01/2024");
  lv_obj_set_style_text_color(label_date, lv_color_hex(0xAAAAAA), 0);
  lv_obj_set_style_text_font(label_date, &lv_font_montserrat_14, 0);
  lv_obj_align(label_date, LV_ALIGN_TOP_MID, 0, 38);

  // นาฬิกา
  label_clock = lv_label_create(scr);
  lv_label_set_text(label_clock, "00:00:00");
  lv_obj_set_style_text_color(label_clock, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(label_clock, &lv_font_montserrat_20, 0);
  lv_obj_align(label_clock, LV_ALIGN_TOP_MID, 0, 58);

  lv_obj_t * card = lv_obj_create(scr);
  lv_obj_set_size(card, 240, 120);
  lv_obj_set_style_bg_color(card, lv_color_hex(0x2D2D2D), 0);
  lv_obj_set_style_border_width(card, 0, 0);
  lv_obj_set_style_radius(card, 15, 0);
  lv_obj_align(card, LV_ALIGN_CENTER, 0, -50);

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
  lv_obj_align(card_water, LV_ALIGN_CENTER, 0, 80);

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

void update_clock_display(int hours, int minutes, int seconds) {
  if (label_clock != NULL) {
    char buf[16];
    sprintf(buf, "%02d:%02d:%02d", hours, minutes, seconds);
    lv_label_set_text(label_clock, buf);
  }
}

void update_date_display(int day, int month, int year) {
  if (label_date != NULL) {
    char buf[16];
    sprintf(buf, "%02d/%02d/%04d", day, month, year);
    lv_label_set_text(label_date, buf);
  }
}

void setup() {
  Waveshare_Init_All();
  
  // ต้องเรียกฟังก์ชันนี้ด้วยครับ UI ถึงจะถูกวาดขึ้นมา
  build_meter_ui();
  
  // ตั้งค่าวันที่เริ่มต้น
  update_date_display(8, 5, 2026);
}

void loop() {

  lv_timer_handler();

  static float mock_value = 12345.67;
  static float mock_water_value = 123.45;
  static unsigned long last_update = 0;
  static int hours = 14;
  static int minutes = 30;
  static int seconds = 0;

  // อัปเดตนาฬิกาทุกวินาที
  unsigned long current_time = millis();
  if (current_time - last_update >= 1000) {
    last_update = current_time;
    
    seconds++;
    if (seconds >= 60) {
      seconds = 0;
      minutes++;
      if (minutes >= 60) {
        minutes = 0;
        hours++;
        if (hours >= 24) {
          hours = 0;
        }
      }
    }
    
    update_clock_display(hours, minutes, seconds);
  }

  update_kwh_display(mock_value);
  update_water_display(mock_water_value);

  mock_value += 0.01;
  mock_water_value += 0.05;

  delay(100);
}