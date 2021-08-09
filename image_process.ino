#include "esp_camera.h"
#include "micro_img.h"

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
// #define CAMERA_MODEL_AI_THINKER

#include "camera_pins.h"

#define FRAME_SIZE FRAMESIZE_QQVGA
#define WIDTH 160
#define HEIGHT 120

int** kernel;
microimg::micro_img micro = microimg::micro_img();

bool setup_camera(framesize_t);
bool capture_still();
void setup() {
    Serial.begin(115200);
    Serial.println(setup_camera(FRAME_SIZE) ? "OK" : "ERR INIT");
    kernel = microimg::sobel_filter_x();
}


void loop() {
    if (!capture_still()) {
        Serial.println("Failed capture");
        delay(2000);

        return;
    }
    
    delay(3000);
}


/**
 *
 */
bool setup_camera(framesize_t frameSize) {
    camera_config_t config;

    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_GRAYSCALE;
    config.frame_size = frameSize;
    config.jpeg_quality = 12;
    config.fb_count = 1;

    bool ok = esp_camera_init(&config) == ESP_OK;

    sensor_t *sensor = esp_camera_sensor_get();
    sensor->set_framesize(sensor, frameSize);

    return ok;
}

/**
 * Capture image and do down-sampling
 */
bool capture_still() {
    camera_fb_t *frame = esp_camera_fb_get();

    if (!frame)
        return false;

    // print_image(frame->buf);

    uint8_t** arr = micro.initialize_array_uint8_t(HEIGHT,WIDTH,255);

    for(int i = 0; i< HEIGHT; i++)
    {
        for(int j = 0; j<WIDTH; j++)
        {
            arr[i][j] = frame->buf[i*WIDTH + j];
        }
    }

    esp_camera_fb_return(frame);
    
    arr = micro.padding(arr, HEIGHT, WIDTH, 3, 1);


    uint8_t** result = micro.conv2d(arr, HEIGHT + 2, WIDTH + 2, kernel, 3, 3);

    print_image(result, HEIGHT, WIDTH);
    micro.delete_arr(result, HEIGHT);

    return true;
}



// Print Image as a 2D Array
void print_image(uint8_t* frame){
  for(int y = 0; y < HEIGHT; y++)
  {
    for(int x = 0; x<WIDTH;x++)
    {
      Serial.print(frame[y * WIDTH + x]);
      Serial.print("\t");
    }
    Serial.println();
  }
  Serial.println();
}


void print_image(uint8_t** frame, int n, int m){
  for(int i = 0; i < n; i++)
  {
    for(int j = 0; j<m; j++)
    {
      Serial.print(frame[i][j]);
      Serial.print("\t");
    }
    Serial.println();
  }
  Serial.println();
}
