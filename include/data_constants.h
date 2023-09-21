#ifndef DATACONSTANTS_H_INCLUDED
#define DATACONSTANTS_H_INCLUDED

#include "images_directions.h"

const uint8_t *ImageFromNumber(int number) {
  switch (number) {
  case 1:
    return img_direction_arrive;
  case 2:
    return img_direction_notificaiton_right;
  case 3:
    return img_direction_arrive_left;
  case 4:
    return img_direction_notificaiton_sharp_right;
  case 5:
    return img_direction_arrive_right;
  case 6:
    return img_direction_notification_left;
  case 7:
    return img_direction_arrive_straight;
  case 8:
    return img_direction_notification_sharp_left;
  case 9:
    return img_direction_close;
  case 10:
    return img_direction_notification_slight_left;
  case 11:
    return img_direction_continue;
  case 12:
    return img_direction_notification_slight_right;
  case 13:
    return img_direction_continue_left;
  case 14:
    return img_direction_notification_straight;
  case 15:
    return img_direction_continue_right;
  case 16:
    return img_direction_off_ramp_left;
  case 17:
    return img_direction_continue_slight_left;
  case 18:
    return img_direction_off_ramp_right;
  case 19:
    return img_direction_continue_slight_right;
  case 20:
    return img_direction_off_ramp_slight_left;
  case 21:
    return img_direction_off_ramp_slight_right;
  case 22:
    return img_direction_continue_uturn;
  case 23:
    return img_direction_on_ramp_left;
  case 24:
    return img_direction_depart;
  case 25:
    return img_direction_on_ramp_right;
  case 26:
    return img_direction_depart_left;
  case 27:
    return img_direction_on_ramp_sharp_left;
  case 28:
    return img_direction_depart_right;
  case 29:
    return img_direction_on_ramp_sharp_right;
  case 30:
    return img_direction_depart_straight;
  case 31:
    return img_direction_on_ramp_slight_left;
  case 32:
    return img_direction_end_of_road_left;
  case 33:
    return img_direction_on_ramp_slight_right;
  case 34:
    return img_direction_end_of_road_right;
  case 35:
    return img_direction_on_ramp_straight;
  case 36:
    return img_direction_flag;
  case 37:
    return img_direction_rotary;
  case 38:
    return img_direction_fork;
  case 39:
    return img_direction_rotary_left;
  case 40:
    return img_direction_fork_left;
  case 41:
    return img_direction_fork_right;
  case 42:
    return img_direction_rotary_sharp_left;
  case 43:
    return img_direction_fork_slight_left;
  case 44:
    return img_direction_rotary_sharp_right;
  case 45:
    return img_direction_fork_slight_right;
  case 46:
    return img_direction_rotary_slight_left;
  case 47:
    return img_direction_fork_straight;
  case 48:
    return img_direction_rotary_slight_right;
  case 49:
    return img_direction_invalid;
  case 50:
    return img_direction_rotary_straight;
  case 51:
    return img_direction_invalid_left;
  case 52:
    return img_direction_roundabout;
  case 53:
    return img_direction_invalid_right;
  case 54:
    return img_direction_roundabout_left;
  case 55:
    return img_direction_invalid_slight_left;
  case 56:
    return img_direction_roundabout_right;
  case 57:
    return img_direction_invalid_slight_right;
  case 58:
    return img_direction_roundabout_sharp_left;
  case 59:
    return img_direction_invalid_straight;
  case 60:
    return img_direction_roundabout_sharp_right;
  case 61:
    return img_direction_invalid_uturn;
  case 62:
    return img_direction_roundabout_slight_left;
  case 63:
    return img_direction_merge_left;
  case 64:
    return img_direction_roundabout_slight_right;
  case 65:
    return img_direction_merge_right;
  case 66:
    return img_direction_roundabout_straight;
  case 67:
    return img_direction_merge_slight_left;
  case 68:
    return img_direction_turn_left;
  case 69:
    return img_direction_merge_slight_right;
  case 70:
    return img_direction_turn_right;
  case 71:
    return img_direction_merge_straight;
  case 72:
    return img_direction_turn_sharp_left;
  case 73:
    return img_direction_new_name_left;
  case 74:
    return img_direction_turn_sharp_right;
  case 75:
    return img_direction_new_name_right;
  case 76:
    return img_direction_turn_slight_left;
  case 77:
    return img_direction_new_name_sharp_left;
  case 78:
    return img_direction_turn_slight_right;
  case 79:
    return img_direction_new_name_sharp_right;
  case 80:
    return img_direction_turn_straight;
  case 81:
    return img_direction_new_name_slight_left;
  case 82:
    return img_direction_updown;
  case 83:
    return img_direction_new_name_slight_right;
  case 84:
    return img_direction_uturn;
  case 85:
    return img_direction_new_name_straight;
  }
  return nullptr;
}

#endif // DATACONSTANTS_H_INCLUDED
