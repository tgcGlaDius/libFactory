//
// Created by elias on 25/05/2021.
//

#include "../libFactory.h"

#include <gtest/gtest.h>

#include <memory>

struct testBaseAutoregClass{
    typedef int __factoryIdentifierType__;
    virtual void incA(){a++;}
    static int getFactoryID(){return 0;}
    int a = 0;
};

typedef libFactory::BasicFactory<testBaseAutoregClass> testBasicAutoregFactory;

struct testDerivA: public testBaseAutoregClass, libFactory::autoRegister<testBasicAutoregFactory, testDerivA>{
    testDerivA() { assert(libFactory_bRegistered);}
    void incA() override {a+=2;}
    static __factoryIdentifierType__ getFactoryID(){return 1;}
    static std::unique_ptr<testBaseAutoregClass> create(){return std::make_unique<testDerivA>(); }
};

struct testDerivB: public testBaseAutoregClass, libFactory::autoRegister<testBasicAutoregFactory, testDerivB>{
    testDerivB() { assert(libFactory_bRegistered);}
    void incA() override {a+=3;}
    static __factoryIdentifierType__ getFactoryID(){return 2;}
    static std::unique_ptr<testBaseAutoregClass> create(){return std::make_unique<testDerivB>(); }
};

struct testBaseSingleArg{
    explicit testBaseSingleArg(int a) : a(a) {}
    typedef int __factoryIdentifierType__;
    virtual void incA(){a++;}
    static int getFactoryID(){return 0;}
    int a = 0;
};

typedef libFactory::SingleArgumentFactory<testBaseSingleArg, int> testSingleArgAutoregFactory;

struct testDerivSingleArgA: public testBaseSingleArg, libFactory::autoRegister<testSingleArgAutoregFactory, testDerivSingleArgA>{
    explicit testDerivSingleArgA(int a): testBaseSingleArg(a) { assert(libFactory_bRegistered);}
    void incA() override {a+=2;}
    static __factoryIdentifierType__ getFactoryID(){return 1;}
    static std::unique_ptr<testBaseSingleArg> create(int arg1){return std::make_unique<testDerivSingleArgA>(arg1); }
};

struct testDerivSingleArgB: public testBaseSingleArg, libFactory::autoRegister<testSingleArgAutoregFactory, testDerivSingleArgB>{
    explicit testDerivSingleArgB(int a): testBaseSingleArg(a) { assert(libFactory_bRegistered);}
    void incA() override {a+=3;}
    static __factoryIdentifierType__ getFactoryID(){return 2;}
    static std::unique_ptr<testBaseSingleArg> create(int arg1){return std::make_unique<testDerivSingleArgB>(arg1); }
};



TEST(autoRegistrationTests, registeredBasic){
    ASSERT_NE(testBasicAutoregFactory::create(testDerivA::getFactoryID()), nullptr);
    ASSERT_NE(testBasicAutoregFactory::create(testDerivB::getFactoryID()), nullptr);
    ASSERT_EQ(testBasicAutoregFactory::create(testBaseAutoregClass::getFactoryID()), nullptr);
}

TEST(autoRegistrationTests, createBasic){
    auto ret1 = testBasicAutoregFactory::create(testDerivA::getFactoryID());
    ASSERT_NE(ret1, nullptr);
    ret1->incA();
    ASSERT_EQ(ret1->a, 2);

    auto ret2 = testBasicAutoregFactory::create(testDerivB::getFactoryID());
    ASSERT_NE(ret2, nullptr);
    ret2->incA();
    ASSERT_EQ(ret2->a, 3);
}

TEST(autoRegistrationTests, registeredSingleArg){
    ASSERT_NE(testSingleArgAutoregFactory::create(testDerivA::getFactoryID(), 0), nullptr);
    ASSERT_NE(testSingleArgAutoregFactory::create(testDerivB::getFactoryID(), 0), nullptr);
    ASSERT_EQ(testSingleArgAutoregFactory::create(testBaseAutoregClass::getFactoryID(), 0), nullptr);
}

TEST(autoRegistrationTests, createSingleArg){
    auto ret1 = testSingleArgAutoregFactory::create(testDerivA::getFactoryID(), 2);
    ASSERT_NE(ret1, nullptr);
    ret1->incA();
    ASSERT_EQ(ret1->a, 4);

    auto ret2 = testSingleArgAutoregFactory::create(testDerivB::getFactoryID(), 3);
    ASSERT_NE(ret2, nullptr);
    ret2->incA();
    ASSERT_EQ(ret2->a, 6);
}
