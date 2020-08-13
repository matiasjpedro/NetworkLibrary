#include "GridGraph.h"
#if GRID_LOGS
#include <iostream>
#endif

void UGridNode::RemoveObject(std::shared_ptr<UObject> InObj)
{
	auto& FoundObj = DynamicActorsMap.find(InObj);

	const bool bWasFound = FoundObj != DynamicActorsMap.end();

	if (bWasFound)
	{
		GetCellNodesForObj(FoundObj->second, GatheredNodes);
		for (auto& Node : GatheredNodes)
		{
			Node->RemoveObject(FoundObj->first);
		}
	}
}

void UGridNode::UpdateGrid()
{
	for (auto& Obj : DynamicActorsMap)
	{
		const Vector2 Location = Obj.first->GetObjectLocation();
		FObjCellInfo& PreviousCellInfo = Obj.second;

		FObjCellInfo NewCellInfo = GetCellInfoForObj(Location);

		// First Time - Just add
		if (!PreviousCellInfo.IsValid())
		{
			GetCellNodesForObj(NewCellInfo, GatheredNodes);

			for (auto& Node : GatheredNodes)
			{
				Node->AddObject(Obj.first);
			}

			PreviousCellInfo = NewCellInfo;
		}
		else
		{
			const bool NoLongerIntersecting = NewCellInfo.StartX > PreviousCellInfo.EndX
				|| NewCellInfo.EndX < PreviousCellInfo.StartX
				|| NewCellInfo.StartY > PreviousCellInfo.EndY
				|| NewCellInfo.EndY < PreviousCellInfo.StartY;
			
			// If it's no longer intersecting with the previous nodes, just remove it from all and add it to the new ones.
			if (NoLongerIntersecting)
			{
				GetCellNodesForObj(PreviousCellInfo, GatheredNodes);
				for (auto& Node : GatheredNodes)
				{
					Node->RemoveObject(Obj.first);
				}

				GetCellNodesForObj(NewCellInfo, GatheredNodes);
				for (auto& Node : GatheredNodes)
				{
					Node->AddObject(Obj.first);
				}
			}
			// There is some intersecting going on here, so we need to find out what cells need to be added or removed.
			else
			{
				if (PreviousCellInfo.StartX < NewCellInfo.StartX)
				{
					// We lost rows on the left side
					for (int32_t Y = PreviousCellInfo.StartY; Y <= PreviousCellInfo.EndY; Y++)
					{
						std::vector<UCellNode>& GridY = Grid[Y];

						for (int32_t X = PreviousCellInfo.StartX; X < NewCellInfo.StartX; X++)
						{
							GridY[X].RemoveObject(Obj.first);
						}
					}
				}
				else if (PreviousCellInfo.StartX > NewCellInfo.StartX)
				{
					// We added rows on the left side
					for (int32_t Y = NewCellInfo.StartY; Y <= NewCellInfo.EndY; Y++)
					{
						std::vector<UCellNode>& GridY = Grid[Y];

						for (int32_t X = NewCellInfo.StartX; X < PreviousCellInfo.StartX ; X++)
						{
							GridY[X].AddObject(Obj.first);
						}
					}
				}

				if (PreviousCellInfo.EndX < NewCellInfo.EndX)
				{
					// We added rows on the right side
					for (int32_t Y = NewCellInfo.StartY; Y <= NewCellInfo.EndY; Y++)
					{
						std::vector<UCellNode>& GridY = Grid[Y];

						for (int32_t X = PreviousCellInfo.EndX + 1; X <= NewCellInfo.EndX; X++)
						{
							GridY[X].AddObject(Obj.first);
						}
					}
				}
				else if (PreviousCellInfo.EndX > NewCellInfo.EndX)
				{
					// We lost rows on the right side
					for (int32_t Y = PreviousCellInfo.StartY; Y <= PreviousCellInfo.EndY; Y++)
					{
						std::vector<UCellNode>& GridY = Grid[Y];

						for (int32_t X = NewCellInfo.EndX + 1; X <= PreviousCellInfo.EndX; X++)
						{
							GridY[X].RemoveObject(Obj.first);
						}
					}
				}

				// As we already handled the columns of the left/right differences now we only need to handle the ones not covered by that
				const int32_t StartX = std::max<int32_t>(NewCellInfo.StartX, PreviousCellInfo.StartX);
				const int32_t EndX = std::min<int32_t>(NewCellInfo.EndX, PreviousCellInfo.EndX);

				if (PreviousCellInfo.StartY < NewCellInfo.StartY)
				{
					// We lost columns on the top side
					for (int32_t Y = PreviousCellInfo.StartY; Y < NewCellInfo.StartY; ++Y)
					{
						std::vector<UCellNode>& GridY = Grid[Y];

						for (int32_t X = StartX; X <= EndX; ++X)
						{
							GridY[X].RemoveObject(Obj.first);
						}
					}

				}
				else if (PreviousCellInfo.StartY > NewCellInfo.StartY)
				{
					// We added columns on the top side
					for (int32_t Y = NewCellInfo.StartY; Y < PreviousCellInfo.StartY; ++Y)
					{
						std::vector<UCellNode>& GridY = Grid[Y];

						for (int32_t X = StartX; X <= EndX; ++X)
						{
							GridY[X].AddObject(Obj.first);
						}
					}
				}

				if (PreviousCellInfo.EndY < NewCellInfo.EndY)
				{
					// We added columns on the bottom side
					for (int32_t Y = PreviousCellInfo.EndY + 1; Y <= NewCellInfo.EndY; ++Y)
					{
						std::vector<UCellNode>& GridY = Grid[Y];

						for (int32_t X = StartX; X <= EndX; ++X)
						{
							GridY[X].AddObject(Obj.first);
						}
					}
				}
				else if (PreviousCellInfo.EndY > NewCellInfo.EndY)
				{
					// We lost columns on the bottom side
					for (int32_t Y = NewCellInfo.EndY + 1; Y <= PreviousCellInfo.EndY; ++Y)
					{
						std::vector<UCellNode>& GridY = Grid[Y];

						for (int32_t X = StartX; X <= EndX; ++X)
						{
							GridY[X].RemoveObject(Obj.first);
						}
					}
				}
			}

			PreviousCellInfo = NewCellInfo;
		}
	}
}

