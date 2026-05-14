#include <Waveshare_LCD.h>
#include <WiFi.h>
#include <time.h>

const char* ssid = "ECUSHOP_OFFICE";
const char* password = "0835712428";

// --- ตั้งค่า NTP Server ---
const char* ntpServer = "time.navy.mi.th";  
const long gmtOffset_sec = 7 * 3600;  // GMT+7 สำหรับประเทศไทย
const int daylightOffset_sec = 0;     // ไม่มี daylight saving

// Global Variable สำหรับอัปเดตค่าตัวเลขบนหน้าจอ
lv_obj_t * label_kwh_value;
lv_obj_t * label_water_value;
lv_obj_t * label_status;
lv_obj_t * label_clock;
lv_obj_t * label_date;

bool time_synced = false;
// === ฟังก์ชันสำหรับสร้าง UI หน้าจอหลัก ===
void build_meter_ui() {
  lv_obj_t * scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x0A0E27), 0);

  // === Header Card ===
  lv_obj_t * header_card = lv_obj_create(scr);
  lv_obj_set_size(header_card, 250, 100); 
  lv_obj_set_style_bg_color(header_card, lv_color_hex(0x1A1F3A), 0);
  lv_obj_set_style_border_width(header_card, 0, 0);
  lv_obj_set_style_radius(header_card, 20, 0);
  lv_obj_set_style_shadow_width(header_card, 15, 0);
  lv_obj_set_style_shadow_color(header_card, lv_color_hex(0x000000), 0);
  lv_obj_set_style_shadow_opa(header_card, LV_OPA_40, 0);
  lv_obj_align(header_card, LV_ALIGN_TOP_MID, 0, 30); 

  // Title
  lv_obj_t * label_title = lv_label_create(header_card);
  lv_label_set_text(label_title, "ROOM NO.0001");
  lv_obj_set_style_text_color(label_title, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(label_title, &lv_font_montserrat_24, 0); 
  lv_obj_align(label_title, LV_ALIGN_TOP_MID, 0, 5);

  // วันที่
  label_date = lv_label_create(header_card);
  lv_label_set_text(label_date, "01/01/2024");
  lv_obj_set_style_text_color(label_date, lv_color_hex(0x8B92B0), 0);
  lv_obj_set_style_text_font(label_date, &lv_font_montserrat_16, 0); 
  lv_obj_align(label_date, LV_ALIGN_BOTTOM_LEFT, 15, -10);

  // นาฬิกา
  label_clock = lv_label_create(header_card);
  lv_label_set_text(label_clock, "00:00:00");
  lv_obj_set_style_text_color(label_clock, lv_color_hex(0xFFFFFF), 0);
  lv_obj_set_style_text_font(label_clock, &lv_font_montserrat_16, 0); 
  lv_obj_align(label_clock, LV_ALIGN_BOTTOM_RIGHT, -15, -10);

  // === การ์ดไฟฟ้า (ซ้าย) ===
  lv_obj_t * card_kwh = lv_obj_create(scr);
  lv_obj_set_size(card_kwh, 190, 200);
  lv_obj_set_style_bg_color(card_kwh, lv_color_hex(0x1A1F3A), 0);
  lv_obj_set_style_border_width(card_kwh, 2, 0);
  lv_obj_set_style_border_color(card_kwh, lv_color_hex(0x00FF00), 0);
  lv_obj_set_style_border_opa(card_kwh, LV_OPA_60, 0);
  lv_obj_set_style_radius(card_kwh, 20, 0);
  lv_obj_set_style_shadow_width(card_kwh, 10, 0);
  lv_obj_set_style_shadow_color(card_kwh, lv_color_hex(0x00FF00), 0);
  lv_obj_set_style_shadow_opa(card_kwh, LV_OPA_30, 0);
  lv_obj_align(card_kwh, LV_ALIGN_CENTER, -105, 20); 

  // ไอคอนไฟฟ้า
  lv_obj_t * icon_kwh = lv_label_create(card_kwh);
  lv_label_set_text(icon_kwh, LV_SYMBOL_CHARGE);
  lv_obj_set_style_text_color(icon_kwh, lv_color_hex(0x00FF00), 0);
  lv_obj_set_style_text_font(icon_kwh, &lv_font_montserrat_30, 0); 
  lv_obj_align(icon_kwh, LV_ALIGN_TOP_MID, 0, 15);

  // ค่าไฟฟ้า
  label_kwh_value = lv_label_create(card_kwh);
  lv_label_set_text(label_kwh_value, "000000.00");
  lv_obj_set_style_text_color(label_kwh_value, lv_color_hex(0x00FF00), 0);
  lv_obj_set_style_text_font(label_kwh_value, &lv_font_montserrat_28, 0); 
  lv_obj_set_style_text_letter_space(label_kwh_value, 2, 0);
  lv_obj_align(label_kwh_value, LV_ALIGN_CENTER, 0, 0);

  // หน่วย kWh
  lv_obj_t * label_kwh_unit = lv_label_create(card_kwh);
  lv_label_set_text(label_kwh_unit, "kWh");
  lv_obj_set_style_text_color(label_kwh_unit, lv_color_hex(0x8B92B0), 0);
  lv_obj_set_style_text_font(label_kwh_unit, &lv_font_montserrat_16, 0); 
  lv_obj_align(label_kwh_unit, LV_ALIGN_BOTTOM_MID, 0, -15);

  // === การ์ดน้ำ (ขวา) ===
  lv_obj_t * card_water = lv_obj_create(scr);
  lv_obj_set_size(card_water, 190, 200);
  lv_obj_set_style_bg_color(card_water, lv_color_hex(0x1A1F3A), 0);
  lv_obj_set_style_border_width(card_water, 2, 0);
  lv_obj_set_style_border_color(card_water, lv_color_hex(0x00BFFF), 0);
  lv_obj_set_style_border_opa(card_water, LV_OPA_60, 0);
  lv_obj_set_style_radius(card_water, 20, 0);
  lv_obj_set_style_shadow_width(card_water, 10, 0);
  lv_obj_set_style_shadow_color(card_water, lv_color_hex(0x00BFFF), 0);
  lv_obj_set_style_shadow_opa(card_water, LV_OPA_30, 0);
  lv_obj_align(card_water, LV_ALIGN_CENTER, 105, 20);

  // ไอคอนน้ำ
  lv_obj_t * icon_water = lv_label_create(card_water);
  lv_label_set_text(icon_water, LV_SYMBOL_TINT);
  lv_obj_set_style_text_color(icon_water, lv_color_hex(0x00BFFF), 0);
  lv_obj_set_style_text_font(icon_water, &lv_font_montserrat_30, 0); 
  lv_obj_align(icon_water, LV_ALIGN_TOP_MID, 0, 15);

  // ค่าน้ำ
  label_water_value = lv_label_create(card_water);
  lv_label_set_text(label_water_value, "0000.00");
  lv_obj_set_style_text_color(label_water_value, lv_color_hex(0x00BFFF), 0);
  lv_obj_set_style_text_font(label_water_value, &lv_font_montserrat_30, 0); 
  lv_obj_set_style_text_letter_space(label_water_value, 2, 0);
  lv_obj_align(label_water_value, LV_ALIGN_CENTER, 0, 0);

  // หน่วย m3
  lv_obj_t * label_water_unit = lv_label_create(card_water);
  lv_label_set_text(label_water_unit, "m3"); 
  lv_obj_set_style_text_color(label_water_unit, lv_color_hex(0x8B92B0), 0);
  lv_obj_set_style_text_font(label_water_unit, &lv_font_montserrat_16, 0); 
  lv_obj_align(label_water_unit, LV_ALIGN_BOTTOM_MID, 0, -15);

  // === Status Bar ===
  label_status = lv_label_create(scr);
  lv_label_set_text(label_status, LV_SYMBOL_WIFI " Preparing...");
  lv_obj_set_style_text_color(label_status, lv_color_hex(0xFFA500), 0);
  lv_obj_set_style_text_font(label_status, &lv_font_montserrat_20, 0); 
  lv_obj_align(label_status, LV_ALIGN_BOTTOM_MID, 0, -35); 
}

