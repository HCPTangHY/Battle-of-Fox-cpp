#include "BaseModel.h"
#include "../util/Container.h"

class Star : public BaseModel {
    public:
        std::string starID;
        std::string name;
        double galaxyX;
        double galaxyY;
        Array planets;
        Array systemObjects;

        static std::shared_ptr<Star> get_by_id(std::string id) {
            auto c = get_game_dict()->get<std::shared_ptr<Dict>>("stars")->get<std::shared_ptr<Dict>>(id);
            std::shared_ptr<Star> r = std::make_shared<Star>();
            Array keys = c->keys();
            r->starID = id;
            r->name = c->get<std::string>("name");
            r->galaxyX = c->get<double>("galaxyX");
            r->galaxyY = c->get<double>("galaxyY");
            r->planets = c->get<Array>("planets");
            r->systemObjects = c->get<Array>("systemObjects");

            return r;
        }
};