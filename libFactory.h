#ifndef LIBFACTORY_LIBRARY_H
#define LIBFACTORY_LIBRARY_H

#include <map>
#include <memory>
#include <iostream>

/**
 * @brief namespace for library classes and functions
 */
namespace libFactory{

    /**
     * @brief the base class of all factories
     *
     * @details
     * the base class of all factories, if you create your own factory class then you need to inherit this either directly or indirectly
     * this class does not have a function to create objects, you either need to use one of the predefined factory types (see libFactory::BasicFactory) or create your own factory and implement a create function
     *
     * @tparam baseClass the base class of all the constructed objects
     * @tparam creatorFunctionType the type definition of the creator functions that get registered
     * @tparam identifierType the type of the identifier used to identify what object to create, uses baseClass::__factoryIdentifierType__ if not defined
     */
    template<class baseClass, typename creatorFunctionType, class identifierType = typename baseClass::__factoryIdentifierType__>
    class FactoryBase{
    public:
        /**
         * @brief function to register a creator function to create an object
         * @param id the id used to identify the object
         * @param funcCreate the function used to create the object
         * @return true if the function was registered
         * @return false if the id already exists in the registry
         */
        static bool registerCreatorFunc(const identifierType& id, const creatorFunctionType& funcCreate){
            auto& reg = getTypeRegistry();
            if (auto it = reg.find(id); it == reg.end())
            { // C++17 init-if ^^
                reg[id] = funcCreate;
                return true;
            }
            return false;
        }

        /**
         * @brief function to remove a creator function from the registry
         * @param id id of the function to remove
         * @return true if the function was removed
         * @return false if the id does not exist in the registry
         */
        static bool removeCreatorFunc(const identifierType& id){
            auto& reg = getTypeRegistry();
            if(auto it = reg.find(id); it != reg.end()){
                reg.erase(id);
                return true;
            }
            return false;
        }

        /**
         * @brief function to clear the registry
         */
        static void clearRegistry(){
            auto& reg = getTypeRegistry();
            reg.clear();
        }
    protected:
        /**
         * @brief the registry map typedef
         */
        typedef std::map<identifierType, creatorFunctionType> registryMapType;

        /**
         * @brief function to get the internal type registry
         * @return reference to the type registry
         */
        static registryMapType& getTypeRegistry(){
            static registryMapType typeRegistry;
            return typeRegistry;
        }
    };

    /**
     * @brief helper class to register an object with a factory, the intended use of this class is to inherit it in your class publicly and then it will register the class at static init time
     * @note the object to be registered has to implement a static getFactoryID() method that returns the factory ID of the object and implement the static create function that will be used as the object creator function
     * @warning sometimes the compiler will automatically not compile this class and therefore not register the object, to aviod this add `assert(libFactory_bRegistered) at the top of the object constructor`
     * @tparam factory the factory to register the object with
     * @tparam object the object to be registered, read note section for requirements
     * @see GoogleTests/autoRegistrationTests.cpp
     */
    template<class factory, class object>
    class autoRegister{
    public:

    protected:
        static bool libFactory_bRegistered;
    private:
        static bool _registerWithFactory_(){
            return factory::registerCreatorFunc(object::getFactoryID(), object::create);
        }
    };
    template<class factory, class object>
    bool autoRegister<factory, object>::libFactory_bRegistered = autoRegister<factory, object>::_registerWithFactory_();

    /**
     * @brief a basic factory that uses 0 argument creator functions
     * @tparam baseClass the base class of all created objects
     * @tparam identifierType the type of the identifier used to identify what object to create, uses baseClass::__factoryIdentifierType__ if not defined
     * @tparam functorReturn the return type of the creator functions
     *
     * @see GoogleTests/basicFactoryTests.cpp
     */
    template<class baseClass, class identifierType = typename baseClass::__factoryIdentifierType__, typename functorReturn = std::unique_ptr<baseClass>>
    class BasicFactory: public FactoryBase<baseClass, functorReturn (*)(void), identifierType>{
    public:
        /**
         * @brief function to create a object from its id
         * @param id the id of the object
         * @return the return value of the creator function
         * @return nullptr if the id does not exist in the registry
         */
        static functorReturn create(identifierType id){
            auto& reg =  baseFactory::getTypeRegistry();
            if (auto it = reg.find(id); it != reg.end()){
                return it->second(); // call the createFunc
            }
            return nullptr;
        }

