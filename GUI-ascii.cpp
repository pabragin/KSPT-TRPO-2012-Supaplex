#include <curses.h>
#include <stdlib.h>
#include <string>

using namespace std;

#define ENTER 10
#define ESCAPE 27
class GUI
{
private: 
void init_curses()
{
        initscr();
        start_color();
        init_pair(1,COLOR_WHITE,COLOR_BLUE);
        init_pair(2,COLOR_BLUE,COLOR_WHITE);
        init_pair(3,COLOR_RED,COLOR_WHITE);
	init_pair(4,COLOR_RED,COLOR_BLUE);
        curs_set(0);
        noecho();
        keypad(stdscr,TRUE);
}

void draw_menubar()
{
	WINDOW *menubar;
	bkgd(COLOR_PAIR(1));
    	menubar=subwin(stdscr,1,80,0,0);
    	wbkgd(menubar,COLOR_PAIR(2));
    	waddstr(menubar,"Menu");
    	wattron(menubar,COLOR_PAIR(3));
    	waddstr(menubar,"(F1)");
    	wattroff(menubar,COLOR_PAIR(3));
    	wmove(menubar,0,15);
    	waddstr(menubar,"Help");
    	wattron(menubar,COLOR_PAIR(3));
    	waddstr(menubar,"(F2)");
    	wattroff(menubar,COLOR_PAIR(3));
    	wmove(menubar,0,30);
    	waddstr(menubar,"About");
    	wattron(menubar,COLOR_PAIR(3));
    	waddstr(menubar,"(F3)");
    	wattroff(menubar,COLOR_PAIR(3));
	wmove(menubar,0,45);
    	waddstr(menubar,"Exit");
    	wattron(menubar,COLOR_PAIR(3));
    	waddstr(menubar,"(F4)");
    	wattroff(menubar,COLOR_PAIR(3));
}

WINDOW **draw_game_win(WINDOW *game_win)
{
	WINDOW **frames;
	frames=(WINDOW **)malloc(3*sizeof(WINDOW *));
	frames[0]=subwin(game_win,16,60,4,2);//игровое поле
	wbkgd(frames[0],COLOR_PAIR(2));
	box(frames[0],ACS_VLINE,ACS_HLINE);
	frames[1]=subwin(game_win,16,16,4,62);//очки
	wbkgd(frames[1],COLOR_PAIR(2));
	box(frames[1],ACS_VLINE,ACS_HLINE);
	frames[2]=subwin(game_win,3,76,20,2);//ходы
	wbkgd(frames[2],COLOR_PAIR(2));
	box(frames[2],ACS_VLINE,ACS_HLINE);
	wmove(frames[1],1,1);
	wattron(frames[1],COLOR_PAIR(3));
    	waddstr(frames[1],"Score:");
    	wmove(frames[1],3,1);
	wattron(frames[1],COLOR_PAIR(3));
    	waddstr(frames[1],"Moves:");
	wmove(frames[1],5,1);
	wattron(frames[1],COLOR_PAIR(3));
    	waddstr(frames[1],"Lambdas:");
	wmove(frames[2],1,1);
	wattron(frames[2],COLOR_PAIR(3));
    	waddstr(frames[2],"Mov:");
	touchwin(stdscr);
	refresh();
	return frames;
}

void draw_map(char **map, int column, int row, WINDOW *game_win)
{
	int i = 0;
	int j = 0;
	for(i=0;i<row; i++){
		for (j=0;j<column; j++){
			 mvwaddch(game_win, row+1, column+1, map[i][j]);
		}
	}
	touchwin(stdscr);
        refresh();
}

void draw_points(int Score, int Moves, int Lambdas, const char *Mov, WINDOW **frames)
{
	char Score_str[5];
	sprintf(Score_str, "%d", Score);
	char Moves_str[5];
	sprintf(Moves_str, "%d", Moves);
	char Lambdas_str[5];
	sprintf(Lambdas_str, "%d", Lambdas);
	mvwaddstr(frames[1], 1, 8, "       ");
	mvwaddstr(frames[1], 3, 8, "       ");
	mvwaddstr(frames[1], 5, 10, "     ");
	mvwaddstr(frames[2], 1, 6, "                                                                     ");
	mvwaddstr(frames[1], 1, 8, Score_str);
	mvwaddstr(frames[1], 3, 8, Moves_str);
	mvwaddstr(frames[1], 5, 10, Lambdas_str);
	string s;
	s = Mov;
	if (s.size()>69)
		mvwprintw(frames[2], 1,6, s.substr(s.size()-69, s.size()).c_str());
	else
		mvwprintw(frames[2], 1,6, s.c_str());
	touchwin(stdscr);
        refresh();
}

void about_game_win(WINDOW *frame)
{
	wmove(frame,1,1);
	wattron(frame,COLOR_PAIR(3));
    	waddstr(frame,"This game was created by:\n Zaychenko Evgeny\n Bragin Pavel\n Hozyainov Ivan\n and Yarov Maxim");
	touchwin(stdscr);
	refresh();
}

void help_game_win(WINDOW *frame)
{
	wmove(frame,1,1);
	wattron(frame,COLOR_PAIR(3));
    	waddstr(frame,"This game is not ready yet");
	touchwin(stdscr);
	refresh();
}

void delete_menu(WINDOW **items,int count)
{
        int i;
        for (i=0;i<count;i++)
                delwin(items[i]);
        free(items);
	touchwin(stdscr);
        refresh();
}

WINDOW **draw_menu(int start_col)
{
        int i;
        WINDOW **items;
        items=(WINDOW **)malloc(5*sizeof(WINDOW *));

        items[0]=newwin(6,19,1,start_col);
        wbkgd(items[0],COLOR_PAIR(2));
        box(items[0],ACS_VLINE,ACS_HLINE);
        items[1]=subwin(items[0],1,17,2,start_col+1);
        items[2]=subwin(items[0],1,17,3,start_col+1);
        items[3]=subwin(items[0],1,17,4,start_col+1);
        items[4]=subwin(items[0],1,17,5,start_col+1);
        wprintw(items[1],"Load_Map");
	wprintw(items[2],"Edit_Map");
	wprintw(items[3],"Human_play");
	wprintw(items[4],"AI_test");
        wbkgd(items[1],COLOR_PAIR(1));
        wrefresh(items[0]);
        return items;
}

int scroll_menu(WINDOW **items,int count,int menu_start_col)
{
        int key;
        int selected=0;
        while (1) {
                key=getch();
                if (key==KEY_DOWN || key==KEY_UP) {
                        wbkgd(items[selected+1],COLOR_PAIR(2));
                        wnoutrefresh(items[selected+1]);
                        if (key==KEY_DOWN) {
                                selected=(selected+1) % count;
                        } else {
                                selected=(selected+count-1) % count;
                        }
                        wbkgd(items[selected+1],COLOR_PAIR(1));
                        wnoutrefresh(items[selected+1]);
                        doupdate();
                } else if (key==ESCAPE) {
                        return -1;
                } else if (key==ENTER) {
                        return selected;
                }
        }
}
void draw_game_name()
{
	WINDOW *gamename;
	gamename=subwin(stdscr,3,20,1,30);
    	wbkgd(gamename,COLOR_PAIR(1));
    	box(gamename,ACS_VLINE|A_BOLD,ACS_HLINE|A_BOLD);
    	attron(A_BLINK|A_BOLD|A_UNDERLINE);
    	move(2,36);
    	printw("SUPAPLEX");
    	attroff(A_BLINK|A_BOLD|A_UNDERLINE);
    	refresh();
}
public:
GUI()
{
	int key, selected_item;

    	init_curses();

	WINDOW *game_win;    
	game_win=subwin(stdscr,19,76,4,2);
    	wbkgd(game_win,COLOR_PAIR(2));

    	draw_menubar();
	draw_game_name();

	do
	{
        WINDOW **menu_items;
	key=getch();
	if (key==KEY_F(1)) {	
            	menu_items=draw_menu(0);
            	selected_item=scroll_menu(menu_items,4,0);
            	delete_menu(menu_items,5);
        if (selected_item>=0){
		werase(game_win);
                WINDOW **gw = draw_game_win(game_win);
		string s= "12345678901234567890123456789012345678901234567890123456789012345678";
		draw_points(100, 10, 1, s.c_str(), gw);
	}
	}
	if (key==KEY_F(2)) {
                werase(game_win);
                help_game_win(game_win);
		}
	if (key==KEY_F(3)) {
		werase(game_win);
                about_game_win(game_win);
		}
	}while (key!=KEY_F(4));
    	endwin();
}
};
int main()
{
	GUI* nGUI = new GUI();
    	return 0;
}
