#include <array>
#include <iostream>
#include <string>
#include <vector>

class SquareOne
{
private:
    // types
    enum class Piece
    {
        E,  // edge piece
        C1, // 1st half of corner piece
        C2  // 2nd half of corner piece
    };
    using Layer = std::array<Piece, 12>;

    // constants
    static constexpr Layer CUBE_U = {
        Piece::C1, Piece::C2, Piece::E,
        Piece::C1, Piece::C2, Piece::E,
        Piece::C1, Piece::C2, Piece::E,
        Piece::C1, Piece::C2, Piece::E
    };
    static constexpr Layer CUBE_D = {
        Piece::E, Piece::C1, Piece::C2, 
        Piece::E, Piece::C1, Piece::C2, 
        Piece::E, Piece::C1, Piece::C2, 
        Piece::E, Piece::C1, Piece::C2 
    };
    
    // states
    Layer stateU;
    Layer stateD;


    static void shift(Layer& layer, int amount)
    {
        if (amount == 0)
        {
            return;
        }
        Layer copy;
        for (int i = 0; i < 12; i++)
        {
            int modIndex = (i + amount) % 12;
            modIndex = (modIndex >= 0) ? modIndex : (modIndex + 12); // 😭
            copy[i] = layer[modIndex];
        }
        layer = copy;
    }

    static char pieceToChar(Piece p)
    {
        switch (p)
        {
            case Piece::C1:
                return 'C';
            case Piece::C2:
                return 'D';
            case Piece::E:
                return 'E';
            default:
                return 'X';
        }
    }

    static Piece charToPiece(char c)
    {
        switch (c)
        {
            case 'C':
                return Piece::C1;
                break;
            case 'D':
                return Piece::C2;
                break;
            default:
                std::cout << "WARNING: Invalid char in charToPiece: " << c << "; using E instead\n";
            case 'E':
                return Piece::E;
        }
    }

    static inline bool layerSolved(Layer l)
    // verifies solved by check if every 3rd piece is the same
    {
        return (l[0] == l[3]) && (l[3] == l[6]) && (l[6] == l[9])
            && (l[1] == l[4]) && (l[4] == l[7]) && (l[7] == l[10]); // 2 pieces enough checking?
    }

    static void printLayer(Layer& layer)
    {
        for (int i = 0; i < 12; i++)
        {
            std::cout << pieceToChar(layer[i]);
        }
        std::cout << std::endl;
    }

    static inline int incrementMove(int move)
    {
        if (move <= 0)
        {
            return (-move) + 1;
        }
        else
        {
            if (move < 6)
            {
                return -move;
            }
            else
            {
                return 0;
            }
        }
    }

    void solver(std::vector<int>& progress, std::vector<int>& bestSolution)
    // DFS
    // TODO: use BFS to find optimal first
    {
        if (isSolved())
        {
            if (progress.size() < bestSolution.size())
            {
                bestSolution = progress;
                std::cout << "Found solution: ";
                printAlg(progress);
            }
            return;
        }
        else if (progress.size() >= bestSolution.size())
        {
            return;
        }
        int dMove = 0, uMove = 0;
        do
        {
            do
            {
                move(uMove, dMove);
                if (slice())
                {
                    progress.push_back(uMove);
                    progress.push_back(dMove);
                    solver(progress, bestSolution);
                    progress.pop_back();
                    progress.pop_back();
                    slice();
                }
                move(-uMove, -dMove);

                uMove = incrementMove(uMove);
            }
            while (uMove != 0);
            dMove = incrementMove(dMove);
        }
        while (dMove != 0);
    }

public:
    // constructor
    SquareOne(const std::string inputU = "", const std::string inputD = "")
    {
        stateU = CUBE_U;
        stateD = CUBE_D;
        if ((inputU.length() != 12) || (inputD.length() != 12))
        {
            std::cout << "WARNING: no or invalid input, constructing solved SquareOne\n";
        }
        else
        {

            for (int i = 0; i < 12; i++)
            {
                stateU[i] = charToPiece(inputU[i]);
                stateD[i] = charToPiece(inputD[i]);
            }
        }
    }

    bool move(int uMove, int dMove)
    {
        if ((uMove < -6) || (uMove > 6) || (dMove < -6) || (dMove > 6))
        {
            std::cout << "WARNING: bad move not executed: (" << uMove << "," << dMove << ")\n";
            return false;
        }
        shift(stateU, -uMove);
        shift(stateD, -dMove);
        return true;
    }

    bool canSlice()
    {
        return (stateU[0] != Piece::C2) && (stateU[6] != Piece::C2)
            && (stateD[0] != Piece::C2) && (stateD[6] != Piece::C2);
    }

    bool slice()
    {
        if (!canSlice())
        {
            return false;
        }
        Piece temp;
        for (int i = 0; i < 6; i++)
        {
            temp = stateU[i + 6];
            stateU[i + 6] = stateD[i];
            stateD[i] = temp;
        }
        return true;
    }

    inline bool isSolved()
    {
        return layerSolved(stateU) && layerSolved(stateD);
    }

    void solve()
    {
        std::vector<int> progress;
        progress.reserve(14);
        std::vector<int> bestSolution(15);
        solver(progress, bestSolution);
    }

    void print()
    {
        std::cout << "SquareOne\n  U:";
        printLayer(stateU);
        std::cout << "  D:";
        printLayer(stateD);
    }

    static void printAlg(std::vector<int> alg, bool useParens = false)
    // currently, algorithms are represented by even-length
    // vector<int>'s of the format (u,d)/
    // TODO: use a different structure?
    {
        if ((alg.size() % 2) != 0)
        {
            std::cout << "WARNING: alg size is not even" << '\n';
            return;
        }
        for (int i = 0; i < alg.size(); i += 2)
        {
            if (!useParens)
            {
                std::cout << alg.at(i) << "," << alg.at(i + 1) << "/";
            }
            else
            {
                std::cout << "(" << alg.at(i) << "," << alg.at(i + 1) << ") / ";
            }
        }
        std::cout << std::endl;
    }

};


void runSolverCLI(int argc, const char* argv[])
{
    if (argc < 3)
    {
        std::cout << "ERROR: not enough arguments" << std::endl;
        return;
    }
    SquareOne sq1(argv[1], argv[2]);
    std::cout << "Solving the following state:" << "\n";
    sq1.print();
    std::cout << "Beginning cubeshape search..." << std::endl;
    sq1.solve();
    std::cout << "Done search." << std::endl;
}


int main(int argc, const char* argv[])
{
    runSolverCLI(argc, argv); // comment out this line to write your own code
    return 0;
}