void UGridNode::GetCellNodesForObj(const FObjCellInfo& CellInfo, std::vector<UCellNode*>& OutNodes)
{
	if (!CellInfo.IsValid())
	{
		//Log
		return;
	}

	//TODO: How to clear but keep the capacity to avoid reallocations?
	OutNodes.clear();

	const int32_t StartX = CellInfo.StartX;
	const int32_t StartY = CellInfo.StartY;
	const int32_t EndX = CellInfo.EndX;
	const int32_t EndY = CellInfo.EndY;

	//TODO: Here I could make it grow if the size of the grid was not implicit at construction.
	for (int32_t Y = StartY; Y <= EndY; Y++)
	{
		std::vector<UCellNode>& GridY = Grid[Y];

		for (int32_t X = StartX; X <= EndX; X++)
		{
			UCellNode* NodePtr = &GridY[X];
			OutNodes.push_back(NodePtr);
		}
	}
}

FObjCellInfo UGridNode::GetCellInfoForObj(const Vector2& Location)
{
	FObjCellInfo CellInfo;

	Vector2 LocationBias = Vector2(Location.mX - SpatialBias.mX, Location.mY - SpatialBias.mY);

	//TODO: For now just hardcoded
	float ObjBounds = 10;

	const float MinX = LocationBias.mX - ObjBounds;
	const float MinY = LocationBias.mY - ObjBounds;
	float MaxX = LocationBias.mX + ObjBounds;
	float MaxY = LocationBias.mY + ObjBounds;

	// If is out of bounds clamp it to the limit
	if (MaxX > BoundsX || MaxY > BoundsY)
	{
#if GRID_LOGS
		std::cout << " Out of bounds: " << Location.mY << " " << Location.mX << '\n';
#endif

		MaxX = std::min(MaxX, (float)BoundsX - 1.f);
		MaxY = std::min(MaxY, (float)BoundsY - 1.f);
	}

	CellInfo.StartX = std::max<int32_t>(0, (int32_t)(MinX / CellSize));
	CellInfo.StartY = std::max<int32_t>(0, (int32_t)(MinY / CellSize));

	CellInfo.EndX = std::max<int32_t>(0 , (int32_t)(MaxX / CellSize));
	CellInfo.EndY = std::max<int32_t>(0, (int32_t)(MaxY / CellSize));

	return CellInfo;
}

#if GRID_LOGS
void UGridNode::DrawGridObjs() const
{
	std::cout << "GridObjs: " << '\n';

	for (const auto& GridColumn : Grid)
	{
		for (const auto& GridRowNode : GridColumn)
		{
			std::cout << GridRowNode.GetObjects().size();
		}

		std::cout << '\n';
	}
}
#endif

