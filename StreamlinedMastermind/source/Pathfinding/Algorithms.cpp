#include "smpch.h"
#include "Pathfinding/Algorithms.h"
#include "Core/Logger.h"
#include "Utils/Timer.h"

//#define show_info 

pf::Algorithms::Algorithms(const std::vector<std::unique_ptr<TileNode>>& a_tiles, const unsigned int a_totalColumns)
    : m_totalColumns(a_totalColumns)
    , m_tiles(a_tiles)
{
    m_heuristic = std::make_unique<Heuristics>(0, m_totalColumns);
}

glm::vec2 pf::Algorithms::AStar(ce::UVector2 a_source, ce::UVector2 a_target) const
{
    bool foundPath = false;
    bool foundSource = false;
    unsigned int pathNumber = GetTileIndex(a_target);
    unsigned int next = 0;
    unsigned int totalSearchOperations = 0;

    std::vector<std::pair<int, float>> searchTile;  // The tiles where the algorithm searches through.
    std::vector<int> pathTileNumber;    // The tiles where the path is created.

    ce::Timer<float> timer;

    for (const auto& tile : m_tiles)
    {
        if (tile->GetState() != TileNode::EState::Wall)
        {
            // Making sure that the states and passes are reset.
            tile->SetState(TileNode::EState::Unvisited);
            tile->SetPass(0);
        }
    }

    if (m_tiles[GetTileIndex(a_target)]->GetState() == TileNode::EState::Wall)
    {
#ifdef show_info
        CE_APP_INFO("Sir, you're clicking on a wall...");
#endif
        return a_source;
    }

    m_tiles[GetTileIndex(a_target)]->SetState(TileNode::EState::Target);
    m_tiles[GetTileIndex(a_source)]->SetState(TileNode::EState::Source);

    if (a_source == a_target)
    {
#ifdef show_info
        CE_APP_INFO("At Destination");
#endif

        return a_target;
    }

    const float sourceDistanceFromSource = m_heuristic->Manhattan(a_source, a_source);  // Dijkstra Cost
    const float sourceDistanceToTarget = m_heuristic->Manhattan(a_target, a_source);    // Greedy/Best-First search Cost
    const float sourceTotalDistance = sourceDistanceFromSource + sourceDistanceToTarget;    // A* Cost

    searchTile.emplace_back(GetTileIndex(a_source), float(sourceTotalDistance));    // First tile in the search is the source tile.

    // Begin of the search.
    while (!foundSource)
    {
        const int currentTile = searchTile[next].first;
        int currentPass = 0;

        //if (searchTile.size() - 1 != next)
        {
            next++;
        }

//        if (next != 0 && next != 1 && next == searchTile.size() - 1)
//        {
//#ifdef show_info
//            CE_APP_INFO("No path could be found!");
//#endif
//
//            return a_source;
//        }

        if (m_tiles[currentTile]->GetState() != TileNode::EState::Source && m_tiles[currentTile]->GetState() != TileNode::EState::Target)
        {
            m_tiles[currentTile]->SetState(TileNode::EState::Visited);
        }
        
        //  Searches through the adjacent tiles of the current tile the search is in and puts them in the search list.
        for (unsigned int i = 0; i < m_tiles[currentTile]->GetAdjecentTileIndexes().size(); i++)
        {
            const int adjecentTile = m_tiles[currentTile]->GetAdjecentTileIndexes()[i];

            if (m_tiles[adjecentTile]->GetState() == TileNode::EState::Target)
            {
                m_tiles[adjecentTile]->SetPreviousTileIndex(currentTile);
                totalSearchOperations++;
                foundSource = true;
            }
            else if (m_tiles[adjecentTile]->GetState() == TileNode::EState::Unvisited)
            {
                bool isCloser = false;
                const float distanceFromSource = m_heuristic->Manhattan(GetTileIndex(a_source), adjecentTile);  // Dijkstra Cost.
                const float distanceToTarget = m_heuristic->Manhattan(GetTileIndex(a_target), adjecentTile);    // Greedy/Best-First search Cost.
                const float totalDistance = distanceFromSource + distanceToTarget;  // A* Cost.

                // Checks if something should be put at a higher priority if the total cost is lower than others.
                for (unsigned int j = next; j < searchTile.size(); j++)
                {
                    if (totalDistance < searchTile[j].second)
                    {
                        isCloser = true;

                        searchTile.insert(searchTile.begin() + j + m_tiles[currentTile]->GetPass(), std::make_pair(int(adjecentTile), float(totalDistance)));

                        break;
                    }

                    if (totalDistance == searchTile[j].second)
                    {
                        currentPass++;
                    }
                }

                if (!isCloser)
                {
                    searchTile.emplace_back(int(adjecentTile), float(totalDistance));
                }

                //  Only Breadth-First search.
                //m_searchTileNumber.push_back(adjecentTile);

                m_tiles[adjecentTile]->SetPreviousTileIndex(currentTile);

                totalSearchOperations++;
            }

            if (m_tiles[currentTile]->GetState() == TileNode::EState::Source && m_tiles[adjecentTile]->GetState() == TileNode::EState::Target)
            {
                return a_target;
            }
        }

        m_tiles[currentTile]->SetPass(currentPass);

        for (const auto i : searchTile)
        {
            if (m_tiles[i.first]->GetState() == TileNode::EState::Unvisited)
            {
                m_tiles[i.first]->SetState(TileNode::EState::Neighbour);
            }
        }

        if (searchTile.size() - 1 == 0)
        {
#ifdef show_info
            CE_APP_INFO("Yeah, it's stuck!");
#endif

            return a_source;
        }

        if (next != 0 && next != 1 && next == searchTile.size())
        {
#ifdef show_info
            CE_APP_INFO("No path could be found!");
#endif

            return a_source;
        }

        
    }

    //  Begin of creating the path.
    while (!foundPath)
    {
        pathTileNumber.push_back(m_tiles[pathNumber]->GetPreviousTileIndex());
        pathNumber = m_tiles[pathNumber]->GetPreviousTileIndex();

        for (auto tile : pathTileNumber)
        {
            if (m_tiles[tile]->GetState() == TileNode::EState::Visited)
            {
                m_tiles[tile]->SetState(TileNode::EState::Path);
            }
        }

        if (m_tiles[pathTileNumber[pathTileNumber.size() - 1]]->GetState() == TileNode::EState::Source)
        {
            foundPath = true;
        }
    }

#ifdef show_info
    CE_APP_INFO("Path Length: {0}", pathTileNumber.size());
    CE_APP_INFO("Elapsed Time: {0}", timer.GetElapsedTime());
    CE_APP_INFO("Search Operations: {0}", totalSearchOperations);
#endif

    if (pathTileNumber.size() > 1)
    {
        return GetGridPos(m_tiles[pathTileNumber[pathTileNumber.size() - 2]]->GetIndex());
    }

    return GetGridPos(m_tiles[pathTileNumber[pathTileNumber.size() - 1]]->GetIndex());
}

unsigned pf::Algorithms::GetTileIndex(const ce::UVector2 a_gridPos) const
{
    return a_gridPos.x * m_totalColumns + a_gridPos.y;
}

ce::UVector2 pf::Algorithms::GetGridPos(const unsigned a_index) const
{
    const unsigned int posX = a_index / m_totalColumns;
    const unsigned int posY = a_index % m_totalColumns;

    return { float(posX), float(posY) };
}