// #include "BaseModel.h"
// #include "../util/Container.h"
// #include "../util/GameData.h"

// namespace BoF {
// class Country: public BaseModel {
//     public:
//         std::string countryID;
//         std::string name;
//         bool is_ai;
//         int capitalPlanetID;

//         static std::shared_ptr<Country> get_by_id(std::string id) {
//             auto c = get_game_dict()->get<std::shared_ptr<Dict>>("countries")->get<std::shared_ptr<Dict>>(id);
//             std::shared_ptr<Country> country = std::make_shared<Country>();
//             Array keys = c->keys();
//             country->countryID = id;
//             country->name = c->get<std::string>("name");
//             country->is_ai = c->get<bool>("is_ai");
//             country->capitalPlanetID = c->get<int>("capitalPlanetID");

//             return country;
//         }
//         void update() {
//             auto c = get_game_dict()->get<std::shared_ptr<Dict>>("countries")->get<std::shared_ptr<Dict>>(this->countryID);
//             c->set("name", name);
//             c->set("is_ai", is_ai);
//             c->set("capitalPlanetID", capitalPlanetID);
//         }
// };
// }