// === ฟังก์ชันดึงเวลาจาก NTP และเซ็ตให้ RTC ===
void sync_time_from_ntp() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  
  // ตั้งค่า NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  // รอให้ได้เวลาจาก NTP 
  struct tm timeinfo;
  int attempts = 0;
  while (!getLocalTime(&timeinfo) && attempts < 20) {
    delay(500);
    lv_timer_handler();
    attempts++;
  }
  
  if (attempts >= 20) {
    return;
  }
  // เซ็ตเวลาให้ RTC
  datetime_t rtc_time;
  rtc_time.year = timeinfo.tm_year + 1900;
  rtc_time.month = timeinfo.tm_mon + 1;
  rtc_time.day = timeinfo.tm_mday;
  rtc_time.hour = timeinfo.tm_hour;
  rtc_time.minute = timeinfo.tm_min;
  rtc_time.second = timeinfo.tm_sec;
  rtc_time.dotw = timeinfo.tm_wday; // 0=Sunday, 1=Monday, ...
  
  PCF85063_Set_All(rtc_time);
  time_synced = true;
  
  delay(100);
}

void load_last_meter_values(float room_kwh[], float room_water[]) {
  if (!SD_MMC.begin()) {
    return;
  }
  
  datetime_t current_time;
  PCF85063_Read_Time(&current_time);
  
  // อ่านค่าล่าสุดจากแต่ละห้อง (100 ห้อง)
  for (int room = 1; room <= 100; room++) {
    char filename[50];
    sprintf(filename, "/%04d_%04d%02d%02d.csv", 
            room, current_time.year, current_time.month, current_time.day);
    
    if (!SD_MMC.exists(filename)) {
      continue;
    }
    
    File file = SD_MMC.open(filename, FILE_READ);
    if (!file) {
      continue;
    }
    
    // อ่านบรรทัดสุดท้าย
    String lastLine = "";
    while (file.available()) {
      String line = file.readStringUntil('\n');
      if (line.length() > 10 && line.indexOf(',') > 0) {
        lastLine = line;
      }
    }
    file.close();
    
    // แยกค่า kWh และ m3 จากบรรทัดสุดท้าย
    if (lastLine.length() > 0) {
      int comma1 = lastLine.indexOf(',');
      int comma2 = lastLine.indexOf(',', comma1 + 1);
      int comma3 = lastLine.indexOf(',', comma2 + 1);
      
      if (comma3 > 0) {
        String kwhStr = lastLine.substring(comma2 + 1, comma3);
        String waterStr = lastLine.substring(comma3 + 1);
        
        room_kwh[room - 1] = kwhStr.toFloat();
        room_water[room - 1] = waterStr.toFloat();
      }
    }
  }
}

