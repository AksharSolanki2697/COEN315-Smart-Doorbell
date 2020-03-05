#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"

#include "fb_gfx.h"
#include "fd_forward.h"
#include "fr_forward.h"
#include <FS.h>
#include <SPIFFS.h>

// Photo File Name to save in SPIFFS
#define FILE_PHOTO "/photo.jpg"

#define ENROLL_CONFIRM_TIMES 5
#define FACE_ID_SAVE_NUMBER 7

#define FACE_COLOR_WHITE  0x00FFFFFF
#define FACE_COLOR_BLACK  0x00000000
#define FACE_COLOR_RED    0x000000FF
#define FACE_COLOR_GREEN  0x0000FF00
#define FACE_COLOR_BLUE   0x00FF0000
#define FACE_COLOR_YELLOW (FACE_COLOR_RED | FACE_COLOR_GREEN)
#define FACE_COLOR_CYAN   (FACE_COLOR_BLUE | FACE_COLOR_GREEN)
#define FACE_COLOR_PURPLE (FACE_COLOR_BLUE | FACE_COLOR_RED)

static mtmn_config_t mtmn_config = {0};
static int8_t detection_enabled = 0;
static int8_t recognition_enabled = 0;
static int8_t is_enrolling = 0;
static face_id_list id_list = {0};
static char face_name[FACE_ID_SAVE_NUMBER][4];

static void rgb_print(dl_matrix3du_t *image_matrix, uint32_t color, const char * str){
    fb_data_t fb;
    fb.width = image_matrix->w;
    fb.height = image_matrix->h;
    fb.data = image_matrix->item;
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR888;
    fb_gfx_print(&fb, (fb.width - (strlen(str) * 14)) / 2, 10, color, str);
}

static int rgb_printf(dl_matrix3du_t *image_matrix, uint32_t color, const char *format, ...){
    char loc_buf[64];
    char * temp = loc_buf;
    int len;
    va_list arg;
    va_list copy;
    va_start(arg, format);
    va_copy(copy, arg);
    len = vsnprintf(loc_buf, sizeof(loc_buf), format, arg);
    va_end(copy);
    if(len >= sizeof(loc_buf)){
        temp = (char*)malloc(len+1);
        if(temp == NULL) {
            return 0;
        }
    }
    vsnprintf(temp, len+1, format, arg);
    va_end(arg);
    rgb_print(image_matrix, color, temp);
    if(len > 64){
        free(temp);
    }
    return len;
}

static int run_face_recognition(dl_matrix3du_t *image_matrix, box_array_t *net_boxes, char *names){
    dl_matrix3du_t *aligned_face = NULL;
    int matched_id = 0;

    aligned_face = dl_matrix3du_alloc(1, FACE_WIDTH, FACE_HEIGHT, 3);
    if(!aligned_face){
        Serial.println("Could not allocate face recognition buffer");
        return matched_id;
    }
    if (align_face(net_boxes, image_matrix, aligned_face) == ESP_OK){
        if (is_enrolling == 1){
            int8_t left_sample_face = enroll_face(&id_list, aligned_face);

            if(left_sample_face == (ENROLL_CONFIRM_TIMES - 1)){
                Serial.printf("Enrolling Face ID: %d\n", id_list.tail);
            }
            Serial.printf("Enrolling Face ID: %d sample %d\n", id_list.tail, ENROLL_CONFIRM_TIMES - left_sample_face);
            rgb_printf(image_matrix, FACE_COLOR_CYAN, "ID[%u] Sample[%u]", id_list.tail, ENROLL_CONFIRM_TIMES - left_sample_face);
            if (left_sample_face == 0){
                is_enrolling = 0;
                Serial.printf("Enrolled Face ID: %d\n", id_list.tail);
            }
        } else {
            matched_id = recognize_face(&id_list, aligned_face);
            if (matched_id >= 0) {
                Serial.printf("Match Face ID: %u\n", matched_id);
                rgb_printf(image_matrix, FACE_COLOR_GREEN, "Hello Subject %u", matched_id);
            } else {
                Serial.println("No Match Found");
                rgb_print(image_matrix, FACE_COLOR_RED, "Intruder Alert!");
                matched_id = -1;
            }
        }
    } else {
        Serial.println("Face Not Aligned");
        //rgb_print(image_matrix, FACE_COLOR_YELLOW, "Human Detected");
    }

    dl_matrix3du_free(aligned_face);
    return matched_id;
}

