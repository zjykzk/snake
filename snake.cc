#include "snake.h"

#include <cstddef>

#include "QApplication"
#include "QTimer"
#include "QPainter"
#include "QPoint"
#include "QKeyEvent"
#include "QMessageBox"
#include "QObject"

static int cClosure(lua_State *L) {
  snake::Snake *canvas = (snake::Snake*)lua_topointer(L, lua_upvalueindex(1));
  typedef int (*func)(snake::Snake*, lua_State*);

  func f = (func)lua_topointer(L, lua_upvalueindex(2));
  return f(canvas, L);
}

static int drawRectProxy(snake::Snake *canvas, lua_State *L) {
  return canvas->drawRect(L);
}

static int delRectProxy(snake::Snake *canvas, lua_State *L) {
  return canvas->delRect(L);
}

static int overProxy(snake::Snake *canvas, lua_State *L) {
  return canvas->over(L);
}

static int drawFreshPoint(snake::Snake *canvas, lua_State *L) {
  return canvas->drasFreshPoint(L);
}

namespace snake {
  Snake::Snake(QWidget *parent) : QWidget(parent), key_(Qt::Key_unknown),
    L(luaL_newstate()), is_over_(false) {
    luaL_openlibs(L); 
    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(update()));
  }

  void Snake::loadResources() {
    if (luaL_loadfile(L, "res/conf.lua") || lua_pcall(L, 0, 0, 0)) {
      QMessageBox::warning(this, tr("error"), 
          tr("loading configure file error"));
      close();
      return;
    }
    update_interval_ = getGlobalNumber(L, "update_interval");
    step_ = getGlobalNumber(L, "step");

    lua_getglobal(L, "window");
    setFixedSize(getNumberFromTable(L, "width") * step_,
        getNumberFromTable(L, "height") * step_);
    lua_pop(L, 1);

    if (luaL_loadfile(L, "res/snake.lua") || lua_pcall(L, 0, 0, 0)) {
      QMessageBox::warning(this, tr("error"), 
          tr(lua_tostring(L, -1)));
      close();
      return;
    }
    lua_getglobal(L, "snake");
    up_ = getNumberFromTable(L, "Up");
    down_ = getNumberFromTable(L, "Down");
    left_ = getNumberFromTable(L, "Left");
    right_ = getNumberFromTable(L, "Right");
  }

  void Snake::start() {
    registerFunction();
    loadResources();
    timer_->start(update_interval_);
  }

  int Snake::over(lua_State *L) {
    is_over_ = true;
    fresh_point_x_ = 0;
    drawing_rects_.clear();
    QMessageBox::information(this, tr("Snake Over"), tr("Game Over"));
    lua_getglobal(L, "init");
    lua_pcall(L, 0, 0, 0);
    lua_pop(L, 1);
    return 0;
  }

  int Snake::drasFreshPoint(lua_State *L) {
    fresh_point_x_ = (int) lua_tonumber(L, 1);
    fresh_point_y_ = (int) lua_tonumber(L, 2);
    return 0;
  }

  int Snake::getGlobalNumber(lua_State *L, const char *name) {
    lua_getglobal(L, name);
    int ans = (int)lua_tonumber(L, -1);
    lua_pop(L, 1);
    return ans;
  }

  int Snake::getNumberFromTable(lua_State *L, const char *name) {
    lua_pushstring(L, name);
    lua_gettable(L, -2);
    int ans = (int) lua_tonumber(L, -1);
    lua_pop(L, 1);
    return ans;
  }

  void Snake::paintEvent(QPaintEvent *event) {
    if (is_over_) return;

    int direction = -1;
    switch (key_) {
      case Qt::Key_Up:
      case Qt::Key_W: direction = up_; break;
      case Qt::Key_Down:
      case Qt::Key_S: direction = down_; break;
      case Qt::Key_Left:
      case Qt::Key_A: direction = left_; break;
      case Qt::Key_Right:
      case Qt::Key_D: direction = right_; break;
      default: break;
    }

    lua_getglobal(L, "snake");
    lua_pushstring(L, "move");
    lua_gettable(L, -2);
    lua_pushnumber(L, direction);
    if (lua_pcall(L, 1, 0, 0)) {
      QMessageBox::warning(this, tr("error"), tr(lua_tostring(L, -1)));
    }
    lua_pop(L, 1);

    QPainter p(this);
    for (list<pair<int, int> >::iterator it = drawing_rects_.begin();
        it != drawing_rects_.end(); ++it) {
      p.drawRect(it->first * step_, it->second * step_, step_, step_);
    }

    if (fresh_point_x_ > 0 || fresh_point_y_ > 0)
      p.drawRect(fresh_point_x_ * step_, fresh_point_y_ * step_, step_, step_);
    key_ = Qt::Key_unknown;
  }

  void Snake::keyPressEvent(QKeyEvent *event) {
    key_ = event->key();
  }

  int Snake::drawRect(lua_State *L) {
    int x = (int) lua_tonumber(L, 1), y = (int) lua_tonumber(L, 2);
    drawing_rects_.push_back(make_pair(x, y));
    return 0;
  }

  int Snake::delRect(lua_State *L) {
    if (drawing_rects_.empty()) return 0;
    drawing_rects_.pop_front();
    return 0;
  }

  void Snake::registerFunction() {
    lua_newtable(L);
    lua_pushlightuserdata(L, (void *)this);
    lua_pushlightuserdata(L, (void *)&drawRectProxy);
    lua_pushcclosure(L, &cClosure, 2);
    lua_setfield(L, -2, "drawRect");

    lua_pushlightuserdata(L, (void *)this);
    lua_pushlightuserdata(L, (void *)&delRectProxy);
    lua_pushcclosure(L, &cClosure, 2);
    lua_setfield(L, -2, "delRect");

    lua_pushlightuserdata(L, (void *)this);
    lua_pushlightuserdata(L, (void *)&overProxy);
    lua_pushcclosure(L, &cClosure, 2);
    lua_setfield(L, -2, "over");

    lua_pushlightuserdata(L, (void *)this);
    lua_pushlightuserdata(L, (void *)&drawFreshPoint);
    lua_pushcclosure(L, &cClosure, 2);
    lua_setfield(L, -2, "drawFreshPoint");

    lua_setglobal(L, "Canvas");
  }

  Snake::~Snake() {
    lua_close(L);
  }
}

int main(int argc, char *argv[]) {
  using namespace snake;

  QApplication app(argc, argv);
  Snake *snake_obj = new Snake();
  snake_obj->start();
  snake_obj->show();
  return app.exec();
}
