#include "serial.h"
#include "app.h"

int main(void) {
  //initialize
  app_init();

  while(1) {
    app_handle_sampling_button();
    app_sensor_sample();
  }
}