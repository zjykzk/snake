#include "snake.h"

#include <cstddef>

#include "QApplication"
#include "QTimer"
#include "QPainter"
#include "QPoint"
#include "QKeyEvent"
#include "QMessageBox"

namespace snake {
  Snake::Snake(QWidget *parent) : QWidget(parent), key_(Qt::Key_unknown),
    L(luaL_newstate()) {
    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(update()));
    start();
  }

  void Snake::loadResources() {
    if (luaL_loadfile(L, "res/conf.lua") || lua_pcall(L, 0, 0, 0)) {
      QMessageBox::warning(this, tr("error"), 
          tr("configure file loading error"));
      close();
      return;
    }

    update_interval_ = getGlobalNumber(L, "update_interval");
    lua_getglobal(L, "Direction");
    Snake::up_ = getNumberFromTable(L, "Up");
    Snake::down_ = getNumberFromTable(L, "Down");
    Snake::left_ = getNumberFromTable(L, "Left");
    Snake::right_ = getNumberFromTable(L, "Right");

    if (luaL_loadfile(L, "res/snake.lua") || lua_pcall(L, 0, 0, 0)) {
      QMessageBox::warning(this, tr("error"), 
          tr("logic file loading error"));
      close();
      return;
    }
    luaL_setfuncs(L, draw_funcs, 0);
  }

  void Snake::start() {
    loadResources();
    timer_->start(update_interval_);
  }

  void Snake::over() {
  }

  void Snake::paintEvent(QPaintEvent * /*event*/) {
    static const QString strs[] = {
      tr("1"), tr("2"), tr("3")
    };
    static int test_id_ = 0;
    static int x = 100, y = 100;

    switch (key_) {
      case Qt::Key_Up:
      case Qt::Key_W: y -= 5; break;
      case Qt::Key_Down:
      case Qt::Key_S: y += 5; break;
      case Qt::Key_Left:
      case Qt::Key_A: x -= 5; break;
      case Qt::Key_Right:
      case Qt::Key_D: x += 5; break;
      default: return;
    }

    QPainter p(this);
    test_id_ = (test_id_+1) % 3;
    p.drawText(QPoint(x, y), strs[test_id_]);
    key_ = Qt::Key_unknown;
  }

  void Snake::keyPressEvent(QKeyEvent *event) {
    key_ = event->key();
  }

  Snake::~Snake() {
    lua_close(L);
  }

  int getGlobalNumber(lua_State *L, const char *name) {
    lua_getglobal(L, name);
    return (int)lua_tonumber(L, -1);
  }

  int getNumberFromTable(lua_State *L, const char *name) {
    lua_pushstring(name);
    lua_gettable(L, -2);
    int ans = (int) lua_tonumber(L, -1);
    lua_pop(L, 1);
    return ans;
  }

  Snake snake;

  const luaL_Reg draw_funcs[] = {
    {"drawRect", drawRect},
    {"clearRect", clearRect},
    {NULL, NULL},
  };

  void drawRect(lua_State *L) {
    int x = (int) lua_tonumber(L, 1), y = (int) lua_tonumber(L, 2);
    snake.addDrawingRect(x, y);
  }

  void clearRect(lua_State *L) {
    int x = (int) lua_tonumber(L, 1), y = (int) lua_tonumber(L, 2);
    snake.clearDrewRect(x, y);
  }
}

int main(int argc, char *argv[]) {
  using namespace snake;

  QApplication app(argc, argv);
  Snake *snake = new Snake();
  snake->show();
  return app.exec();
}
