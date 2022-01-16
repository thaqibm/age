#ifndef AGE_INIT_H
#define AGE_INIT_H

#include<ncurses.h>
#include <functional>
#include <map>
#include <utility>
#include <vector>
#include <string>
#include<unistd.h>


namespace AGE{
/**
 * @brief DisplayWindow Class 
 * creates and displays a 
 * window on terminal screen.
 */
class CursesWindow {
    private:
        WINDOW* win; /** Window pointer for the main screen */ 
        int row;  /** Size of the window  row x col*/
        int col;
        int x0; 
        int y0;
        int mx_row; 
        int mx_col;
    public:
        CursesWindow(std::string msg){
            /**
             * @brief Construct a new DisplayWindow object
             * cannot init fields here since the terminal 
             * does not enter curses more until init() is called.
             */
            init(msg);
            mx_row = LINES; /** Init values after we are in curses mode */ 
            mx_col = COLS;
        }
        void draw_windowed(int row, int col, int y0, int x0){
            win = create_win(row, col, y0, x0);
            this->row = row; 
            this->col = col;
            this->x0 = x0; 
            this->y0 = y0;
        }
        /**
         * @brief draw_vect takes a 2d vector and draws it on 'win'
         * 
         * @param v: 2d vector
         * side effects: Mutates the chars printed on the screen
         *              if vector is smaller or bigger then row x col
         *              then resizes the window
        */
        void draw_vect(const std::vector<std::vector<char>>& v){
            if(v.size() != row or v[0].size() != col){
                destroy_win(win);
                y0 = (mx_row - (int)v.size())/2;
                x0 = (mx_col - (int)v[0].size())/2;
                win = create_win(v.size(), v[0].size(), y0, x0);
            }
            for(int i = 0; i<v.size(); ++i){
                for(int j = 0; j<v[0].size(); ++j){
                    mvwaddch(win, i, j, v[i][j]);
                }
            }
            wrefresh(win);
        }
        void setXY(const int x, const int y, char c){
            mvwaddch(win, y, x, c);
            wrefresh(win);
        }
        std::pair<int, int> get_mx_dim(){
            return std::make_pair(mx_row, mx_col);
        }

    static void repl(const std::function<void(int)>& f, size_t fps, int exitKey){
            int ch;
            while(true){
                napms(1000/fps);
                /** Not register long press **/
                ch = getch();
                if (ch != ERR) {
                    while (getch() == ch);
                }
                /** Not register long press **/
                if(ch==exitKey){
                    return;
                }
                f(ch);
            }
        }
        ~CursesWindow(){
            destroy_win(win); /** Clear the window */    
            endwin();			/* End curses mode		  */
        }
    private:
        static void init(const std::string& msg){
            initscr();			/* Start curses mode 		*/
            cbreak();			
            noecho(); /* Disable echoing of chars */
            timeout(0);
            curs_set(0); /* Disable cursor*/
            nodelay(stdscr, true); /* Do not wait for user input*/
            keypad(stdscr, TRUE);
            printw(msg.c_str());
            refresh();
        }
        static WINDOW *create_win(int row, int col, int y0, int x0) {
            WINDOW *local_win;

            local_win = newwin(row, col, y0, x0);
            wborder(local_win, '.', '.', '.', '.', '.', '.', '.', '.');
            wrefresh(local_win);  /* Show that box 		*/
            return local_win;
        }
        static void destroy_win(WINDOW *local_win){
            wborder(local_win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
            wrefresh(local_win);
            delwin(local_win);
        }
};


}
#endif