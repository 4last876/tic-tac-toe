#include <iostream>
#include <memory>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <functional>

constexpr int SCREEN_WIDTH = 1920;
constexpr int SCREEN_HEIGHT = 1080;
enum class SCENE {MENU,PLAY};
constexpr static int row = 3;

class Board {
  constexpr static char defult_filed_char = 'a';
  char cells[row][row];
  enum action_state {PRINT,RESET,IS_CELL_FREE,UPDATE};
  class action;
  public:
  Board(){
    reset();
  } 
  std::pair<int,int> convert(int);
 char get_cell(int x,int y);
 void display_board();
 void update_cell(int,char);
 bool is_cell_free(int);
  bool is_cell_free(int,int);
 void reset();
};

class Player{
  int index;
  public:
  char symbol;
  int val;
  Player(char symbol_) : symbol(symbol_), index(0),val(0){}

  bool operator==(const Player& player){
    return symbol == player.symbol ? 1 : 0;
  }

template<typename T>void get_move(T &t,int index){
  if(index > (row*row)) return;
  t(index,symbol);
}
};

class Board::action{
   action_state state; 
    Board* board;
    char val;
    int index;
    public:
    action(action_state state_,Board* board_) : state(state_),board(board_){}
    action(action_state state_,char val_,int index_,Board* board_) : state(state_),board(board_),val(val_),index(index_){}
   
    void operator()(){
    switch (state)
    {
    case action_state::PRINT:
      for(int y = 0; y < row; ++y){
    for(int x = 0; x < row; ++x){
      std::cout << board->cells[y][x] << ' ';
    }
    std::cout << std::endl;
  }
      break;

      case action_state::RESET:
      for(int y = 0; y < row; ++y){
    for(int x = 0; x < row; ++x){
      board->cells[y][x] = defult_filed_char;
    }
  }
      break;

      case action_state::UPDATE:
      auto au = board->convert(index);
      for(int y = 0; y < row; ++y){
    for(int x = 0; x < row; ++x){
      board->cells[au.first][au.second] = val;
    }
  }
      break;

    }
    }
  };

void Board::reset(){
  Board::action(action_state::RESET,this)();
}

void Board::display_board(){
  Board::action(action_state::PRINT,this)();
}

void Board::update_cell(int index,char val){
  Board::action(action_state::UPDATE,val,index,this)();
}

bool Board::is_cell_free(int index){
     auto au = convert(index);
     return cells[au.first][au.second] == defult_filed_char ? 1 : 0;
}

bool Board::is_cell_free(int y,int x){
     return cells[y][x] == defult_filed_char ? 1 : 0;
}

std::pair<int,int> Board::convert(int index){
  int indexX = (index % row) - 1;
  int indexY = (index / row);
return std::make_pair(indexY,indexX);
}

char Board::get_cell(int x, int y){
return cells[y][x];
}

class Image{

    std::shared_ptr<SDL_Renderer> render;
    public:
    Image(std::shared_ptr<SDL_Renderer> render_) : render(render_){
       int flags = IMG_INIT_JPG | IMG_INIT_PNG;
        if ((IMG_Init(flags) & flags) != flags) {
        std::cout << "Error: " << SDL_GetError() << std::endl; 
     }   
    }

    SDL_Texture* loadedsurface(std::string str){

     SDL_Surface* loaded = IMG_Load(str.c_str());
     if(loaded == nullptr){
        std::cout << "Error: " << SDL_GetError() << std::endl; 
     }
     SDL_Texture* texture = SDL_CreateTextureFromSurface(render.get(),loaded);
 
      if(!texture){
        std::cout << "текстура не действительна "<< SDL_GetError() << std::endl;
      }
     SDL_FreeSurface(loaded);
     return texture;
    }
};

class Scene{
    public:
    std::shared_ptr<SDL_Renderer> render;
    Image img;
    Scene(std::shared_ptr<SDL_Renderer> render_) : render(render_),img(render_){}
    
    virtual void show() = 0;

    virtual SCENE update(bool& quit) = 0;

    virtual ~Scene() {};
};


