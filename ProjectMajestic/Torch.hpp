#pragma once

#include "Main.hpp"
#include "Block.hpp"

class Torch :public Block {
public:

	Torch() { datasets["light_id"].setType(Data::Uuid); }
	~Torch();

	const string getBlockId() override { return "torch"; }
	bool isSolid() override { return false; }

private:

	virtual void _updateLogic() override;
	virtual void _onPlaced(Entity* placer) override;
	virtual void _onDestroy(Entity* destroyer) override;

	Uuid& lightId() { return datasets["light_id"].getDataUuid(); };

};
