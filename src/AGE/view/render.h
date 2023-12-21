#include <utility>
#include <vector>
#include <functional>
#include "../controller/keyCodes.h"
#include "DisplayWindow.h"


namespace AGE{
    /*
    * frameRate 30 (30 ticks / second).
    */
    constexpr const int frameRate = 30; 


template<template<class...> class Seq, typename P, typename... Rest>
class Seq2DRender{
    protected:
        using Pixle = P;
        using Line = Seq<Pixle, Rest...>;
        using State = Seq<Line>;


    private:
        State state; 
    protected:
        size_t ticks; 


    public:
        Seq2DRender(int row, int col, size_t ticks): state(row, Line(col)), ticks{ticks} {
        }
        virtual void render() = 0;
        virtual void runLoop(std::function<State&(State&, int)> f, int exitCode) = 0;
        void setState(const State& state2)
        {
            state = state2;
            render();
        }
        void setState(State&& state2)
        {
            state = std::move(state2);
            render();
        }
        void setPixle(size_t i, size_t j, Pixle p){
            state[i][j] = p;
        }
        State& getState(){
            return state;
        }
        virtual ~Seq2DRender() {

        }

};
class CursesRender : public Seq2DRender<std::vector, char>{

    using base = Seq2DRender<std::vector, char>;
    using Pixle = base::Pixle;
    public:
        using State = base::State;
    private:
    	std::shared_ptr<AGE::CursesWindow> DW;
    public:
        CursesRender(int row, int col, size_t ticks, std::shared_ptr<AGE::CursesWindow> DW): 
        base{row, col, ticks},
        DW{std::move(DW)}
        {
            int startx, starty;
            std::pair<int, int> p = this->DW->get_mx_dim();
            starty = (p.first - row) / 2;  /* Calculating for a center placement */
            startx = (p.second - col) / 2; /* of the window		*/
            this->DW->draw_windowed(row, col, starty, startx);
        }
        void render() override
        {
            DW->draw_vect(getState());
        }

        void runLoop(std::function<State&(State&, int)> f, int exitkey = 'q') override {
            DW->repl([this, &f](int ch)->void {
                this->setState(f(this->getState(), ch));
                this->render();
            }, ticks, exitkey);
        }
    private: 
        void mutate_render(size_t x, size_t y, Pixle c){
            DW->setXY(x, y, c);
        }
};

}