static void draw_face_boxes(dl_matrix3du_t *image_matrix, box_array_t *boxes, int face_id){
    int x, y, w, h, i;
    uint32_t color = FACE_COLOR_YELLOW;
    if(face_id < 0){
        color = FACE_COLOR_RED;
    } else if(face_id > 0){
        color = FACE_COLOR_GREEN;
    }
    fb_data_t fb;
    fb.width = image_matrix->w;
    fb.height = image_matrix->h;
    fb.data = image_matrix->item;
    fb.bytes_per_pixel = 3;
    fb.format = FB_BGR888;
    for (i = 0; i < boxes->len; i++){
        // rectangle box
        x = (int)boxes->box[i].box_p[0];
        y = (int)boxes->box[i].box_p[1];
        w = (int)boxes->box[i].box_p[2] - x + 1;
        h = (int)boxes->box[i].box_p[3] - y + 1;
        fb_gfx_drawFastHLine(&fb, x, y, w, color);
        fb_gfx_drawFastHLine(&fb, x, y+h-1, w, color);
        fb_gfx_drawFastVLine(&fb, x, y, h, color);
        fb_gfx_drawFastVLine(&fb, x+w-1, y, h, color);
#if 0
        // landmark
        int x0, y0, j;
        for (j = 0; j < 10; j+=2) {
            x0 = (int)boxes->landmark[i].landmark_p[j];
            y0 = (int)boxes->landmark[i].landmark_p[j+1];
            fb_gfx_fillRect(&fb, x0, y0, 3, 3, color);
        }
#endif
    }
}

// Check if photo capture was successful
bool checkPhoto( fs::FS &fs ) {
  File f_pic = fs.open( FILE_PHOTO );
  unsigned int pic_sz = f_pic.size();
  return ( pic_sz > 100 );
}
//Run face recongintion Command
void runEnrollFace( void ) {
  camera_fb_t * fb = NULL;
  int64_t fr_start;
  int64_t fr_end;
  fr_start = esp_timer_get_time();
  fr_end = esp_timer_get_time();
  Serial.println((fr_end)/100000.0);
  Serial.println((fr_start)/100000.0);
  Serial.println((fr_start-fr_end)/100000.0);
  is_enrolling = 1;
  
  while(is_enrolling && ((fr_start-fr_end)/100000.0) < 10)
  {
    fr_end = esp_timer_get_time();
    Serial.println("Taking a photo...");

    fb = esp_camera_fb_get();
    if (!fb) {
      is_enrolling = 0;
      Serial.println("Camera capture failed");
      return;
    }
    
    size_t out_len, out_width, out_height, out_len2;
    uint8_t * out_buf;
    bool s;
    bool detected = false;
    int face_id = 0;
    dl_matrix3du_t *image_matrix;
    if(fb->width <= 400){
      image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
      if (!image_matrix) {
          esp_camera_fb_return(fb);
          is_enrolling = 0;
          Serial.println("dl_matrix3du_alloc failed");
          return;
      }

      out_buf = image_matrix->item;
      out_len = fb->width * fb->height * 3;
      out_width = fb->width;
      out_height = fb->height;

      s = fmt2rgb888(fb->buf, fb->len, fb->format, out_buf);

    
      if(!s){
          dl_matrix3du_free(image_matrix);
          esp_camera_fb_return(fb);
          is_enrolling = 0;
          Serial.println("to rgb888 failed");
          return;
      }

      box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);
  
      if (net_boxes){
          detected = true;
          face_id = run_face_recognition(image_matrix, net_boxes, "");
          draw_face_boxes(image_matrix, net_boxes, face_id);
          //free(net_boxes->score);
          //free(net_boxes->box);
          //free(net_boxes->landmark);
          //free(net_boxes);
      }
      esp_camera_fb_return(fb);
      dl_matrix3du_free(image_matrix);
    }
    else
    {
      Serial.println("Not right camera size");
      is_enrolling = 0;
      esp_camera_fb_return(fb);
      return;
    }
  }
  if(is_enrolling)
  {
    delete_face(&id_list);
    Serial.println("could not enroll");
    is_enrolling = 0;
  }

}

