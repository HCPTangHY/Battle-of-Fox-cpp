class BaseModel {
protected:
    DataService* data_service;
    
public:
    BaseModel(DataService* service) : data_service(service) {}
    virtual ~BaseModel() = default;
    
    virtual void load() = 0;
    virtual void save() = 0;
};
