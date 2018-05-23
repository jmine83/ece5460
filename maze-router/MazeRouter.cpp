/*
 * MazeRouter.cpp
 * Work on this file to complete your maze router
 */

using namespace std;

#include "SimpleGR.h"

///////////////////////////////////////////////////////////////////////////////
// Implement an A* search based maze routing algorithm
// and a corresponding back-trace procedure
// The function needs to correctly deal with the following conditions:
// 1. Only search within a bounding box defined by botleft and topright points
// 2. Control if any overflow on the path is allowed or not
///////////////////////////////////////////////////////////////////////////////
CostType SimpleGR::routeMaze(Net& net, bool allowOverflow, const Point &botleft,
                             const Point &topright, const EdgeCost &func, vector<Edge*> &path)
{
  bool debug = false; // turn debug mode on or off
  if (debug)
  {
    cout << "\n" << "########################################" << "\n";
    cout << "GCellData Initial" << "\n";
    priorityQueue.showGCellData();
  }

  // find out the ID of the source and sink gcells
  const IdType srcGCellId = getGCellId(net.gCellOne);
  const IdType snkGCellId = getGCellId(net.gCellTwo);

  // insert the source gcell into the priority queue
  priorityQueue.setGCellCost(srcGCellId, 0., 0., NULLID);

  if (debug)
  {
    cout << "GCellData Initial -> Source GCell Added" << "\n";
    priorityQueue.showGCellData();
  }
  
  // Instantiate the Cost function to calculate the Manhattan distance between
  // two gcells. This distance is used as the heuristic cost for A* search.
  ManhattanCost &lb = ManhattanCost::getFunc();

  unsigned i;
  if (debug)
  {
    cout << "\n";
    cout << "INPUT Start" << "\n";
    cout << "net.numSegments = " << net.numSegments << "\n";
    cout << "net.numVias = " << net.numVias << "\n";
    cout << "net.gCellOne = " << net.gCellOne.x << " , " << net.gCellOne.y << " , " << net.gCellOne.z << "\n";
    cout << "net.gCellTwo = " << net.gCellTwo.x << " , " << net.gCellTwo.y << " , " << net.gCellTwo.z << "\n";
    cout << "net.id = " << net.id << "\n";
    cout << "net.routed = ";
    if (net.routed)
      cout << "true" << "\n";
    else
      cout << "false" << "\n";
    cout << "net.segments[] = ";
    if (net.segments.size() != 0)
    {
      for (i = 0; i < net.segments.size(); i++)
      {
        cout << net.segments[i];
        if (i != net.segments.size()-1)
          cout << " , ";
        else
          cout << "\n";
      }
    }
    else
      cout << "EMPTY" << "\n";
    cout << "allowOverflow = ";
    if (allowOverflow)
      cout << "true" << "\n";
    else
      cout << "false" << "\n";
    cout << "botleft = " << botleft.x << " , " << botleft.y << " , " << botleft.z << "\n";
    cout << "topright = " << topright.x << " , " << topright.y << " , " << topright.z << "\n";
    cout << "func = pathCost_type: DLMCost or UnitCost" << "\n";
    cout << "path[] = ";
    if (path.size() != 0)
      cout << "NOT EMPTY" << "\n";
    else
      cout << "EMPTY" << "\n";
    for (int z = 0; z < 2; z++)
    {
      for (int y = 0; y < 3; y++)
      {
        for (int x = 0; x < 3; x++)
        {
          cout << "gcellArr3D[" << z << "][" << y << "][" << x << "] = " << "\n";
          cout << "  posX = " << gcellArr3D[z][y][x].incX << "\n";
          cout << "  negX = " << gcellArr3D[z][y][x].decX << "\n";
          cout << "  posY = " << gcellArr3D[z][y][x].incY << "\n";
          cout << "  negY = " << gcellArr3D[z][y][x].decY << "\n";
          cout << "  posZ = " << gcellArr3D[z][y][x].incZ << "\n";
          cout << "  negZ = " << gcellArr3D[z][y][x].decZ << "\n";
        }
      }
    }
    cout << "edgeCosts[] = " << "\n";
    for (i = 0; i < grEdgeArr.size(); i++)
      cout << i << ": " << func(i) << "\n";
    cout << "srcGCellId = " << srcGCellId << "\n";
    cout << "snkGCellId = " << snkGCellId << "\n";
    cout << "INPUT Finish" << "\n";
    cout << "\n";
  }

  // A* search kernel
  // Loop until all "frontiers" in the priority queue are exhausted, or when
  // the sink gcell is found.
  bool pathFound = false;
  do {
    // YOUR A* search CODE GOES IN HERE
    // Source: http://www.growingwiththeweb.com/2012/06/a-pathfinding-algorithm.html
/*
    Instructions for detecting when a gcell is the open list, the closed list, or neither.

    priorityQueue.isGCellVsted(gcellId) == true <<< Added to open list, but could have moved to closed list.
    priorityQueue.isGCellVsted(gcellId) == false <<< Has not been added to either list yet.

    priorityQueue.getGCellData(selGCellId).heapLoc == NULLID << Removed from open list and added to closted list.
    priorityQueue.getGCellData(selGCellId).heapLoc != NULLID << Could be on the open list or no list.
*/
    if (debug)
    {
      cout << "A* Start" << "\n";
      cout << "Get Best GCell" << "\n";
    }
    // open list element with lowest totalCost
    IdType curGCellId = priorityQueue.getBestGCell();
    if (debug)
    {
      cout << "curGCellId = " << curGCellId << "\n";
      cout << "GCellData A* -> Get Best GCell" << "\n";
      priorityQueue.showGCellData();
    }
    if (curGCellId == snkGCellId)
    {
      pathFound = true;
      break;
    }
    else
    {
      if (debug)
      {
        cout << "Still Getting to the Sink" << "\n";
      }
      // add current gcell to closed list and remove from the open list
      priorityQueue.rmBestGCell();
      if (debug)
      {
        cout << "Removed Best GCell" << "\n";
        cout << "GCellData A* -> Removed Best GCell" << "\n";
        priorityQueue.showGCellData();
      }
      Point curGCellCoord = gcellIdtoCoord(curGCellId);
      if (debug)
      {
        cout << "curGCellCoord = " << curGCellCoord.x << "," << curGCellCoord.y << "," << curGCellCoord.z << "\n";
      }
      vector<Point> Gcells = getGCellPoints(curGCellCoord, botleft, topright);
      if (debug)
      {
        cout << "Gcells[] = " << "\n";
        for (i = 0; i < Gcells.size(); i++)
        {
          cout << "  " << Gcells[i].x << "," << Gcells[i].y << "," << Gcells[i].z << "\n";
        }
      }
      IdType selGCellId;
      vector<IdType> selGCellIds;
      // check the neighboring gcells from current gcell
      for (unsigned n = 0; n < Gcells.size(); ++n)
      {
        selGCellId = gcellCoordToId(Gcells[n].x, Gcells[n].y, Gcells[n].z);
        if (debug)
        {
          cout << "selGCellId = " << selGCellId << "\n";
        }
        selGCellIds.push_back(selGCellId);
        if (debug)
        {
          cout << "selGCellIds[] = ";
          for (i = 0; i < selGCellIds.size(); i++)
          {
            cout << selGCellIds[i] << " ";
          }
          cout << "\n";
        }
        if (priorityQueue.getGCellData(selGCellId).heapLoc != NULLID)
        {
          // selected neighbor gcell is not in closed list
          if (debug)
          {
            cout << "Selected GCell Not in Closed List" << "\n";
          }
          Point selGCellCoord = gcellIdtoCoord(selGCellId);
          if (debug)
          {
            cout << "selGCellCoord = " << selGCellCoord.x << "," << selGCellCoord.y << "," << selGCellCoord.z << "\n";
          }
          CostType pathCost;
          CostType totalCost;
          EdgeType edgeAdj = getGCellsAdj(curGCellCoord, selGCellCoord);
          IdType edgeId = getEdgeId(getGCell(curGCellId), getGCell(selGCellId), edgeAdj);
          if (edgeId != NULLID)
          {
            if (debug)
            {
              cout << "Found Connecting Edge!" << "\n";
            }
            Point snkGCellCoord = gcellIdtoCoord(snkGCellId);
            if (debug)
            {
              cout << "snkGCellCoord = " << snkGCellCoord.x << "," << snkGCellCoord.y << "," << snkGCellCoord.z << "\n";
            }
            pathCost = func(edgeId);
            if (debug)
            {
              cout << "pathCost = " << pathCost << "\n";
            }
            totalCost = pathCost + lb(selGCellCoord, snkGCellCoord);
            if (debug)
            {
              cout << "totalCost = " << totalCost << "\n";
            }
          }
          else
          {
            if (debug)
            {
              cout << "No Connecting Edge" << "\n";
            }
            pathCost = NULLCAP;
          }
          if (pathCost != NULLCAP)
          {
            if (debug)
            {
              cout << "Edge is Not Blocked" << "\n";
            }
            if (!priorityQueue.isGCellVsted(selGCellId))
            {
              // selected neighbor gcell is not in open list
              if (debug)
              {
                cout << "Selected GCell Not in Open List" << "\n";
              }
              Edge* selEdge = getEdge(edgeId);
              if (allowOverflow)
              {
                if (debug)
                {
                  cout << "Overflow is Allowed" << "\n";
                  cout << "Add to the open list." << "\n";
                }
                priorityQueue.setGCellCost(selGCellId, totalCost, pathCost, curGCellId);
                if (debug)
                {
                  cout << "GCellData A* -> Added GCell to the open list." << "\n";
                  priorityQueue.showGCellData();
                }
              }
              else
              {
                if (debug)
                {
                  cout << "Overflow is Not Allowed" << "\n";
                  cout << "Checking if edge is overflowing..." << "\n";
                  cout << "  Usage = " << selEdge->usage << "\n";
                  cout << "  Capacity = " << selEdge->capacity << "\n";
                }
                if (selEdge->usage - selEdge->capacity <= 0)
                {
                  if (debug)
                  {
                    cout << "Edge is Not Full... Add to the open list.";
                  }
                  priorityQueue.setGCellCost(selGCellId, totalCost, pathCost, curGCellId);
                  if (debug)
                  {
                    cout << "GCellData A* -> Added GCell to the open list." << "\n";
                    priorityQueue.showGCellData();
                  }
                }
                else
                {
                  if (debug)
                  {
                    cout << "Edge is Full... Do not add to the open list.";
                  }
                }
              }
            }
            else
            {
              // selected neighbor gcell is in open list
              if (debug)
              {
                cout << "Selected GCell is in Open List" << "\n";
              }
              for (unsigned u = 0; u < selGCellIds.size(); ++u)
              {
                // find a neighbor gcell on the open list
                if (debug)
                {
                  cout << "Find a Neighbor GCell on the Open List" << "\n";
                }
                if (priorityQueue.isGCellVsted(selGCellIds[u]) && priorityQueue.getGCellData(selGCellIds[u]).heapLoc != NULLID)
                {
                  // find and record the better cost
                  if (debug)
                  {
                    cout << "Find and Record the Better Cost" << "\n";
                  }
                  if (priorityQueue.getGCellData(selGCellId).pathCost < priorityQueue.getGCellData(selGCellIds[u]).pathCost)
                  {
                    if (debug)
                    {
                      cout << "Record and Set the Better Cost" << "\n";
                    }
                    CostType selGCell_pathCost = priorityQueue.getGCellData(selGCellId).pathCost;
                    IdType selGCell_parentGCell = priorityQueue.getGCellData(selGCellId).parentGCell;
                    CostType opeGCell_totalCost = priorityQueue.getGCellData(selGCellIds[u]).totalCost;
                    priorityQueue.setGCellCost(selGCellIds[u], opeGCell_totalCost, selGCell_pathCost, selGCell_parentGCell);
                  }
                }
              }
            }
          }
          else
          {
            if (debug)
            {
              cout << "Edge is Blocked" << "\n";
            }
          }
        }
      }
    }
    // YOUR A* search CODE ENDS HERE
  } while (!priorityQueue.isEmpty());

  if (debug)
  {
    if (pathFound)
    {
      cout << "Path to Sink Found!" << "\n";
    }
    else
    {
      cout << "Path to Sink Not Found" << "\n";
    }
    cout << "A* Finish" << "\n";
  }

  // now backtrace and build up the path, if we found one
  // back-track from sink to source, and fill up 'path' vector with all the edges that are traversed
  if (priorityQueue.isGCellVsted(snkGCellId)) {
    // YOUR backtrace CODE GOES IN HERE
    if (debug)
    {
      cout << "BackTrace Start" << "\n";
    }
    IdType homGCellId = snkGCellId;
    if (debug)
    {
      cout << "start_homGCellId = " << homGCellId << "\n";
    }
    IdType nexGCellId = priorityQueue.getGCellData(snkGCellId).parentGCell;
    if (debug)
    {
      cout << "start_nexGCellId = " << nexGCellId << "\n";
    }
    do {
      EdgeType edgeAdj = getGCellsAdj(gcellIdtoCoord(homGCellId), gcellIdtoCoord(nexGCellId));
      IdType edgeId = getEdgeId(getGCell(homGCellId), getGCell(nexGCellId), edgeAdj);
      if (debug)
      {
        cout << "edgeId = " << edgeId << "\n";
      }
      path.push_back(getEdge(edgeId));
      if (debug)
      {
        cout << "path[] = ";
        for (i = 0; i < path.size(); i++)
        {
          cout << path[i]->id << " ";
        }
        cout << "\n";
      }
      homGCellId = nexGCellId;
      if (debug)
      {
        cout << "finish_homGCellId = " << homGCellId << "\n";
      }
      nexGCellId = priorityQueue.getGCellData(homGCellId).parentGCell;
      if (debug)
      {
        cout << "finish_nexGCellId = " << nexGCellId << "\n";
      }
    } while (nexGCellId != NULLID);
    if (debug)
    {
      cout << "BackTrace Finish" << "\n";
      cout << "########################################" << "\n\n";
    }
    // YOUR backtrace CODE ENDS HERE
  }

  // calculate the accumulated cost of the path
  const CostType finalCost =
      priorityQueue.isGCellVsted(snkGCellId) ?
          priorityQueue.getGCellData(snkGCellId).pathCost : numeric_limits<CostType>::max();

  // clean up
  priorityQueue.clear();
  priorityQueue.reset();

  return finalCost;
}
