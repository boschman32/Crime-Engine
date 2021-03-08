#pragma once

namespace pf
{
    class Heuristics
    {
    public:
        Heuristics() = default;
        Heuristics(unsigned int a_totalRows, unsigned int a_totalColumns);
        ~Heuristics() = default;

        float Manhattan(unsigned int a_tileIndexA, unsigned int a_tileIndexB) const;
        float Manhattan(glm::vec2 a_tilePosA, glm::vec2 a_tilePosB) const;
        //float Diagonal(unsigned int a_tileIndexA, unsigned int a_tileIndexB);
        //float Diagonal(Vector2f a_tilePosA, Vector2f a_tilePosB);
        //float Euclidean(unsigned int a_tileIndexA, unsigned int a_tileIndexB) const;
        //float Euclidean(Vector2f a_tilePosA, Vector2f a_tilePosB) const;

    private:
        unsigned int m_totalRows;
        unsigned int m_totalColumns;
    };
}
