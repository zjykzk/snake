#ifndef SNAKE_SNAKE_H_
#define SNAKE_SNAKE_H_
#include <vector>
#include <utility>

#include "QWidget"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luaconf.h"
}

class QTimer;
class vector;
class pair;

namespace snake {
  using std::vector;
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
        drawing_rects_.pushback(make_pair(x, y));
      }
      void clearDrewRect(int x, int y) {
        clearing_rects_.pushback(make_pair(x, y));
      }
      ~Snake();

    protected:
      void paintEvent(QPaintEvent *event);
      void keyPressEvent(QKeyEvent *event);

    private:
      static int up_, down_, left_, right_;
      int update_interval_;
      QTimer *timer_;
      int key_;
      lua_State *L;
      vector<pair<int, int> > drawing_rects_;
      vector<pair<int, int> > clearing_rects_;
  };
}

#endif // SNAKE_SNAKE_H_
