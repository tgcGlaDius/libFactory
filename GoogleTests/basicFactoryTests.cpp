//
// Created by elias on 24/05/2021.
//
#include "../libFactory.h"

#include <gtest/gtest.h>

#include <memory>

struct testBaseClass{
    typedef int __factoryIdentifierType__;
    virtual void incA(){a++;}
    static int getFactoryID(){return 0;}
    int a = 0;
};

typedef libFactory::BasicFactory<testBaseClass> testFactory;

struct testDerivA: public testBaseClass{
    void incA() override {a+=2;}
    static __factoryIdentifierType__ getFactoryID(){return 1;}
};

struct testDerivB: public testBaseClass{
    void incA() override {a+=3;}
    static __factoryIdentifierType__ getFactoryID(){return 2;}
};

class basicFactoryTests: public ::testing::Test {
public:

    basicFactoryTests() = default;

protected:
    void SetUp() override {
        auto ret = testFactory::registerObject<testBaseClass>();
        ASSERT_TRUE(ret);
        ret = testFactory::registerObject<testDerivA>();
        ASSERT_TRUE(ret);
        ret = testFactory::registerObject<testDerivB>();
        ASSERT_TRUE(ret);
    }
protected:
    void TearDown() override {
        testFactory::clearRegistry();
    }
    ~basicFactoryTests() override = default;
};

TEST_F(basicFactoryTests, clearRegistrations){
    testFactory::clearRegistry();
    ASSERT_EQ(testFactory::create(0), nullptr);
}

TEST_F(basicFactoryTests, singleManualIDRegistration){
    testFactory::clearRegistry();

    auto ret = testFactory::registerObject<testBaseClass>(0);
    ASSERT_TRUE(ret);
    ASSERT_NE(testFactory::create(0), nullptr);
}

TEST_F(basicFactoryTests, singleAutoIDRegistration){
    testFactory::clearRegistry();

    auto ret = testFactory::registerObject<testBaseClass>();
    ASSERT_TRUE(ret);
    ASSERT_NE(testFactory::create(testBaseClass::getFactoryID()), nullptr);
}

TEST_F(basicFactoryTests, reRegistration){
    auto ret = testFactory::registerObject<testBaseClass>();
    ASSERT_FALSE(ret);
}

TEST_F(basicFactoryTests, multipleManualIDRegistration){
    testFactory::clearRegistry();

    auto ret = testFactory::registerObject<testBaseClass>(0);
    ASSERT_TRUE(ret);
    ret = testFactory::registerObject<testDerivA>(1);
    ASSERT_TRUE(ret);
    ret = testFactory::registerObject<testDerivB>(2);
    ASSERT_TRUE(ret);

    ASSERT_NE(testFactory::create(0), nullptr);
    ASSERT_NE(testFactory::create(1), nullptr);
    ASSERT_NE(testFactory::create(2), nullptr);
}

TEST_F(basicFactoryTests, multipleAutoIDRegistration){
    testFactory::clearRegistry();

    auto ret = testFactory::registerObject<testBaseClass>();
    ASSERT_TRUE(ret);
    ret = testFactory::registerObject<testDerivA>();
    ASSERT_TRUE(ret);
    ret = testFactory::registerObject<testDerivB>();
    ASSERT_TRUE(ret);

    ASSERT_NE(testFactory::create(testBaseClass::getFactoryID()), nullptr);
    ASSERT_NE(testFactory::create(testDerivA::getFactoryID()), nullptr);
    ASSERT_NE(testFactory::create(testDerivB::getFactoryID()), nullptr);
}

TEST_F(basicFactoryTests, createObjects){
    auto ret1 = testFactory::create(testBaseClass::getFactoryID());
    ASSERT_NE(ret1, nullptr);
    ret1->incA();
    ASSERT_EQ(ret1->a, 1);

    auto ret2 = testFactory::create(testDerivA::getFactoryID());
    ASSERT_NE(ret2, nullptr);
    ret2->incA();
    ASSERT_EQ(ret2->a, 2);

    auto ret3 = testFactory::create(testDerivB::getFactoryID());
    ASSERT_NE(ret3, nullptr);
    ret3->incA();
    ASSERT_EQ(ret3->a, 3);
}

TEST_F(basicFactoryTests, removeSingleRegistration){
    auto ret = testFactory::removeCreatorFunc(testBaseClass::getFactoryID());
    ASSERT_TRUE(ret);
    ASSERT_EQ(testFactory::create(testBaseClass::getFactoryID()), nullptr);

    auto ret2 = testFactory::create(testDerivA::getFactoryID());
    ASSERT_NE(ret2, nullptr);
    ret2->incA();
    ASSERT_EQ(ret2->a, 2);

    auto ret3 = testFactory::create(testDerivB::getFactoryID());
    ASSERT_NE(ret3, nullptr);
    ret3->incA();
    ASSERT_EQ(ret3->a, 3);
}

TEST_F(basicFactoryTests, removeRegistrations){
    auto ret = testFactory::removeCreatorFunc(testBaseClass::getFactoryID());
    ASSERT_TRUE(ret);
    ASSERT_EQ(testFactory::create(testBaseClass::getFactoryID()), nullptr);

    ret = testFactory::removeCreatorFunc(testDerivA::getFactoryID());
    ASSERT_TRUE(ret);
    ASSERT_EQ(testFactory::create(testDerivA::getFactoryID()), nullptr);

    ret = testFactory::removeCreatorFunc(testDerivB::getFactoryID());
    ASSERT_TRUE(ret);
    ASSERT_EQ(testFactory::create(testDerivB::getFactoryID()), nullptr);
}