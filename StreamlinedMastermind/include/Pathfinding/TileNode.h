#pragma once

namespace pf
{
    class TileNode
    {
    public:

        enum class EState { Source, Target, Unvisited, Neighbour, Visited, Path, Wall, Current };

        TileNode() = delete; 
        TileNode(unsigned int a_index);
        ~TileNode() = default;

        EState GetState() const;
        std::vector<int> GetAdjecentTileIndexes() const;
        unsigned int GetPreviousTileIndex() const;
        unsigned int GetIndex() const;
        unsigned int GetCost() const;
        unsigned int GetPass() const;

        void SetState(EState a_state);
        void SetAdjecentTileIndexes(unsigned int a_number);
        void SetPreviousTileIndex(unsigned int a_number);
        void SetIndex(unsigned int a_index);
        void SetCost(unsigned int a_cost);
        void SetPass(unsigned int a_pass);

    private:
        unsigned int m_previousTileIndex;
        unsigned int m_index;
        unsigned int m_cost;
        unsigned int m_pass;

        EState m_state;

        std::vector<int> m_adjecentTileIndexes;
    };
}

