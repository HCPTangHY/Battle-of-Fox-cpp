#include "BaseModel.h"
#include "../util/Container.h"

class Planet : public BaseModel{

    public:
        int pID;
        int size;
        int ownerID;
        int controllerID;
        int planetType;
        Array buildings;
        Dict inputResources;
        Dict outputResources;
        Array transportRoutes;
};