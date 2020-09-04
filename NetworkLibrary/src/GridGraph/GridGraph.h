#pragma once
#include "GridGraphTypes.h"
#include <iostream>

//TODO:

#define GRID_LOGS 1

class UNode
{
public:

	virtual void AddObject(std::shared_ptr<UGridObject> InObj) {};
	virtual void RemoveObject(std::shared_ptr<UGridObject> InObj) {};

protected:

	std::vector<UNode> ChildNodes;
};

class UCellNode : UNode
{
	uint32_t CellX, CellY;

	std::vector<std::shared_ptr<UGridObject>> Objects;

public:

	UCellNode(uint32_t InCellX, uint32_t InCellY) :
		CellX(InCellX), CellY(InCellY)
	{

	}

	virtual void AddObject(std::shared_ptr<UGridObject> InObj) override
	{
		Objects.push_back(InObj);
	}

	virtual void RemoveObject(std::shared_ptr<UGridObject> InObj) override
	{
		Objects.erase(std::remove(Objects.begin(), Objects.end(), InObj));

	}

	inline uint32_t GetCellX() const { return CellX; }
	inline uint32_t GetCellY() const { return CellY; }

	inline const std::vector<std::shared_ptr<UGridObject>>& GetObjects() const { return Objects; }

};

class UGridNode : public UNode
{
	uint32_t CellSize;
	uint32_t BoundsX, BoundsY;
	Vector2 SpatialBias;

	std::vector<std::vector<UCellNode>> Grid;
	std::unordered_map<std::shared_ptr<UGridObject>, FObjCellInfo> DynamicActorsMap;

	std::vector<UCellNode*> GatheredNodes;

public:

	virtual void AddObject(std::shared_ptr<UGridObject> InObj) override
	{
		DynamicActorsMap.emplace(InObj, FObjCellInfo());
	}

	virtual void RemoveObject(std::shared_ptr<UGridObject> InObj) override;

	UGridNode(uint32_t InCellSize, Vector2 InSpatialBias, uint32_t InBoundsX, uint32_t InBoundsY) :
		CellSize(InCellSize), SpatialBias(InSpatialBias), BoundsX(InBoundsX), BoundsY(InBoundsY)
	{
		const uint32_t AmountRows = BoundsX / CellSize;
		const uint32_t AmountColumns = BoundsY / CellSize;

		Grid.reserve(AmountColumns);

		for (uint32_t i = 0; i < AmountColumns; i++)
		{
			std::vector <UCellNode> TempRow;
			TempRow.reserve(AmountRows);

			for (uint32_t j = 0; j < AmountRows; j++)
			{
				TempRow.push_back(UCellNode(j, i));
			}

			Grid.push_back(std::move(TempRow));
		}
	}

	void UpdateGrid();

	void GetCellNodesForObj(const FObjCellInfo& CellInfo, std::vector<UCellNode*>& OutNodes);
	FObjCellInfo GetCellInfoForObj(const Vector2& Location);

#if GRID_LOGS
	void DrawGridObjs() const;
#endif

};