void save_meter_data_for_room(int room_number, float kwh, float water) {
  if (!time_synced) {
    return;
  }

  datetime_t current_time;
  PCF85063_Read_Time(&current_time);
  
  char filename[50];
  sprintf(filename, "/%04d_%04d%02d%02d.csv", 
          room_number, current_time.year, current_time.month, current_time.day);
  
  // ตรวจสอบว่าไฟล์มีอยู่แล้วหรือไม่
  bool file_exists = SD_MMC.exists(filename);
  
  File file = SD_MMC.open(filename, FILE_APPEND);
  if (!file) {
    return;
  }
  
  // ถ้าไฟล์ใหม่ ให้เพิ่ม header
  if (!file_exists) {
    file.println("Date,Time,kWh,m3");
  }
  
  char buffer[200];
  sprintf(buffer, "%04d-%02d-%02d,%02d:%02d:%02d,%.2f,%.2f",
          current_time.year, current_time.month, current_time.day,
          current_time.hour, current_time.minute, current_time.second,
          kwh, water);
  
  file.println(buffer);
  file.flush();
  file.close();
}

void save_meter_data(float kwh, float water) {
  if (!time_synced) {
    return;
  }

  datetime_t current_time;
  PCF85063_Read_Time(&current_time);
  
  char filename[50];
  sprintf(filename, "/0001_%04d%02d%02d.csv", 
          current_time.year, current_time.month, current_time.day);
  
  // ตรวจสอบว่า SD card พร้อมใช้งานหรือไม่
  if (!SD_MMC.begin()) {
    SD_Init();  // ลอง init ใหม่
    delay(500);
    
    if (!SD_MMC.begin()) {
      return;
    }
  }
  
  // ตรวจสอบว่าไฟล์มีอยู่แล้วหรือไม่ เพื่อเพิ่ม header
  bool file_exists = SD_MMC.exists(filename);
  
  File file = SD_MMC.open(filename, FILE_APPEND);
  if (!file) {
    return;
  }
  
  // ถ้าไฟล์ใหม่ ให้เพิ่ม header
  if (!file_exists) {
    file.println("Date,Time,kWh,m3");
  }
  
  char buffer[200];
  sprintf(buffer, "%04d-%02d-%02d,%02d:%02d:%02d,%.2f,%.2f",
          current_time.year, current_time.month, current_time.day,
          current_time.hour, current_time.minute, current_time.second,
          kwh, water);
  
  file.print(buffer);
  file.flush();  // บังคับให้เขียนลง SD card ทันที
  file.close();

}

