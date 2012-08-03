#include "snake.h"

#include <cstddef>

#include "QApplication"
#include "QTimer"
#include "QPainter"
#include "QPoint"
#include "QKeyEvent"
#include "QMessageBox"
#include "QObject"

static int getGlobalNumber(lua_State *L, const char *name) {
  lua_getglobal(L, name);
  int ans = (int)lua_tonumber(L, -1);
  lua_pop(L, 1);
  return ans;
}

static int getNumberFromTable(lua_State *L, const char *name) {
  lua_pushstring(L, name);
  lua_gettable(L, -2);
  int ans = (int) lua_tonumber(L, -1);
  lua_pop(L, 1);
  return ans;
}

static snake::Snake* snake_obj;

static int drawRect(lua_State *L) {
  int x = (int) lua_tonumber(L, 1), y = (int) lua_tonumber(L, 2);
  snake_obj->addDrawingRect(x, y);
  return 0;
}

static int delRect(lua_State *L) {
  snake_obj->delFirstRect();
  return 0;
}

namespace snake {
  Snake::Snake(QWidget *parent) : QWidget(parent), key_(Qt::Key_unknown),
    L(luaL_newstate()) {
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

    lua_register(L, "drawRect", drawRect);
    lua_register(L, "delRect", delRect);

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

    QMessageBox::information(this, tr("load successfully"), 
      tr("loading successfully"));
  }

  void Snake::start() {
    loadResources();
    timer_->start(update_interval_);
  }

  void Snake::over() {
  }

  void Snake::paintEvent(QPaintEvent * /*event*/) {
    static int x = 200, y = 200;

    switch (key_) {
      case Qt::Key_Up:
      case Qt::Key_W:  y -= 5; break;
      case Qt::Key_Down:
      case Qt::Key_S: y += 5; break;
      case Qt::Key_Left:
      case Qt::Key_A: x -= 5; break;
      case Qt::Key_Right:
      case Qt::Key_D: x += 5; break;
      default: break;
    }

    QPainter p(this);
    p.drawRect(x, y, 10, 10);

    for (list<pair<int, int> >::iterator it = drawing_rects_.begin();
        it != drawing_rects_.end(); ++it) {
      p.drawRect(it->first, it->second, 10, 10);
    }

    key_ = Qt::Key_unknown;
  }

  void Snake::keyPressEvent(QKeyEvent *event) {
    key_ = event->key();
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
  }

  Snake::~Snake() {
    lua_close(L);
  }
}

int main(int argc, char *argv[]) {
  using namespace snake;

  QApplication app(argc, argv);
  snake_obj = new Snake();
  snake_obj->start();
  snake_obj->show();
  return app.exec();
}
