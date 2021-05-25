//
// Created by elias on 25/05/2021.
//

//
// Created by elias on 24/05/2021.
//
#include "../libFactory.h"

#include <gtest/gtest.h>

#include <memory>

struct testBaseClass{
    typedef int __factoryIdentifierType__;
    testBaseClass(int a) : a(a) {}
    virtual void incA(){a++;}
    static int getFactoryID(){return 0;}
    int a = 0;
};

typedef libFactory::SingleArgumentFactory<testBaseClass, int> testFactory;

struct testDerivA: public testBaseClass{
    testDerivA(int a) : testBaseClass(a) {}
    void incA() override {a+=2;}
    static __factoryIdentifierType__ getFactoryID(){return 1;}
};

struct testDerivB: public testBaseClass{
    testDerivB(int a) : testBaseClass(a) {}
    void incA() override {a+=3;}
    static __factoryIdentifierType__ getFactoryID(){return 2;}
};

class singleArgumentFactoryTests: public ::testing::Test {
public:

    singleArgumentFactoryTests() = default;

protected:
    void SetUp() override {
        auto ret = testFactory::registerObjectConstructor<testBaseClass>();
        ASSERT_TRUE(ret);
        ret = testFactory::registerObjectConstructor<testDerivA>();
        ASSERT_TRUE(ret);
        ret = testFactory::registerObjectConstructor<testDerivB>();
        ASSERT_TRUE(ret);
    }
protected:
    void TearDown() override {
        testFactory::clearRegistry();
    }
    ~singleArgumentFactoryTests() override = default;
};

TEST_F(singleArgumentFactoryTests, clearRegistrations){
    testFactory::clearRegistry();
    ASSERT_EQ(testFactory::create(0, 0), nullptr);
}

TEST_F(singleArgumentFactoryTests, singleManualIDRegistration){
    testFactory::clearRegistry();

    auto ret = testFactory::registerObjectConstructor<testBaseClass>(0);
    ASSERT_TRUE(ret);
    ASSERT_NE(testFactory::create(0, 0), nullptr);
}

TEST_F(singleArgumentFactoryTests, singleAutoIDRegistration){
    testFactory::clearRegistry();

    auto ret = testFactory::registerObjectConstructor<testBaseClass>();
    ASSERT_TRUE(ret);
    ASSERT_NE(testFactory::create(testBaseClass::getFactoryID(), 0), nullptr);
}

TEST_F(singleArgumentFactoryTests, reRegistration){
    auto ret = testFactory::registerObjectConstructor<testBaseClass>();
    ASSERT_FALSE(ret);
}

TEST_F(singleArgumentFactoryTests, multipleManualIDRegistration){
    testFactory::clearRegistry();

    auto ret = testFactory::registerObjectConstructor<testBaseClass>(0);
    ASSERT_TRUE(ret);
    ret = testFactory::registerObjectConstructor<testDerivA>(1);
    ASSERT_TRUE(ret);
    ret = testFactory::registerObjectConstructor<testDerivB>(2);
    ASSERT_TRUE(ret);

    ASSERT_NE(testFactory::create(0, 0), nullptr);
    ASSERT_NE(testFactory::create(1, 0), nullptr);
    ASSERT_NE(testFactory::create(2, 0), nullptr);
}

TEST_F(singleArgumentFactoryTests, multipleAutoIDRegistration){
    testFactory::clearRegistry();

    auto ret = testFactory::registerObjectConstructor<testBaseClass>();
    ASSERT_TRUE(ret);
    ret = testFactory::registerObjectConstructor<testDerivA>();
    ASSERT_TRUE(ret);
    ret = testFactory::registerObjectConstructor<testDerivB>();
    ASSERT_TRUE(ret);

    ASSERT_NE(testFactory::create(testBaseClass::getFactoryID(), 0), nullptr);
    ASSERT_NE(testFactory::create(testDerivA::getFactoryID(), 0), nullptr);
    ASSERT_NE(testFactory::create(testDerivB::getFactoryID(), 0), nullptr);
}

TEST_F(singleArgumentFactoryTests, createObjects){
    auto ret1 = testFactory::create(testBaseClass::getFactoryID(), 1);
    ASSERT_NE(ret1, nullptr);
    ret1->incA();
    ASSERT_EQ(ret1->a, 2);

    auto ret2 = testFactory::create(testDerivA::getFactoryID(), 2);
    ASSERT_NE(ret2, nullptr);
    ret2->incA();
    ASSERT_EQ(ret2->a, 4);

    auto ret3 = testFactory::create(testDerivB::getFactoryID(), 3);
    ASSERT_NE(ret3, nullptr);
    ret3->incA();
    ASSERT_EQ(ret3->a, 6);
}

TEST_F(singleArgumentFactoryTests, removeSingleRegistration){
    auto ret = testFactory::removeCreatorFunc(testBaseClass::getFactoryID());
    ASSERT_TRUE(ret);
    ASSERT_EQ(testFactory::create(testBaseClass::getFactoryID(), 0), nullptr);

    auto ret2 = testFactory::create(testDerivA::getFactoryID(), 0);
    ASSERT_NE(ret2, nullptr);
    ret2->incA();
    ASSERT_EQ(ret2->a, 2);

    auto ret3 = testFactory::create(testDerivB::getFactoryID(), 0);
    ASSERT_NE(ret3, nullptr);
    ret3->incA();
    ASSERT_EQ(ret3->a, 3);
}

TEST_F(singleArgumentFactoryTests, removeRegistrations){
    auto ret = testFactory::removeCreatorFunc(testBaseClass::getFactoryID());
    ASSERT_TRUE(ret);
    ASSERT_EQ(testFactory::create(testBaseClass::getFactoryID(), 0), nullptr);

    ret = testFactory::removeCreatorFunc(testDerivA::getFactoryID());
    ASSERT_TRUE(ret);
    ASSERT_EQ(testFactory::create(testDerivA::getFactoryID(), 0), nullptr);

    ret = testFactory::removeCreatorFunc(testDerivB::getFactoryID());
    ASSERT_TRUE(ret);
    ASSERT_EQ(testFactory::create(testDerivB::getFactoryID(), 0), nullptr);
}