// === ฟังก์ชันจัดการอัปเดตข้อมูลตัวเลข ===
void connect_wifi_to_ui() {
  if (label_status != NULL) {
    lv_label_set_text(label_status, LV_SYMBOL_WIFI " Connecting to WiFi...");
    lv_obj_set_style_text_color(label_status, lv_color_hex(0xFFA500), 0);
    lv_timer_handler(); 
  }

  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    lv_timer_handler();
    attempts++;
  }

  // อัปเดตหน้าจอเมื่อรู้ผลการเชื่อมต่อ
  if (WiFi.status() == WL_CONNECTED) {
    if (label_status != NULL) {
      lv_label_set_text(label_status, LV_SYMBOL_WIFI " Syncing Time...");
      lv_obj_set_style_text_color(label_status, lv_color_hex(0xFFA500), 0);
      lv_timer_handler();
    }
    
    // ดึงเวลาจาก NTP และเซ็ตให้ RTC
    sync_time_from_ntp();
    
    if (label_status != NULL) {
      lv_label_set_text(label_status, LV_SYMBOL_WIFI " Connected");
      lv_obj_set_style_text_color(label_status, lv_color_hex(0x00FF00), 0); // เปลี่ยนเป็นสีเขียว
    }
  } else {
    if (label_status != NULL) {
      lv_label_set_text(label_status, LV_SYMBOL_WARNING " WiFi Disconnected");
      lv_obj_set_style_text_color(label_status, lv_color_hex(0xFF0000), 0); // เปลี่ยนเป็นสีแดง
    }
  }
}

// === ฟังก์ชันจัดการอัปเดตข้อมูลตัวเลข ===
void update_kwh_display(double value) {
  if (label_kwh_value != NULL) {
    char buf[32];
    // แยกส่วนจำนวนเต็มและทศนิยม
    int integer_part = (int)value;
    int decimal_part = (int)((value - integer_part) * 100);
    snprintf(buf, sizeof(buf), "%06d.%02d", integer_part, decimal_part);  // 6 หลัก + . + 2 หลัก
    lv_label_set_text(label_kwh_value, buf);
  }
}

void update_water_display(double value) {
  if (label_water_value != NULL) {
    char buf[32];
    int integer_part = (int)value;
    int decimal_part = (int)((value - integer_part) * 100);
    snprintf(buf, sizeof(buf), "%04d.%02d", integer_part, decimal_part);  // 4 หลัก + . + 2 หลัก
    lv_label_set_text(label_water_value, buf);
  }
}

void update_clock_display(int hours, int minutes, int seconds) {
  if (label_clock != NULL) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", hours, minutes, seconds);
    lv_label_set_text(label_clock, buf);
  }
}

