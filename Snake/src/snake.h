#ifndef SNAKE_H
#define SNAKE_H

#include <list>
#include <utility>
#include <fstream>
#include <sstream>
#include <random>
#include <cassert>
#include <QPainter>

//in any position , there is one of : snake body; food; nothing(grass);
enum class MatrixValueEnum
{
    NOTHING = '0', SNAKE_BODY = '1', FOOD = '2'
};

class Model
{
public:
    size_t GetRow(void) const { return m_playBoard.size(); }
    size_t GetCol(void) const { return m_playBoard.at(0).size(); }
    void EatFood(std::pair<int, int> nextPosition);
    bool PushFoodAt(int row, int col);
    bool ExistFood(int row, int col) const;
    void IncreaseOnlyBody(std::pair<int, int> nextPosition);
    void AppendToBoard(const std::vector<char>& lineData);
    //go to snake body or position overflow , game over
    bool IsGameOver(int row, int col) const;
    // The new coordinates were calculated after the snake's head was moved once
    std::pair<int, int> GetNextPosition(int row_step, int column_step) const
    {
        //new destination that the snake's head intends to go to
        auto old = GetCurrentPosition();
        auto newRow = old.first += row_step;
        auto newCol = old.second += column_step;
        return std::make_pair(newRow, newCol);
    }
    // Get the coordinates of the snake's head
    std::pair<int, int> GetCurrentPosition(void) const
    {
        //Returns the coordinates of the snake-head
        //, which is the value of the first element of m_snakeBody
        auto front = m_snakeBody.front();
        return front;
    }
    void MoveOneStepTo(std::pair<int, int> nextPosition);
    // a new food for the snake to eat
    void CreateFood(void);
    //qt draw pix map : https://zhuanlan.zhihu.com/p/662881605
    void ShowGame(QPainter& painter, int pictureSize) const;
public:
    std::vector<std::vector<char>> m_playBoard;
    std::list<std::pair<int, int>> m_snakeBody;
};

class Control
{
public:
    size_t GetRow(void) const { return m_model.GetRow(); }
    size_t GetCol(void) const { return m_model.GetCol(); }
    // The user enters a character (e / s / f / d)
    //and decides in which direction to move the snake's head
    // new position of snake's head (x,y) = (x,y) + (row_step, column_step)
    bool GoAhead(char userInputDirection);
    bool LoadPlayDataFromFile(std::istream& fin);
    //Up is the direction of decrease of rows
    //, down is the direction of increase of value of rows;
    //the column is the decrease of the left column and the right column
    bool GoAhead(int row_step, int column_step);

    Model m_model;
};

class Snake
{

public:
    size_t GetRow(void) const { return m_control.GetRow(); }
    size_t GetCol(void) const { return m_control.GetCol(); }
    // Load the UI data from the file, store it in an internal container
    //, and then draw the UI according to the container content
    bool LoadPlayDataFromFile(const std::string& file);
    // start game
    bool Play(char direction);//return true if gameover
    // Draw the snake game, and match the data between the UI and the memory

    // nothing  <== '0'
    // food  <== '2'
    // body  <== '1'
    // head  <== '1'
    void PrintMatrix(QPainter& painter, int pictureSize) const;
private:
    Control m_control;
};
#endif