// Capture Photo and Save it to SPIFFS
void capturePhotoSaveSpiffs( void ) {
  camera_fb_t * fb = NULL; // pointer
  bool ok = 0; // Boolean indicating if the picture has been taken correctly

  do {
    // Take a photo with the camera
    Serial.println("Taking a photo...");

    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }
    
    size_t out_len, out_width, out_height, out_len2;
    uint8_t * out_buf;
    uint8_t * out_buf2;
    bool s;
    bool detected = false;
    int face_id = 0;
    dl_matrix3du_t *image_matrix;
    if(fb->width <= 400){
      image_matrix = dl_matrix3du_alloc(1, fb->width, fb->height, 3);
      if (!image_matrix) {
          esp_camera_fb_return(fb);
          Serial.println("dl_matrix3du_alloc failed");
          return;
      }

      out_buf = image_matrix->item;
      out_len = fb->width * fb->height * 3;
      out_width = fb->width;
      out_height = fb->height;

      s = fmt2rgb888(fb->buf, fb->len, fb->format, out_buf);

    
      if(!s){
          esp_camera_fb_return(fb);
          dl_matrix3du_free(image_matrix);
          Serial.println("to rgb888 failed");
          return;
      }

      box_array_t *net_boxes = face_detect(image_matrix, &mtmn_config);
  
      if (net_boxes){
          detected = true;
          face_id = run_face_recognition(image_matrix, net_boxes, "");
          draw_face_boxes(image_matrix, net_boxes, face_id);
          //free(net_boxes->score);
          //free(net_boxes->box);
          //free(net_boxes->landmark);
          //free(net_boxes);
      }

      s = fmt2jpg(out_buf, out_len, out_width, out_height, PIXFORMAT_RGB888, 90,  &out_buf2, &out_len2 );
      dl_matrix3du_free(image_matrix);
      if(!s){
        esp_camera_fb_return(fb);
        Serial.println("JPEG compression failed");
        return ;
      }
    }
    // Photo file name
    Serial.printf("Picture file name: %s\n", FILE_PHOTO);
    File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);

    // Insert the data in the photo file
    if (!file) {
      Serial.println("Failed to open file in writing mode");
    }
    else {
      if(fb->width > 400){
        file.write(fb->buf, fb->len); // payload (image), payload length
      }else{
        file.write((const uint8_t* )out_buf2, out_len2);
        free(out_buf2);
      }
      Serial.print("The picture has been saved in ");
      Serial.print(FILE_PHOTO);
      Serial.print(" - Size: ");
      Serial.print(file.size());
      Serial.println(" bytes");
    }
    // Close the file
    file.close();
    esp_camera_fb_return(fb);

    // check if file has been correctly saved in SPIFFS
    ok = checkPhoto(SPIFFS);
  } while ( !ok );
}

void faceInit()
{
    
    mtmn_config.type = FAST;
    mtmn_config.min_face = 80;
    mtmn_config.pyramid = 0.707;
    mtmn_config.pyramid_times = 4;
    mtmn_config.p_threshold.score = 0.6;
    mtmn_config.p_threshold.nms = 0.7;
    mtmn_config.p_threshold.candidate_number = 20;
    mtmn_config.r_threshold.score = 0.7;
    mtmn_config.r_threshold.nms = 0.7;
    mtmn_config.r_threshold.candidate_number = 10;
    mtmn_config.o_threshold.score = 0.7;
    mtmn_config.o_threshold.nms = 0.7;
    mtmn_config.o_threshold.candidate_number = 1;
    
    face_id_init(&id_list, FACE_ID_SAVE_NUMBER, ENROLL_CONFIRM_TIMES);
}