class Menu : public Scene {
// крестики нолики
Board board;
Player playerx;
Player playery;
Player* current_player;

//
    struct field{
        field(SDL_Rect rect_) :rect(rect_),is_empty(1) {}
        bool is_empty;
        SDL_Rect rect;
        static SDL_Texture* textureO;
        static SDL_Texture* textureX;
        SDL_Texture* current_texture;
       static void setTexture(SDL_Texture* textureO_, SDL_Texture* textureX_){
        textureO = textureO_;
        textureX = textureX_;
       }
    };
    SDL_Texture* texture;
    std::vector<field> fields;
    const int WIDTH = 500;
    const int HEIGHT = 500;
    const SDL_Rect ramka = { (SCREEN_WIDTH / 2) - (WIDTH / 2), (SCREEN_HEIGHT / 2) - (HEIGHT / 2), WIDTH + 2 , HEIGHT + 2};
    const SDL_Rect rect1 = { (SCREEN_WIDTH / 2) - (WIDTH / 2), (SCREEN_HEIGHT / 2) - (HEIGHT / 2), WIDTH , HEIGHT};
    public:
    Menu(std::shared_ptr<SDL_Renderer> render_) : Scene(render_) ,playerx('X'),playery('Y'),current_player(&playerx){
         field::setTexture(img.loadedsurface("/home/lolzs/Изображения/photo_2026-01-17_16-44-34.jpg"),img.loadedsurface("/home/lolzs/Изображения/photo_2026-01-17_16-44-37.jpg"));
         texture = img.loadedsurface("/home/lolzs/Изображения/photo_2026-01-17_16-49-21.jpg");
         initRects();
    };
    ~Menu(){
        SDL_DestroyTexture(field::textureO);
        SDL_DestroyTexture(field::textureX);
        SDL_DestroyTexture(texture);
    }

    void initRects(){
        int szField = sqrt(((WIDTH * HEIGHT) / 9));
        std::cout << szField  << std::endl;
    for(int y = 0; y < sqrt(9); ++y){
        for(int x = 0; x < sqrt(9); ++x){
            fields.emplace(fields.end(),SDL_Rect{x*(szField),y*(szField),szField,szField});
         }
      }
    }

    void show(){
    SDL_SetRenderDrawColor(render.get(), 0xFF, 0xFF, 0xFF, 0xFF );
    SDL_RenderClear(render.get());
    
    SDL_RenderSetViewport(render.get(),&rect1);
    SDL_RenderCopy(render.get(),texture,nullptr,nullptr); 
    for(int i = 0; i < 9; ++i){
        SDL_SetRenderDrawColor(render.get(), 0x0, 0x0, 0xFF, 0x0 );
            SDL_RenderDrawRect(render.get(),&fields[i].rect);
            if(!fields[i].is_empty){
            SDL_RenderCopy(render.get(),fields[i].current_texture,nullptr,&fields[i].rect);
            SDL_RenderDrawRect(render.get(),&fields[i].rect);
            }
         }
    SDL_RenderSetViewport(render.get(),nullptr);
    SDL_SetRenderDrawColor(render.get(), 0x0, 0x0, 0x0, 0x0 );
    SDL_RenderDrawRect(render.get(),&ramka); 
    SDL_RenderPresent(render.get());
    }

    SCENE update(bool& quit){
    SDL_Event e;
    SCENE type = SCENE::MENU;
    while(SDL_PollEvent(&e) != 0){
        if(e.type == SDL_QUIT){
            quit = !quit;
        }
        if(e.key.type == SDL_KEYDOWN){
            if(e.key.keysym.sym  == SDLK_SPACE){
               type = SCENE::PLAY;
            }
        }
 if( e.type == SDL_MOUSEBUTTONDOWN)
    {
        int x, y;
        SDL_GetMouseState( &x, &y );
        x -=  rect1.x;
        y -=  rect1.y;
     for(int index = 0; index < 9; ++index){
          if(y > fields[index].rect.y && y < (fields[index].rect.y + fields[index].rect.h) && x > fields[index].rect.x && x < (fields[index].rect.x + fields[index].rect.w) && fields[index].is_empty){
           
          auto func = std::bind(&Board::update_cell,&board,std::placeholders::_1,std::placeholders::_2);
          if(board.is_cell_free(index + 1)){
          current_player->get_move(func,index + 1);
          fields[index].current_texture = current_player->symbol == playery.symbol ? field::textureO : field::textureX;
          fields[index].is_empty = 0;
          }
          if(check_winner(current_player)){
          std::cout << "ПОБЕДИТЕЛЬ " << current_player->symbol << std::endl;
          exit(2);
          }else if(check_draw()){
          std::cout << "НИЧЬЯ " << std::endl;
          exit(2);
         }
          switch_player();
           }
      }
    }
}
    return type;
    }

