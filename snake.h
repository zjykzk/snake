#ifndef SNAKE_SNAKE_H_
#define SNAKE_SNAKE_H_
#include <list>
#include <utility>

#include "QWidget"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luaconf.h"
}

class QTimer;
class list;
class pair;

namespace snake {
  using std::list;
  using std::pair;
  using std::make_pair;

  class Snake : public QWidget {
    Q_OBJECT

    public:
      Snake(QWidget *parent=0);
      void loadResources();
      void start();
      int over(lua_State *L);;
      int drawRect(lua_State *L);
      int delRect(lua_State *L);
      int drasFreshPoint(lua_State *L);
      ~Snake();

    protected:
      void paintEvent(QPaintEvent *event);
      void keyPressEvent(QKeyEvent *event);

    private:
      static int getGlobalNumber(lua_State *L, const char *name);
      static int getNumberFromTable(lua_State *L, const char *name);
      void registerFunction();

      int up_, down_, left_, right_;
      int step_;
      int update_interval_;
      QTimer *timer_;
      int key_;
      lua_State *L;
      list<pair<int, int> > drawing_rects_;
      int fresh_point_x_, fresh_point_y_;
  };
}

#endif // SNAKE_SNAKE_H_
