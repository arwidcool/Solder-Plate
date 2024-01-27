#ifndef __enums_h__
#define __enums_h__

enum ButtonState
{
  IDLE,
  PRESSED,
  RELEASED
};

enum ButtonKind
{
  UP,
  DOWN,
  BACK,
  SELECT,
  NONE
};

#define BTNKIND_STR(kind) (kind == ButtonKind::UP ? "UP" : kind == ButtonKind::DOWN ? "DOWN"   \
                                                    : kind == ButtonKind::BACK   ? "BACK"   \
                                                    : kind == ButtonKind::SELECT ? "SELECT" \
                                                                                 : "NONE")
#define BTNSTATE_STR(state) (state == ButtonState::IDLE ? "IDLE" : state == ButtonState::PRESSED ? "PRESSED" \
                                                                                              : "RELEASED")
#define BTNSTATECHANGE_STR(change) (String(BTNSTATE_STR(change.from)) + " -> " + BTNSTATE_STR(change.to))

#endif