void update_date_display(int day, int month, int year) {
  if (label_date != NULL) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d", day, month, year);
    lv_label_set_text(label_date, buf);
  }
}

// === ฟังก์ชัน setup() เริ่มต้นระบบ ===
void setup() {
  Serial.begin(115200);
  delay(2000);
  Waveshare_Init_All();
  PCF85063_Init();
  SD_Init();
  build_meter_ui();
  connect_wifi_to_ui();

}

void loop() {
  lv_timer_handler();
  
  // ค่าเริ่มต้นของแต่ละห้อง (100 ห้อง)
  static float room_kwh[100];
  static float room_water[100];
  
  static bool first_run = true;
  if (first_run) {
    // ตั้งค่าเริ่มต้นให้ทุกห้อง
    for (int i = 0; i < 100; i++) {
      room_kwh[i] = 100000.0 + (i * 1000);  // ห้อง 1=100000, ห้อง 2=101000, ...
      room_water[i] = 1000.0 + (i * 10);     // ห้อง 1=1000, ห้อง 2=1010, ...
    }
    
    // โหลดค่าจาก SD card
    load_last_meter_values(room_kwh, room_water);
    first_run = false;
  }
  
  static unsigned long last_update = 0;
  static unsigned long last_wifi_check = 0;
  static unsigned long last_save = 0;

  unsigned long current_time = millis();
  
  // อัปเดตการแสดงผลบนหน้าจอทุกวินาที (แบบ real-time)
  if (current_time - last_update >= 1000) {
    last_update = current_time;
    
    datetime_t current_datetime;
    PCF85063_Read_Time(&current_datetime);
    
    update_clock_display(current_datetime.hour, current_datetime.minute, current_datetime.second);
    update_date_display(current_datetime.day, current_datetime.month, current_datetime.year);
    
    // เพิ่มค่าทุกห้อง ทุกวินาที (100 ห้อง)
    for (int i = 0; i < 100; i++) {
      room_kwh[i] += 0.01667;  // 1 หน่วย/นาที = 0.01667 หน่วย/วินาที
      room_water[i] += 0.01667;
    }
    
    // แสดงค่าห้อง 1 บนหน้าจอ
    update_kwh_display(room_kwh[0]);
    update_water_display(room_water[0]);
  }

  // ตรวจสอบสถานะ WiFi ทุก 5 วินาที
  if (current_time - last_wifi_check >= 5000) {
    last_wifi_check = current_time;
    
    if (label_status != NULL) {
      if (WiFi.status() == WL_CONNECTED) {
        lv_label_set_text(label_status, LV_SYMBOL_WIFI " Connected");
        lv_obj_set_style_text_color(label_status, lv_color_hex(0x00FF00), 0);
      } else {
        lv_label_set_text(label_status, LV_SYMBOL_WARNING " WiFi Disconnected");
        lv_obj_set_style_text_color(label_status, lv_color_hex(0xFF0000), 0);
        WiFi.reconnect();
      }
    }
  }

  // บันทึกข้อมูลทุก 10 นาที (600,000 มิลลิวินาที)
  if (current_time - last_save >= 600000) {  
    
    // Re-initialize SD card ก่อนบันทึก (กรณีถอดแล้วเสียบใหม่)
    SD_MMC.end();
    delay(100);
    if (!SD_MMC.begin()) {
      SD_Init();
      delay(200);
    }
    
    // ตรวจสอบว่า SD card พร้อมใช้งานหรือไม่
    if (SD_MMC.begin()) {
      // บันทึกข้อมูล 100 ห้อง
      for (int room = 1; room <= 100; room++) {
        save_meter_data_for_room(room, room_kwh[room-1], room_water[room-1]);
        lv_timer_handler();  // ป้องกัน watchdog reset
        delay(50);
      }
      last_save = current_time;  // อัปเดตเวลาเฉพาะเมื่อบันทึกสำเร็จ
    }
    // ถ้า SD card ไม่พร้อม จะไม่ update last_save และจะลองใหม่ในรอบถัดไป
  }

  delay(10); 
}