    //крестики нолики 

    bool check_winner(Player* player){
for(int y = 0; y < row; ++y){
  player->val = 0;
    for(int x = 0; x < row; ++x){
      if(board.get_cell(x,y) == player->symbol) player->val += 1;
      if(player->val == 3) return 1;
    }
  }

for(int y = 0; y < row; ++y){
  player->val = 0;
    for(int x = 0; x < row; ++x){
      if(board.get_cell(y,x) == player->symbol) player->val += 1;
      if(player->val == 3) return 1;
    }
  }

  if(board.get_cell(0,0)== player->symbol && board.get_cell(1,1)== player->symbol && board.get_cell(2,2)== player->symbol ) return 1;
  if(board.get_cell(0,2)== player->symbol && board.get_cell(1,1)== player->symbol && board.get_cell(2,0)== player->symbol ) return 1;
  return 0;
}

void switch_player(){
  if((*current_player) == playerx)
  current_player = &playery;
  else if((*current_player) == playery)
  current_player = &playerx;
}

bool check_draw(){
for(int y = 0; y < row; ++y){
    for(int x = 0; x < row; ++x){
    if(board.is_cell_free(x,y)) return 0;
      }
  }
  return 1;
}

};

SDL_Texture* Menu::field::textureX = nullptr;
SDL_Texture* Menu::field::textureO = nullptr;

class Play : public Scene {
    SDL_Texture* texture;
    public:
    Play(std::shared_ptr<SDL_Renderer> render_) : Scene(render_){
        texture = img.loadedsurface("/home/lolzs/Изображения/q.jpeg");
    };
    ~Play(){
    SDL_DestroyTexture(texture);
   }
    void show(){
    SDL_SetRenderDrawColor(render.get(), 0xFF, 0xFF, 0xFF, 0xFF );
    SDL_RenderClear(render.get());

    SDL_Rect fillRect = { 0, 0, SCREEN_WIDTH , SCREEN_HEIGHT};
    SDL_RenderCopy(render.get(),texture,nullptr,&fillRect);
    
    SDL_RenderPresent(render.get());
    }

    SCENE update(bool& quit){
    SDL_Event e;
    SCENE type = SCENE::PLAY;
    while(SDL_PollEvent(&e) != 0){
        if(e.type == SDL_QUIT){
            quit = !quit;
        }
        
        if(e.key.type == SDL_KEYDOWN){
            if(e.key.keysym.sym == SDLK_SPACE){
              type = SCENE::MENU;
            }
        }
    }
    return type;
    }
};

class Printer{
std::shared_ptr<SDL_Renderer> render;
std::shared_ptr<SDL_Window> win;
SDL_Surface* surfaceWin = nullptr;
std::unique_ptr<Scene> currentScene = nullptr;
SCENE currentSceneEnum;

constexpr static void DeleterRend(SDL_Renderer* render){
SDL_DestroyRenderer(render);
}


constexpr static void DeleterWin(SDL_Window* win){
SDL_DestroyWindow(win);
}

constexpr static void DeleterTexture(SDL_Texture* text){
SDL_DestroyTexture(text);
}

void updateScene(){
static SCENE lastScene = (SCENE)1000;

if(lastScene == currentSceneEnum){
    return;
}
    
switch (currentSceneEnum)
{
case SCENE::MENU:
currentScene.reset(new Menu(render));
    break;

case SCENE::PLAY:
currentScene.reset(new Play(render));
    break;
}
 lastScene = currentSceneEnum;
}

    public:
    Printer(){
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        std::cout << "Error: " << SDL_GetError() << std::endl;
    }

    win.reset(SDL_CreateWindow("Love",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN),DeleterWin);
    if(win.get() == nullptr){
        std::cout << "Error: " << SDL_GetError() << std::endl;
    }

    render.reset(SDL_CreateRenderer(win.get(),-1,SDL_RENDERER_ACCELERATED),DeleterRend);

    if(render.get() == nullptr){
        std::cout << "Error: " << SDL_GetError() << std::endl;
    } 
     currentSceneEnum = SCENE::MENU;
     updateScene();
    }

    void run(){
    bool quit = false;
    while(!quit){
    currentSceneEnum = currentScene->update(quit);
    currentScene->show();
    updateScene();
}
SDL_Quit();
    }
};



int main(){
   Printer pr;
   pr.run();

}
