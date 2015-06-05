#include <stdio.h>
#include <Windows.h>
#include <math.h>

#include "LibCorsairRGB\LibCorsairRGB.h"
#include "LibCorsairRGB\keymaps.h"

int _tmain(int argc, _TCHAR* argv[]) {
  // As this is test code, set the verbosity on to check it's working right
  lcrgb_set_verbosity_level(99);

  // Initialise the library which takes control of the keyboards lighting over USB
  int status = lcrgb_initialise();
  if (status != 0) {
    printf("Failed to intialise LibCorsairRGB\n");
    return status;
  }

  // Make sure we call the deinitialise to unset the USB hooks when we exit this app
  atexit(lcrgb_deinitialise);

  // Make sure the keymap is set for the UK
  lcrgb_set_keymap(lcrgb_ISO_UK);

  // Make sure we start off with a blank canvas
  for(unsigned char key = 0; key < 144; key++) {
    lcrgb_set_key_code(key, 0, 0, 0);
  }
  lcrgb_flush_light_buffer();

  // Assign some colours to the keys via name
  printf("Setting h,j,k,l by name, hit enter to continue\n");
  lcrgb_set_key(lcrgb_key_enum::h, 255, 0, 0);
  lcrgb_set_key(lcrgb_key_enum::j, 127, 200, 0);
  lcrgb_set_key(lcrgb_key_enum::k, 0, 200, 200);
  lcrgb_set_key(lcrgb_key_enum::l, 0, 0, 255);
  lcrgb_flush_light_buffer();
  getchar();
	// Clear the colours for those keys
  lcrgb_set_key(lcrgb_key_enum::h, 0, 0, 0);
  lcrgb_set_key(lcrgb_key_enum::j, 0, 0, 0);
  lcrgb_set_key(lcrgb_key_enum::k, 0, 0, 0);
  lcrgb_set_key(lcrgb_key_enum::l, 0, 0, 0);

  // Set one key at a time red and print the key index (handy when trying to figure out keymaps)
  printf("Setting each key to red one at a time, hit enter to move to the next key\n");
  // Loop through each possible key
  for(unsigned char key = 0; key < 144; key++) {
    printf("Setting: %d (0x%02x)\n", key, key);
    lcrgb_set_key_code(key, 255, 255, 255); // The cast should be fixed in the future!
    lcrgb_flush_light_buffer();
    lcrgb_set_key_code(key, 0, 0, 50); // Sets visited keys to a dim blue
    getchar();
  }

  // Do some side to side swooshy gradienty things for a few seconds
  printf("Side-waves\n");
  for (int i = 0; i < 5; i++) {
    for (int x = 0; x < 22; x++) {
      for (int y = 0; y < 7; y++) {
        lcrgb_set_position(x, y, 255, (255.0f/7.0f)*y, 0);
      }
      lcrgb_flush_light_buffer();
      Sleep(50);
    }
    for (int x = 21; x >= 0; x--) {
      for (int y = 0; y < 7; y++) {
        lcrgb_set_position(x, y, 0, 255.0f/y, (255.0f/7.0f)*y);
      }
      lcrgb_flush_light_buffer();
      Sleep(50);
    }
  }

  // Send out a purple scrolly sine wave
  printf("Sine-waves\n");
  for (int t = 0; t < 250; t++) {
    for (int x = 0; x < 22; x++) {
      for (int y = 0; y < 7; y++) {
        lcrgb_set_position(x, y, 127 * (1.f + sin(0.3f * (float)x - ((float)t * 0.2f))), 50, 127);
      }
    }
    lcrgb_flush_light_buffer();
    Sleep(50);
  }

  printf("Demo done, hit enter to exit\n");
  getchar();

  return 0;
}
