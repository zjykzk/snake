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
      void over();
      void addDrawingRect(int x, int y) {
        drawing_rects_.push_back(make_pair(x, y));
      }

      void delFirstRect() {
        if (drawing_rects_.empty()) return;
        drawing_rects_.pop_front();
      }
      ~Snake();

    protected:
      void paintEvent(QPaintEvent *event);
      void keyPressEvent(QKeyEvent *event);

    private:
      int up_, down_, left_, right_;
      int update_interval_;
      QTimer *timer_;
      int key_;
      lua_State *L;
      list<pair<int, int> > drawing_rects_;
  };
}

#endif // SNAKE_SNAKE_H_