        /**
         * @brief helper function to register an object with the factory
         * @warning the object that is being registered has to implement the getFactoryID() function to use this function, if that is not implemented then use registerObject(identifierType id)
         * @tparam object the object to register
         * @return true if the function was registered
         * @return false if the id already exists in the registry
         */
        template<class object>
        static bool registerObject(){
            return baseFactory::registerCreatorFunc(object::getFactoryID(), [](){return std::unique_ptr<baseClass>(new object()); });
        }

        /**
         * @brief helper function to register an object with the factory
         * @param id the id to register the object under
         * @tparam object the object to register
         * @return true if the function was registered
         * @return false if the id already exists in the registry
         */
        template<class object>
        static bool registerObject(identifierType id){
            return baseFactory::registerCreatorFunc(id, [](){return std::unique_ptr<baseClass>(new object()); });
        }
    protected:
        /**
         * @brief typedef for the underlying factory base class
         */
        typedef FactoryBase<baseClass, functorReturn (*)(void), identifierType> baseFactory;
    };

    /**
     * @brief a basic factory that uses 1 argument creator functions
     * @tparam baseClass the base class of all created objects
     * @tparam arg1Type the argument type of the first argument passed to the creator functions
     * @tparam identifierType the type of the identifier used to identify what object to create, uses baseClass::__factoryIdentifierType__ if not defined
     * @tparam functorReturn the return type of the creator functions
     *
     * @see GoogleTests/singleArgumentFactoryTests.cpp
     */
    template<class baseClass, class arg1Type, class identifierType = typename baseClass::__factoryIdentifierType__, typename functorReturn = std::unique_ptr<baseClass>>
    class SingleArgumentFactory: public FactoryBase<baseClass, functorReturn (*)(arg1Type), identifierType>{
    public:
        /**
         * @brief function to create a object from its id
         * @param id the id of the object
         * @return the return value of the creator function
         * @return nullptr if the id does not exist in the registry
         */
        static functorReturn create(identifierType id, arg1Type arg1){
            auto& reg =  baseFactory::getTypeRegistry();
            if (auto it = reg.find(id); it != reg.end()){
                return it->second(arg1); // call the createFunc
            }
            return nullptr;
        }

        /**
         * @brief helper function to register an object with the factory and pass the create() arguments to the constructor
         * @note the object that is being registered has to implement the getFactoryID() function to use this function, if that is not implemented then use registerObject(identifierType id)
         * @tparam object the object to register
         * @return true if the function was registered
         * @return false if the id already exists in the registry
         */
        template<class object>
        static bool registerObjectConstructor(){
            return baseFactory::registerCreatorFunc(object::getFactoryID(), [](arg1Type arg1){return std::unique_ptr<baseClass>(new object(arg1)); });
        }

        /**
         * @brief helper function to register an object with the factory and pass the create() arguments to the constructor
         * @param id the id to register the object under
         * @tparam object the object to register
         * @return true if the function was registered
         * @return false if the id already exists in the registry
         */
        template<class object>
        static bool registerObjectConstructor(identifierType id){
            return baseFactory::registerCreatorFunc(id, [](arg1Type arg1){return std::unique_ptr<baseClass>(new object(arg1)); });
        }
    protected:
        /**
         * @brief typedef for the underlying factory base class
         */
        typedef FactoryBase<baseClass, functorReturn (*)(arg1Type), identifierType> baseFactory;
    };
}

#endif //LIBFACTORY_LIBRARY_H
